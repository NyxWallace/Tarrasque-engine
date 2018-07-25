#include "TE_Engine.h"

//////////////
// DLL MAIN //
//////////////

#ifdef WIN32
#include <Windows.h>

/**
* DLL entry point. Avoid to rely on it for easier code portability (Linux doesn't use this method).
* @param instDLL handle
* @param reason reason
* @param _reserved reserved
* @return true on success, false on failure
*/
int APIENTRY DllMain(HANDLE instDLL, DWORD reason, LPVOID _reserved)
{
	// Check use:
	switch (reason)
	{
		///////////////////////////
	case DLL_PROCESS_ATTACH: //
		break;


		///////////////////////////
	case DLL_PROCESS_DETACH: //
		break;
	}

	// Done:
	return true;
}
#endif

// Shaders:
TE_Shader shader;
// Passthrough shader:
TE_Shader *passthroughShader;
TE_Shader spot_shader;
// Texture:
GLuint globalTex;
GLuint fboTexId, right_fboTexId, fboVbo, fboTexVbo;
//FBO
TE_Fbo *fbo, *right_fbo;



/////////////
// SHADERS //
/////////////

//////////////////////////////////////////
char *vertShader = R"(
   #version 440 core

   // Uniforms:
   uniform mat4 projection;
   uniform mat4 modelview;
   uniform mat3 normalMatrix;

   // Attributes:
   layout(location = 0) in vec3 in_Position;
   layout(location = 1) in vec3 in_Normal;
   layout(location = 2) in vec2 in_Texture;

   // Varying:
   out vec4 fragPosition;
   out vec3 normal; 
   out vec2 texCoord;  

   void main(void)
   {
      fragPosition = modelview * vec4(in_Position, 1.0);
      gl_Position = projection * fragPosition;      
      normal = normalMatrix * in_Normal;
	  texCoord = in_Texture;			 
   }
)";

//////////////////////////////////////////
char *fragShader = R"(
   #version 440 core

   in vec4 fragPosition;
   in vec3 normal;   
   in vec2 texCoord;
   
   out vec4 fragOutput;

   // Material properties:
   uniform vec3 matAmbient;
   uniform vec3 matDiffuse;
   uniform vec3 matSpecular;
   uniform float matShininess;

   // Light properties:
   uniform vec3 lightPosition; 
   uniform vec3 lightAmbient; 
   uniform vec3 lightDiffuse; 
   uniform vec3 lightSpecular;

   // Texture mapping:
   uniform sampler2D texSampler;

   void main(void)
   {    
      // Texture element:
      vec4 texel = texture(texSampler, texCoord);  

      // Ambient term:
      vec3 fragColor = matAmbient * lightAmbient;

      // Diffuse term:
      vec3 _normal = normalize(normal);
      vec3 lightDirection = normalize(lightPosition - fragPosition.xyz);      
      float nDotL = dot(lightDirection, _normal);   
      if (nDotL > 0.0)
      {
         fragColor += matDiffuse * nDotL * lightDiffuse;
      
         // Specular term:
         vec3 halfVector = normalize(lightDirection + normalize(-fragPosition.xyz));                     
         float nDotHV = dot(_normal, halfVector);         
         fragColor += matSpecular * pow(nDotHV, matShininess) * lightSpecular;
      } 
      
      // Final color:
      fragOutput = texel * vec4(fragColor, 1.0);
   }
)";

//////////////////////
// Passthrough shader:
//////////////////////
char *passthroughVS = R"(
   #version 440 core

   // Uniforms:
   uniform mat4 projection;
   uniform mat4 modelview;   

   // Attributes:
   layout(location = 0) in vec2 in_Position;   
   layout(location = 2) in vec2 in_Texture;

   // Varying:   
   out vec2 texCoord;

   void main(void)
   {      
      gl_Position = projection * modelview * vec4(in_Position, 0.0, 1.0);    
      texCoord = in_Texture;
   }
)";

char *passthroughFS = R"(
   #version 440 core
   
   in vec2 texCoord;
   
   uniform vec4 color;

   out vec4 fragOutput;   

   // Texture mapping:
   uniform sampler2D texSampler;

   void main(void)   
   {  
      // Texture element:
      vec4 texel = texture(texSampler, texCoord);      
      
      // Final color:
      fragOutput = color * texel; 
   }
)";



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// NON MEMBER METHODS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* Debug message callback for OpenGL. See https://www.opengl.org/wiki/Debug_Output
*/
void __stdcall DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
	std::cout << "OpenGL says: \"" << std::string(message) << "\"" << std::endl;
}

/**
* This callback is invoked each time the window gets resized (and once also when created).
* @param width new window width
* @param height new window height
*/
void reshapeCallback(int width, int height)
{
	// For your information...:
	std::cout << "[reshape func invoked]" << std::endl;

	TE_Engine *engine = TE_Engine::get_istance();
	// Update viewport size:
	glViewport(0, 0, width, height);

	// Update matrices:
	engine->set_perspective(glm::perspective(glm::radians(engine->get_fov()), (float)width / (float)height, engine->get_near_plane(), engine->get_far_plane()));
	engine->set_ortho(glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f));
	engine->set_fbo_perspective(glm::perspective(glm::radians(engine->get_fov()), (float)width / (float)height, engine->get_near_plane(), engine->get_far_plane()));
	engine->get_skybox()->set_proj(engine->get_perspective());

	// (bad) trick to avoid window resizing:
	if (width != engine->m_width || height != engine->m_height)
		glutReshapeWindow((int)engine->m_width, (int)engine->m_height);
}

/**
* Display method for openGL. We can also use an external displayFunction if the client provides one.
*/
void displayCallback(){
	TE_Engine *engine = TE_Engine::get_istance();

	// Clear main frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (engine->get_stereo_rendering()){
		// Switch to render on the left eye fbo
		fbo->render();
		// Clear the buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//////
		// 3D:		
		//Render Skybox
		engine->SkyBox->set_far_plane(engine->get_far_plane());
		engine->SkyBox->set_rotation(engine->get_camera()->get_position());
		engine->SkyBox->render();

		// Activate per pixel rendering shader
		shader.render();

		// Render left eye on a FBO
		engine->apply_left_frustum();
		// Get camera inverse
		TE_MAT4 inverse = engine->get_camera()->inverse();
		
		// Render list
		engine->get_scene()->render(inverse, &shader);

		// Render user display callback
		if (engine->m_displayfPtr != nullptr){
			engine->m_displayfPtr();
		}

		// Done with the left eye fbo
		TE_Fbo::disable();

		// Reset viewport
		glViewport(0, 0, (GLsizei)engine->m_width, (GLsizei)engine->m_height);

		//Reset camera
		engine->get_camera()->translate(TE_VEC3(engine->get_eye_separation() / 2, 0.0f, 0.0f));

		// Render right eye on the other FBO
		right_fbo->render();
		// Clear buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//////
		// 3D:
		//Render Skybox
		engine->SkyBox->set_far_plane(engine->get_far_plane());
		engine->SkyBox->set_rotation(engine->get_camera()->get_position());
		engine->SkyBox->render();

		// Activate per pixel rendering shader
		shader.render();

		// Apply right eye frustum
		engine->apply_right_frustum();
		// Get camera inverse
		inverse = engine->get_camera()->inverse();

		// Render list
		engine->get_scene()->render(inverse, &shader);

		// Render user display callback
		if (engine->m_displayfPtr != nullptr){
			engine->m_displayfPtr();
		}

		// Done with the right eye fbo
		TE_Fbo::disable();

		// Reset viewport
		glViewport(0, 0, engine->m_width, engine->m_height);
		//Reset camera
		engine->get_camera()->translate(TE_VEC3(-engine->get_eye_separation() / 2, 0.0f, 0.0f));

		//////
		// 2D:
		// Activate and setup passthrough shader to rende both left and right eye
		passthroughShader->render();
		passthroughShader->setMatrix(passthroughShader->projLoc, engine->get_ortho());
		passthroughShader->setMatrix(passthroughShader->mvLoc, TE_MAT4());
		passthroughShader->setInt(passthroughShader->texSamplerLoc, 0);
		passthroughShader->setVec4(passthroughShader->colorLoc, glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

		// Binding vbos
		glBindBuffer(GL_ARRAY_BUFFER, fboVbo);
		glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		// Disable in_Normal
		//glDisableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, fboTexVbo);
		glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(2);

		// Setting color mask to red to render left eye
		glColorMask(true, false, false, false);
		glBindTexture(GL_TEXTURE_2D, fboTexId);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Clearing depth buffer
		//glClear(GL_DEPTH_BUFFER_BIT);

		// Setting color mask to cyan to render right eye.
		glColorMask(false, true, true, false);
		glBindTexture(GL_TEXTURE_2D, right_fboTexId);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Setting color mask to normal
		glColorMask(true, true, true, true);

	}
	else{
		// Rendering to left fbo
		fbo->render();

		// Clearing buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//Render Skybox
		engine->SkyBox->set_far_plane(engine->get_far_plane());
		engine->SkyBox->set_rotation(engine->get_camera()->get_position());
		engine->SkyBox->render();

		//////
		// 3D:
		// Activate per pixel lighting shader
		shader.render();
		//Setting projection matrix
		shader.setMatrix(shader.projLoc, engine->get_fbo_perspective());
		
		// Get camera inverse
		TE_MAT4 inverse = engine->get_camera()->inverse();

		// Render list
		engine->get_scene()->render(inverse, &shader);

		// Render user display callback
		if (engine->m_displayfPtr != nullptr){
			engine->m_displayfPtr();
		}

		// Done with the fbo
		TE_Fbo::disable();
		// Reset viewport
		glViewport(0, 0, engine->m_width, engine->m_height);

		//////
		// 2D:
		// Activate and setup passthrough shader
		passthroughShader->render();
		passthroughShader->setMatrix(passthroughShader->projLoc, engine->get_ortho());
		passthroughShader->setMatrix(passthroughShader->mvLoc, TE_MAT4());
		passthroughShader->setInt(passthroughShader->texSamplerLoc, 0);
		passthroughShader->setVec4(passthroughShader->colorLoc, glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

		// Bind fbos
		glBindBuffer(GL_ARRAY_BUFFER, fboVbo);
		glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		//glDisableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, fboTexVbo);
		glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(2);

		// Render texture
		glBindTexture(GL_TEXTURE_2D, fboTexId);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	char text[64]; //@todo: make it static
	snprintf(text, sizeof(text), "FPS: %.1f", engine->m_fps);
	//*********************************************************************************************
	std::cout << "fps: " << TE_Engine::get_istance()->m_fps << std::endl;
	
	// Swap buffers
	glutSwapBuffers();

	TE_Engine::get_istance()->m_frames++;
	// Force rendering refresh
	glutPostWindowRedisplay(TE_Engine::get_istance()->m_windowId);
}

/**
* Method to call a keyboard callback from a client.
* @param key code of the key pressed
* @param mouse_x mouse's x coordinate
* @param mouse_y mouse's y coordinate
*/
void keyboardCallback(unsigned char key, int mouse_x, int mouse_y){
	if (TE_Engine::get_istance()->m_keyboardfPtr != nullptr){
		TE_Engine::get_istance()->m_keyboardfPtr(key, mouse_x, mouse_y);
	}
}

/**
* Method to call a special keyboard callback froma client. This is used
* for arrow keys and other special keys.
* @param key code of the key pressed
* @param mouse_x mouse's x coordinate
* @param mouse_y mouse's y coordinate
*/
void specialCallback(int key, int mouse_x, int mouse_y){
	if (TE_Engine::get_istance()->m_specialfPtr != nullptr){
		TE_Engine::get_istance()->m_specialfPtr(key, mouse_x, mouse_y);
	}
}

/**
* This callback is invoked every second. We use this callback to compute the FPS.
* @param value passepartout value
*/
void timerCallback(int value)
{
	TE_Engine::get_istance()->m_fps = TE_Engine::get_istance()->m_frames / 1.0f;
	TE_Engine::get_istance()->m_frames = 0;

	// Register the next update:
	glutTimerFunc(1000, timerCallback, 0);
}
/**
* Method to call a user defined idle callback.
*/
void idleCallback(){
	if (TE_Engine::get_istance()->m_idlefPtr != nullptr){
		TE_Engine::get_istance()->m_idlefPtr();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ENGINE METHODS IMPLEMENTATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Static variable for signleton pattern.
*/
TE_Engine* TE_Engine::m_istance = nullptr;

/**
* Static variables for list and camera
*/
TE_Camera* TE_Engine::m_camera = new TE_Camera("Default_camera");
TE_List* TE_Engine::m_scene = new TE_List();
int TE_Engine::m_anis_level = 16;
unsigned int TE_Engine::vao = 0;

/**
* Method to render a scene.
* @param camera the camera object
* @param list the list of objects to render
*/
LIB_API void TE_Engine::render(TE_Camera* camera, TE_List* list){
	m_camera = camera;
	m_scene = list;
}

/**
* Initialiation method for the engine, creates te sigleton istance by calling get_istance().
* @return the engine istance
*/
LIB_API TE_Engine* TE_Engine::init(){
	return get_istance();
}

/**
* Singleton method to generate a unique instance of TE_Engine.
* @return a new istance of Engine if it is not yet instantiated
*/
LIB_API TE_Engine* TE_Engine::get_istance(){
	if (m_istance == nullptr){
		m_istance = new TE_Engine();
		m_istance->m_displayfPtr = nullptr;
		m_istance->m_keyboardfPtr = nullptr;
		m_istance->m_specialfPtr = nullptr;
		m_istance->m_idlefPtr = nullptr;
	}
	return m_istance;
}

/**
* Method to start the glut main loop.
*/
void LIB_API TE_Engine::start(){
	glutMainLoop();
}

/**
* Recursive method used to parse the scene we are loading.
* @param node node to parse
* @param scene the scene to which the node belongs
* @return a node of the scene graph
*/
LIB_API TE_Node* TE_Engine::load_node(aiNode *node, aiScene *scene){

	// TE_Node name:
	aiString name = node->mName;

	// TE_Node matrix:
	aiMatrix4x4 nodeMatrix = node->mTransformation;

	// Convert aiMatrix into an OpenGL matrix:
	glm::mat4 matrix;
	memcpy(&matrix, &nodeMatrix, sizeof(TE_MAT4));
	matrix = glm::transpose(matrix);

	// Store vertices of the mesh
	// Check for meshes:
	if (node->mNumMeshes)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[0]];
		// Alloc memory space for vertexes
		float* vertex = (float*)malloc(mesh->mNumVertices * 3 * sizeof(float));
		float* normals = (float*)malloc(mesh->mNumVertices * 3 * sizeof(float));
		float* texture_coords = (float*)malloc(mesh->mNumVertices * 2 * sizeof(float));
		float* colors = (float*)malloc(mesh->mNumVertices * 4 * sizeof(float));
		// Load vertexes into arrays
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			vertex[i * 3] = mesh->mVertices[i].x;
			vertex[i * 3 + 1] = mesh->mVertices[i].y;
			vertex[i * 3 + 2] = mesh->mVertices[i].z;

			normals[i * 3] = mesh->mNormals[i].x;
			normals[i * 3 + 1] = mesh->mNormals[i].y;
			normals[i * 3 + 2] = mesh->mNormals[i].z;

			if (mesh->HasTextureCoords(0)){
				texture_coords[i * 2] = mesh->mTextureCoords[0][i].x;
				texture_coords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
			}
			else {
				texture_coords[i * 2] = 1.0f;
				texture_coords[i * 2 + 1] = 1.0f;
			}
		}

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			colors[i * 4] = 0.0f;
			colors[i * 4 + 1] = 1.0f;
			colors[i * 4 + 2] = 0.0f;
			colors[i * 4 + 3] = 1.0f;
		}

		// VBO Binding
		unsigned int vertexVbo, normalsVbo, texture_coordsVbo, colorsVbo;

		glGenBuffers(1, &vertexVbo);
		glGenBuffers(1, &normalsVbo);
		glGenBuffers(1, &texture_coordsVbo);
		glGenBuffers(1, &colorsVbo);

		glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
		glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(float), vertex, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, normalsVbo);
		glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(float), normals, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, texture_coordsVbo);
		glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 2 * sizeof(float), texture_coords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, colorsVbo);
		glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 4 * sizeof(float), colors, GL_STATIC_DRAW);

		// Free arrays for vertexes
		free(vertex);
		free(normals);
		free(texture_coords);
		free(colors);

		// Get material properties
		TE_Material *material = new TE_Material();
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

			aiString matName;
			mat->Get(AI_MATKEY_NAME, matName);
			//std::cout << " Name    : " << matName.C_Str() << std::endl;

			// Get properties:
			aiColor4D ambient;
			mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			//std::cout << " Ambient : " << ambient.r << ", " << ambient.g << ", " << ambient.b << ", " << ambient.a << std::endl;

			aiColor4D diffuse;
			mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			//std::cout << " Diffuse : " << diffuse.r << ", " << diffuse.g << ", " << diffuse.b << ", " << diffuse.a << std::endl;

			aiColor4D specular;
			mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			//std::cout << " Specular: " << specular.r << ", " << specular.g << ", " << specular.b << ", " << specular.a << std::endl;

			aiColor4D emissive;
			mat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);

			float shininess, shininessStrength, opacity;
			mat->Get(AI_MATKEY_SHININESS, shininess);
			mat->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);
			//mat->Get(AI_MATKEY_OPACITY, opacity);
			opacity = 1.0f;
			//std::cout << " Opacità: " << opacity << std::endl;
			//std::cout << " Shinin. : " << shininess << " " << shininessStrength << std::endl;

			TE_Texture* texture = nullptr;
			if (mat->GetTextureCount(aiTextureType_DIFFUSE))
			{
				aiString textureName;
				mat->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), textureName);
				//std::cout << "  TE_Texture : " << textureName.C_Str() << std::endl;

				texture = new TE_Texture(textureName.C_Str(), 16);
			}
			else{
				// Set default texture for untextured materials
				texture = new TE_Texture(globalTex);
			}

			material->set_ambient(TE_VEC4(ambient.r, ambient.g, ambient.b, opacity));
			material->set_diffuse(TE_VEC4(diffuse.r, diffuse.g, diffuse.b, opacity));
			material->set_specular(TE_VEC4(specular.r, specular.g, specular.b, opacity));
			material->set_emission(TE_VEC4(emissive.r, emissive.g, emissive.b, opacity));
			material->set_opacity(opacity);
			material->set_shininess(shininess);
			material->set_texture(texture);
			material->set_shader(&shader);
		}

		// Set root position coordinates
		TE_Mesh *meshNode = new TE_Mesh(name.C_Str(), mesh->mNumVertices, vertexVbo, normalsVbo, texture_coordsVbo, colorsVbo, material);
		meshNode->set_position(matrix);

		for (unsigned int c = 0; c < node->mNumChildren; c++)
			meshNode->link(load_node(node->mChildren[c], scene));
		std::cout << ".";
		return meshNode;
	}

	std::string lightName = node->mName.C_Str();
	if (lightName.find(".Target") != std::string::npos){
		//std::cout << " node skipped" << std::endl;
		return nullptr;
	}

	// Load lights
	aiLight *light;
	for (unsigned int c = 0; c < scene->mNumLights; c++)
	{
		if (scene->mLights[c]->mName == node->mName)
			light = scene->mLights[c];
	}
	if (light != nullptr){

		//std::cout << "ANGLES: " << light->mAngleInnerCone << "\t" << light->mAngleOuterCone << std::endl;
		//std::cout << "Type: " << light->mType << std::endl;
		//std::cout << "LIGHT POSITION: " << light->mPosition.x << "," << light->mPosition.y << "," << light->mPosition.z << std::endl;

		TE_Light *lightNode = new TE_Light(name.C_Str(), light->mType);
		lightNode->set_ambient(TE_VEC4(light->mColorDiffuse.r*0.1, light->mColorDiffuse.g*0.1, light->mColorDiffuse.b*0.1, 1.0f));
		lightNode->set_diffuse(TE_VEC4(light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b, 1.0f));
		lightNode->set_specular(TE_VEC4(light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b, 1.0f));
		lightNode->set_cutoff(glm::degrees(light->mAngleInnerCone) / 2);
		lightNode->set_shader(&shader);
		//std::cout << " Ambient: " << light->mColorAmbient.r << ", " << light->mColorAmbient.g << ", " << light->mColorAmbient.b << std::endl;
		//std::cout << " Diffuse: " << light->mColorDiffuse.r << ", " << light->mColorDiffuse.g << ", " << light->mColorDiffuse.b << std::endl;
		//std::cout << " Specular: " << light->mColorSpecular.r << ", " << light->mColorSpecular.g << ", " << light->mColorSpecular.b << std::endl;

		lightNode->set_position(matrix);
		lightNode->set_light_pos(TE_VEC3(light->mPosition.x, light->mPosition.y, light->mPosition.z));
		lightNode->set_attenuation(light->mAttenuationConstant, light->mAttenuationLinear, light->mAttenuationQuadratic);

		if (light->mType == aiLightSource_SPOT){
			lightNode->set_direction(TE_VEC3(light->mDirection.x, light->mDirection.y, light->mDirection.z));
			lightNode->set_shader(&spot_shader);
			/*std::cout << " Spot direction x: " << light->mDirection.x << " y: " << light->mDirection.y << " z: " << light->mDirection.z << std::endl;
			std::cout << " Spot direction: " << glm::to_string(lightNode->get_direction()) << std::endl;*/
		}

		for (unsigned int c = 0; c < node->mNumChildren; c++)
			lightNode->link(load_node(node->mChildren[c], scene));

		return lightNode;
	}

	TE_Node *simpleNode = new TE_Node(name.C_Str());
	simpleNode->set_position(matrix);

	for (unsigned int c = 0; c < node->mNumChildren; c++)
		simpleNode->link(load_node(node->mChildren[c], scene));

	return simpleNode;
}

/**
* Method to load a scene graph from a file
* @param path the path to the file to load
* @return the root node of the loaded scene
*/
LIB_API TE_Node* TE_Engine::load_scene(std::string path){
	std::cout << "Loading scene..." << std::endl;
	// Load scene:
	Assimp::Importer importer;
	aiScene *scene = (aiScene *)importer.ReadFile(path, 0);

	if (scene == NULL)
	{
		std::cout << "ERROR: unable to load the scene" << std::endl;
		return nullptr;
	}

	aiNode *node = scene->mRootNode;

	// TE_Node name:
	aiString name = node->mName;
	//std::cout << "TE_Node '" << name.C_Str() << "'" << std::endl;

	// TE_Node matrix:
	aiMatrix4x4 nodeMatrix = node->mTransformation;

	// Convert aiMatrix into an OpenGL matrix:
	glm::mat4 matrix;
	memcpy(&matrix, &nodeMatrix, sizeof(glm::mat4));
	matrix = glm::transpose(matrix);

	// Set root position coordinates
	TE_Node *root = new TE_Node(name.C_Str());
	root->set_position(matrix);
	//std::cout << " Position: " << matrix[3][0] << ", " << matrix[3][1] << ", " << matrix[3][2] << std::endl;

	for (unsigned int c = 0; c < node->mNumChildren; c++)
		root->link(load_node(node->mChildren[c], scene));

	std::cout << std::endl << "Loading complete" << std::endl;
	// Scene summary:
	std::cout << std::endl << "Scene summary: " << std::endl;
	std::cout << "   Objects  : " << scene->mNumMeshes << std::endl;
	std::cout << "   Lights   : " << scene->mNumLights << std::endl;
	std::cout << "   Materials: " << scene->mNumMaterials << std::endl;
	std::cout << "   Anims    : " << scene->mNumAnimations << std::endl;
	std::cout << "   Cameras  : " << scene->mNumCameras << std::endl;
	std::cout << "   Textures : " << scene->mNumTextures << std::endl;
	return root;
}

/**
* Camera getter.
* @return the camera
*/
LIB_API TE_Camera* TE_Engine::get_camera(){
	return m_camera;
}

/**
* Scene list getter.
* @return the list
*/
LIB_API TE_List* TE_Engine::get_scene(){
	return m_scene;
}

/**
* Anisotropic filter level getter.
* @return the anisotropy level
*/
LIB_API int TE_Engine::get_anis_level(){
	return m_anis_level;
}

/**
* Method to pass a custom display callback function from the client.
* @param f function pointer to the custom display callback
*/
void LIB_API TE_Engine::set_display_callback(void(*f)()){
	m_istance->m_displayfPtr = f;
}

/**
* Method to pass a custom keyboard callback function from the client.
* @param f function pointer to the custom keyboard callback
*/
void LIB_API TE_Engine::set_keyboard_callback(void(*f)(unsigned char, int, int)){
	m_istance->m_keyboardfPtr = f;
}

/**
* Method to pass a custom display special function from the client.
* @param f function pointer to the custom special callback
*/
void LIB_API TE_Engine::set_special_callback(void(*f)(int, int, int)){
	m_istance->m_specialfPtr = f;
}

/**
* Method to pass a custom idle callback function from the client.
* @param f function pointer to the custom idle callback
*/
void LIB_API TE_Engine::set_idle_callback(void(*f)()){
	m_istance->m_idlefPtr = f;
}

/**
* Method to switch to a Gouraud shading model.
*/
void LIB_API TE_Engine::smooth_shading(){
	glShadeModel(GL_SMOOTH);
}

/**
* Method to switch to a flat shading model.
*/
void LIB_API TE_Engine::flat_shading(){
	glShadeModel(GL_FLAT);
}

/**
* Method to start 3d rendering.
*/
void LIB_API TE_Engine::tri_start(){
	//GLint view[4];
	//glGetIntegerv(GL_VIEWPORT, view);
	// Update the projection matrix:
	/*glMatrixMode(GL_PROJECTION);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)view[2] / (float)view[3], 1.0f, 1000.0f);
	glLoadMatrixf(glm::value_ptr(projection));
	glMatrixMode(GL_MODELVIEW);*/

	// Clear the screen:
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGBA components
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
* Method to end 3d rendering.
*/
void LIB_API TE_Engine::tri_end(){
	//glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
}

/**
* Method to start 2d rendering.
*/
void LIB_API TE_Engine::bi_start(){
	// Set orthographic projection:
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	/*glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(glm::ortho(0.0f, (float)viewport[2], 0.0f, (float)viewport[3], -1.0f, 1.0f)));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(glm::mat4(1.0)));*/

	// Disable lighting before rendering 2D text:
	//glDisable(GL_LIGHTING);
}

/**
* Method to end 2d rendering, we assume that we want to restart a 3d render after a 2d one.
*/
void LIB_API TE_Engine::bi_end(){
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	//glEnable(GL_LIGHTING);
	// Update the projection matrix:
	/*glMatrixMode(GL_PROJECTION);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewport[2] / (float)viewport[3], 1.0f, 1000.0f);
	glLoadMatrixf(glm::value_ptr(projection));
	glMatrixMode(GL_MODELVIEW);*/
}

/**
* Method to write a string on the screen.
* Notice that the point (0,0) is the lower left corner of the window.
* @param s the string to write on the screen
* @param x the x coordinate on the screen
* @param y the y coordinate on the screen
*/
void LIB_API TE_Engine::write_string(const char* s, float x, float y){
	// Write some text:
	/*glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2f(x, y);
	glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char *)s);*/
}

/**
* Method to set clockwise face culling.
*/
void LIB_API TE_Engine::cw_face_culling(){
	glFrontFace(GL_CW);
}

/**
* Method to set counter-clockwise face culling.
*/
void LIB_API TE_Engine::ccw_face_culling(){
	glFrontFace(GL_CCW);
}

/**
* Method to disable lighting.
*/
void LIB_API TE_Engine::disable_lighting(){
	//glDisable(GL_LIGHTING);
}

/**
* Method to enable lighting
*/
void LIB_API TE_Engine::enable_lighting(){
	//glEnable(GL_LIGHTING);
}
/**
* Method to swap the buffers.
*/
void LIB_API TE_Engine::swap(){
	glutSwapBuffers();
}

/**
* Method call the redisplay function.
*/
void LIB_API TE_Engine::display(){
	glutPostWindowRedisplay(get_istance()->m_windowId);
}

/**
* Method to define a depth function.
* @param gl_function depth test function.
*/
void LIB_API TE_Engine::depth_function(GLenum gl_function){
	glDepthFunc(gl_function);
}

/**
* Method to set the stereoscopic rendering flag.
* @param flag the flag to set the stereo rendering
*/
void LIB_API TE_Engine::set_stereo_rendering(bool flag){
	m_stereo_rendering = flag;
}

/**
* Method to get the stereo rendering flag.
*/
LIB_API bool TE_Engine::get_stereo_rendering(){
	return m_stereo_rendering;
}

/**
* Toggle for stereo rendering.
*/
void LIB_API TE_Engine::toggle_stereo_rendering(){
	m_stereo_rendering = !m_stereo_rendering;
}

/**
* Method used to generate a global texture for untextured materials.
*/
void TE_Engine::gen_global_texture(){
	glGenTextures(1, &globalTex);
	glBindTexture(GL_TEXTURE_2D, globalTex);
	GLubyte  pixel[1][1][4] = { 255.0f, 255.0f, 255.0f, 255.0f };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

}

/**
* Method used to generate the fbo's used for rendering.
*/
void TE_Engine::gen_fbo_texture(){
	glGenTextures(1, &fboTexId);
	glBindTexture(GL_TEXTURE_2D, fboTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //_MIPMAP_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &right_fboTexId);
	glBindTexture(GL_TEXTURE_2D, right_fboTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //_MIPMAP_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

/**
* Method used to apply a frustum when we render anaglyphs, this applies
* the frustum for the left eye.
*/
void TE_Engine::apply_left_frustum(){
	float top, bottom, left, right;
	float radians_fov = glm::radians(this->m_fov);

	top = this->m_near_plane * tan(radians_fov / 2);
	bottom = -top;

	float a = this->m_aspect_ratio * tan(radians_fov / 2) * this->m_convergence;

	float b = a - this->m_eye_separation / 2;
	float c = a + this->m_eye_separation / 2;

	left = -b * this->m_near_plane / this->m_convergence;
	right = c * this->m_near_plane / this->m_convergence;

	// Set the Projection Matrix
	shader.setMatrix(shader.projLoc, glm::frustum(left, right, bottom, top, this->m_near_plane, this->m_far_plane));

	// Displace camera to left
	m_camera->translate(TE_VEC3(-this->m_eye_separation / 2, 0.0f, 0.0f));
}

/**
* Method used to apply a frustum when we render anaglyphs, this applies
* the frustum for the right eye.
*/
void TE_Engine::apply_right_frustum(){
	float top, bottom, left, right;
	float radians_fov = glm::radians(this->m_fov);

	top = this->m_near_plane* tan(radians_fov / 2);
	bottom = -top;

	float a = this->m_aspect_ratio  * tan(radians_fov / 2) * this->m_convergence;

	float b = a - this->m_eye_separation / 2;
	float c = a + this->m_eye_separation / 2;

	left = -c * this->m_near_plane / this->m_convergence;
	right = b * this->m_near_plane / this->m_convergence;

	// Set the Projection Matrix
	shader.setMatrix(shader.projLoc, glm::frustum(left, right, bottom, top, this->m_near_plane, this->m_far_plane));
	// Displace camera to right
	m_camera->translate(TE_VEC3(this->m_eye_separation / 2, 0.0f, 0.0f));
	//std::cout << glm::to_string(m_camera->get_position()) << std::endl;
}

/**
* Method to generate the vertex buffer objects for the fbo's.
*/
void TE_Engine::gen_fbo_vbo(){
	// Create a 2D box for screen rendering:
	glm::vec2 *boxPlane = new glm::vec2[4];
	glm::vec2 *texCoord = new glm::vec2[4];
	boxPlane[0] = glm::vec2(0.0f, 0.0f);
	boxPlane[1] = glm::vec2(m_width, 0.0f);
	boxPlane[2] = glm::vec2(0.0f, m_height);
	boxPlane[3] = glm::vec2(m_width, m_height);

	// Copy data into VBOs:
	glGenBuffers(1, &fboVbo);
	glBindBuffer(GL_ARRAY_BUFFER, fboVbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), boxPlane, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	delete[] boxPlane;

	texCoord[0] = glm::vec2(0.0f, 0.0f);
	texCoord[1] = glm::vec2(1.0f, 0.0f);
	texCoord[2] = glm::vec2(0.0f, 1.0f);
	texCoord[3] = glm::vec2(1.0f, 1.0f);
	glGenBuffers(1, &fboTexVbo);
	glBindBuffer(GL_ARRAY_BUFFER, fboTexVbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), texCoord, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	//glEnableVertexAttribArray(2);
	delete[] texCoord;
}

/**
* Method to build the used shaders
*/
void TE_Engine::build_shaders(){
	// Compile shaders:
	TE_Shader vs;
	vs.loadFromMemory(TE_Shader::TYPE_VERTEX, vertShader);

	TE_Shader ps;
	ps.loadFromMemory(TE_Shader::TYPE_FRAGMENT, fragShader);
	shader.build(&vs, &ps);

	TE_Shader _passthroughVS;
	_passthroughVS.loadFromMemory(TE_Shader::TYPE_VERTEX, passthroughVS);

	TE_Shader _passthroughFS;
	_passthroughFS.loadFromMemory(TE_Shader::TYPE_FRAGMENT, passthroughFS);

	passthroughShader = new TE_Shader();
	passthroughShader->build(&_passthroughVS, &_passthroughFS);
}

/**
* Method to generate framebuffer ID for anaglyph rendering
*/
void TE_Engine::gen_fbo_ids(){
	// Generate left (or default) fbo
	fbo = new TE_Fbo();
	fbo->bindTexture(0, TE_Fbo::BIND_COLORTEXTURE, fboTexId);
	fbo->bindRenderBuffer(1, TE_Fbo::BIND_DEPTHBUFFER, m_width, m_height);
	if (!fbo->isOk())
		std::cout << "[ERROR] FBO not valid" << std::endl;

	// Generate right fbo
	right_fbo = new TE_Fbo();
	right_fbo->bindTexture(0, TE_Fbo::BIND_COLORTEXTURE, right_fboTexId);
	right_fbo->bindRenderBuffer(1, TE_Fbo::BIND_DEPTHBUFFER, m_width, m_height);
	if (!right_fbo->isOk())
		std::cout << "[ERROR] FBO not valid" << std::endl;
}

/**
* TE_Engine Constructor
*/
LIB_API TE_Engine::TE_Engine(): m_width(1920),m_height(1080),m_near_plane(1.0f),m_far_plane(1000.0f),m_convergence(5.0f),m_eye_separation(0.1f),m_aspect_ratio((float)m_width/m_height),m_fov(45.0f){
	// Welcome message
	std::cout << "////////////////////////////////////////////////////////////////" << std::endl;
	std::cout << "//          _____                                             //" << std::endl;
	std::cout << "//         |_   _|_ _ _ _ _ _ __ _ ___ __ _ _  _ ___          //" << std::endl;
	std::cout << "//           | |/ _` | '_| '_/ _` (_-</ _` | || / -_)         //" << std::endl;
	std::cout << "//           |_|\\__,_|_| |_| \\__,_/__/\\__, |\\_,_\\___|         //" << std::endl;
	std::cout << "//               ___ _ _  __ _(_)_ _  ___|_|                  //" << std::endl;
	std::cout << "//              / -_) ' \\/ _` | | ' \\/ -_)                    //" << std::endl;
	std::cout << "//              \\___|_||_\\__, |_|_||_\\___|                    //" << std::endl;
	std::cout << "//                       |___/                                //" << std::endl;
	std::cout << "//                                                            //" << std::endl;
	std::cout << "//                            v2.0                            //" << std::endl;
	std::cout << "//                         OpenGL 4.4                         //" << std::endl;
	std::cout << "//                          VR ready                          //" << std::endl;
	std::cout << "//                                                            //" << std::endl;
	std::cout << "//  Da Costa Nuno, Farine Antoine, Maric Adrian, Storni Niko  //" << std::endl;
	std::cout << "//                         (C) SUPSI                          //" << std::endl;
	std::cout << "//                                                            //" << std::endl;
	std::cout << "////////////////////////////////////////////////////////////////" << std::endl;

	// Init context:
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);

	// FreeGLUT can parse command-line params, in case:
	int i = 1;

	char* name = "terrasque_engine";
	glutInit(&i, &name);

	glutInitContextVersion(4, 4);
	glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);

	// Set window size:
	glutInitWindowSize(m_width, m_height);
	// Set some optional flags:
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Init FreeImage:
	FreeImage_Initialise();

	// Create the window with a specific title:
	m_windowId = glutCreateWindow("Tarrasque engine");

	// The OpenGL context is now initialized...

	// Init Glew:
	glewExperimental = GL_TRUE;
	GLenum error = glewInit();

	if (GLEW_OK != error)
	{
		std::cout << "Error: " << glewGetErrorString(error) << std::endl;
		return;
	}
	else
		if (GLEW_VERSION_4_4)
			std::cout << "Driver supports OpenGL 4.4\n" << std::endl;
		else
		{
			std::cout << "Error: OpenGL 4.4 not supported\n" << std::endl;
			return;
		}
#ifdef _DEBUG
	// Register OpenGL debug callback:
	glDebugMessageCallback((GLDEBUGPROC)DebugCallback, nullptr);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

	// Log and validate supported settings:
	std::cout << "OpenGL properties:" << std::endl;
	std::cout << "   Vendor . . . :  " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "   Driver . . . :  " << glGetString(GL_RENDERER) << std::endl;

	int oglVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &oglVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &oglVersion[1]);
	std::cout << "   Version  . . :  " << glGetString(GL_VERSION) << " [" << oglVersion[0] << "." << oglVersion[1] << "]" << std::endl;
	std::cout << "   GLSL . . . . :  " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << std::endl;

	// Set clear color:
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // RGBA components

	// Vertex Array Object for all our VBO's
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Set callback functions:
	glutDisplayFunc(displayCallback);
	glutReshapeFunc(reshapeCallback);
	glutKeyboardFunc(keyboardCallback);
	glutSpecialFunc(specialCallback);
	glutTimerFunc(3000, timerCallback, 0);
	glutIdleFunc(idleCallback);

	build_shaders();
	shader.initParam();
	passthroughShader->initParam();
	shader.render();
	
	gen_global_texture();
	gen_fbo_texture();
	gen_fbo_vbo();
	gen_fbo_ids();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//skybox
	SkyBox = new TE_SkyBox();
}


/**
* Method to free the engine resources
*/
LIB_API void TE_Engine::free_engine(){
	delete m_istance;
	delete m_camera;
	delete m_scene;
	FreeImage_DeInitialise();
}

/**
* Destructor.
*/
LIB_API TE_Engine::~TE_Engine(){
	delete m_istance;
	delete m_camera;
	delete m_scene;
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);
	FreeImage_DeInitialise();
}
