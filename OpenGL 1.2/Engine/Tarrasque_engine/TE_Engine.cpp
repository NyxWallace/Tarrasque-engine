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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// NON MEMBER METHODS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* This callback is invoked each time the window gets resized (and once also when created).
* @param width new window width
* @param height new window height
*/
void reshapeCallback(int width, int height)
{
	// For your information...:
	std::cout << "[reshape func invoked]" << std::endl;

	// Change OpenGL viewport to match the new window sizes:
	glViewport(0, 0, width, height);

	// Update the projection matrix:
	glMatrixMode(GL_PROJECTION);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 1.0f, 1000.0f);
	glLoadMatrixf(glm::value_ptr(projection));
}

/**
* Display method for openGL. We can also use an external displayFunction if the client provides one.
*/
void displayCallback(){
	// Clear the screen:
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGBA components
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	if (TE_Engine::get_istance()->m_displayfPtr != nullptr){
		TE_Engine::get_istance()->m_displayfPtr();
	}
	TE_MAT4 inverse = TE_Engine::get_istance()->get_camera()->inverse();
	TE_Engine::get_istance()->get_scene()->render(inverse);

    // Set orthographic projection:
    GLint viewport[4];
    char text[64];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(glm::ortho(0.0f, (float)viewport[2], 0.0f, (float)viewport[3], -1.0f, 1.0f)));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(glm::mat4(1.0)));

    // Disable lighting before rendering 2D text:
    glDisable(GL_LIGHTING);

    // Write some text:
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(0.0f, 5.0f);
    sprintf(text, "FPS: %.1f", TE_Engine::get_istance()->m_fps);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char *) text);

    glEnable(GL_LIGHTING);
    // Update the projection matrix:
    glMatrixMode(GL_PROJECTION);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewport[2] / (float)viewport[3], 1.0f, 1000.0f);
    glLoadMatrixf(glm::value_ptr(projection));
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();

    // Force rendering refresh
    glutPostWindowRedisplay(TE_Engine::get_istance()->m_windowId);

    TE_Engine::get_istance()->m_frames++;
	}

/**
* Method to call a keyboard callback from a client.
* @param key code of the key pressed
* @param mouse_x mouse's x coordinate
* @param mouse_y mouse's y coordinate
*/
void keyboardCallback(unsigned char key, int mouse_x, int mouse_y){
	if (TE_Engine::get_istance()->m_keyboardfPtr != nullptr){
		TE_Engine::get_istance()->m_keyboardfPtr(key,mouse_x,mouse_y);
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
		TE_Engine::get_istance()->m_specialfPtr(key,mouse_x,mouse_y);
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
int TE_Engine::m_anis_level = 0;

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
	//std::cout << "TE_Node '" << name.C_Str() << "'" << std::endl;

	// TE_Node matrix:
	aiMatrix4x4 nodeMatrix = node->mTransformation;

	// Convert aiMatrix into an OpenGL matrix:
	glm::mat4 matrix;
	memcpy(&matrix, &nodeMatrix, sizeof(TE_MAT4));
	matrix = glm::transpose(matrix);
	//std::cout << " Position: " << matrix[3][0] << ", " << matrix[3][1] << ", " << matrix[3][2] << ", " << matrix[3][3] << std::endl;

	// Store vertices of the mesh
	// Check for meshes:
	if (node->mNumMeshes)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[0]];

		vector<TE_VEC3> *vertex = new vector<TE_VEC3>;
		vector<TE_VEC3> *normals = new vector<TE_VEC3>;
		vector<TE_VEC2> *texture_coords = new vector<TE_VEC2>;
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			vertex->push_back(TE_VEC3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
			//std::cout << " x: " << mesh->mVertices[i].x << " \t " << "y: " << mesh->mVertices[i].y << " \t " << "z: " << mesh->mVertices[i].z << std::endl;

			normals->push_back(TE_VEC3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
			//std::cout << " x: " << mesh->mNormals[i].x << " \t " << "y: " << mesh->mNormals[i].y << " \t " << "z: " << mesh->mNormals[i].z << std::endl;

			if (mesh->HasTextureCoords(0)){
				texture_coords->push_back(TE_VEC2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
				//std::cout << " x: " << mesh->mTextureCoords[0][i].x << " \t " << "y: " << mesh->mTextureCoords[0][i].y << std::endl;
			}
			else {
				texture_coords->push_back(TE_VEC2(0.0, 0.0));
			}
		}
		//std::cout << "  Vertices imported: " << mesh->mNumVertices << " Vertices: " << vertex->size() << std::endl;



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

				texture = new TE_Texture(textureName.C_Str(),get_anis_level());
			}

			material->set_material(TE_VEC4(ambient.r, ambient.g, ambient.b, opacity),
				TE_VEC4(diffuse.r, diffuse.g, diffuse.b, opacity),
				TE_VEC4(specular.r, specular.g, specular.b, opacity),
				TE_VEC4(emissive.r, emissive.g, emissive.b, opacity),
				opacity,
				shininess,
				texture);
			/*char a;
			std::cin >> a;*/
		}

		// Set root position coordinates
		TE_Mesh *meshNode = new TE_Mesh(name.C_Str(), vertex, normals, texture_coords, material);
		meshNode->set_position(matrix);

		for (unsigned int c = 0; c < node->mNumChildren; c++)
			meshNode->link(load_node(node->mChildren[c], scene));
		std::cout << ".";
		return meshNode;
	}

	string lightName = node->mName.C_Str();
	if (lightName.find(".Target") != std::string::npos){
		//std::cout << " node skipped" << std::endl;
		return nullptr;
	}


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
		TE_Light *lightNode = new TE_Light(name.C_Str(),light->mType,
			TE_VEC4(light->mColorDiffuse.r*0.1, light->mColorDiffuse.g*0.1, light->mColorDiffuse.b*0.1,1.0f),
			TE_VEC4(light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b,1.0f),
			TE_VEC4(light->mColorDiffuse.r*0.1, light->mColorDiffuse.g*0.1, light->mColorDiffuse.b*0.1,1.0f),
			glm::degrees(light->mAngleInnerCone)/2
			);

		//std::cout << " Ambient: " << light->mColorAmbient.r << ", " << light->mColorAmbient.g << ", " << light->mColorAmbient.b << std::endl;
		//std::cout << " Diffuse: " << light->mColorDiffuse.r << ", " << light->mColorDiffuse.g << ", " << light->mColorDiffuse.b << std::endl;
		//std::cout << " Specular: " << light->mColorSpecular.r << ", " << light->mColorSpecular.g << ", " << light->mColorSpecular.b << std::endl;

		lightNode->set_position(matrix);
		lightNode->set_light_pos(TE_VEC3(light->mPosition.x, light->mPosition.y, light->mPosition.z));
		lightNode->set_attenuation(light->mAttenuationConstant, light->mAttenuationLinear, light->mAttenuationQuadratic);

		if (light->mType == aiLightSource_SPOT){
			lightNode->set_direction(TE_VEC3(light->mDirection.x,light->mDirection.y,light->mDirection.z));
			/*std::cout << " Spot direction x: " << light->mDirection.x << " y: " << light->mDirection.y << " z: " << light->mDirection.z << std::endl;
			std::cout << " Spot direction: " << glm::to_string(lightNode->get_direction()) << std::endl;*/
		}

		for (unsigned int c = 0; c<node->mNumChildren; c++)
			lightNode->link(load_node(node->mChildren[c], scene));

		return lightNode;
	}

	TE_Node *simpleNode = new TE_Node(name.C_Str());
	simpleNode->set_position(matrix);

	for (unsigned int c = 0; c<node->mNumChildren; c++)
		simpleNode->link(load_node(node->mChildren[c], scene));

	return simpleNode;
}

/**
* Method to load a scene graph from a file
* @param path the path to the file to load
* @return the root node of the loaded scene
*/
LIB_API TE_Node* TE_Engine::load_scene(string path){
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

	for (unsigned int c = 0; c<node->mNumChildren; c++)
		root->link(load_node(node->mChildren[c], scene));

	std::cout <<std::endl << "Loading complete" << std::endl;
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
void LIB_API TE_Engine::set_display_callback(void (*f)()){
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
	GLint view[4];
	glGetIntegerv(GL_VIEWPORT, view);
	// Update the projection matrix:
	glMatrixMode(GL_PROJECTION);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)view[2] / (float)view[3], 1.0f, 1000.0f);
	glLoadMatrixf(glm::value_ptr(projection));
	glMatrixMode(GL_MODELVIEW);

	// Clear the screen:
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGBA components
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
* Method to end 3d rendering.
*/
void LIB_API TE_Engine::tri_end(){
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
}

/**
* Method to start 2d rendering.
*/
void LIB_API TE_Engine::bi_start(){
	// Set orthographic projection:
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(glm::ortho(0.0f, (float)viewport[2], 0.0f, (float)viewport[3], -1.0f, 1.0f)));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(glm::mat4(1.0)));

	// Disable lighting before rendering 2D text:
	glDisable(GL_LIGHTING);
}

/**
* Method to end 2d rendering, we assume that we want to restart a 3d render after a 2d one.
*/
void LIB_API TE_Engine::bi_end(){
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glEnable(GL_LIGHTING);
	// Update the projection matrix:
	glMatrixMode(GL_PROJECTION);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewport[2] / (float)viewport[3], 1.0f, 1000.0f);
	glLoadMatrixf(glm::value_ptr(projection));
	glMatrixMode(GL_MODELVIEW);
}

/**
* Method to write a string on the screen.
* Notice that the point (0,0) is the lower left corner of the window.
* @param s the string to write on the screen
* @param x the x coordinate on the screen
* @param y the y coordinate on the screen
*/
void LIB_API TE_Engine::write_string(const char* s,float x, float y){
	// Write some text:
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2f(x, y);
	glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char *)s);
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
	glDisable(GL_LIGHTING);
}

/**
* Method to enable lighting
*/
void LIB_API TE_Engine::enable_lighting(){
	glEnable(GL_LIGHTING);
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

void LIB_API TE_Engine::depth_function(GLenum gl_function){
	glDepthFunc(gl_function);
}
/**
* TE_Engine Constructor
*/
LIB_API TE_Engine::TE_Engine(){
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
	std::cout << "//                            v1.0                            //" << std::endl;
	std::cout << "//                                                            //" << std::endl;
	std::cout << "//        Da Costa Nuno, Farine Antoine, Maric Adrian  	      //" << std::endl;
	std::cout << "//                                                            //" << std::endl;
	std::cout << "////////////////////////////////////////////////////////////////" << std::endl;

	// Init context:
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);

	// FreeGLUT can parse command-line params, in case:
	int i = 1;
	char* name = new char[64];
	strcpy(name, "tarrasque_engine");
	glutInit(&i, &name);

	// Set some optional flags:
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Init FreeImage:
	FreeImage_Initialise();

	// Create the window with a specific title:
	m_windowId = glutCreateWindow("Tarrasque engine");

	// The OpenGL context is now initialized...


	// Check for anistropic filtering extension:
	if (strstr((const char *)glGetString(GL_EXTENSIONS), "GL_EXT_texture_filter_anisotropic"))
	{
		std::cout << "   Anisotropic filtering supported" << std::endl;
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_anis_level);
		std::cout << "   Anisotropic filtering max. level: " << m_anis_level << std::endl;
	}
	else
	{
		std::cout << "   Anisotropic filtering not supported" << std::endl;
	}


	// Set callback functions:
	glutDisplayFunc(displayCallback);
	glutReshapeFunc(reshapeCallback);
	glutKeyboardFunc(keyboardCallback);
	glutSpecialFunc(specialCallback);
	glutTimerFunc(3000, timerCallback, 0);
	glutIdleFunc(idleCallback);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	FreeImage_DeInitialise();
}
