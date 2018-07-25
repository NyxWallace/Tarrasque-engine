#include "TE_SkyBox.h"

std::string cubemapNames[] =
{
	"posx.jpg",
	"negx.jpg",
	"negy.jpg",
	"posy.jpg",
	"posz.jpg",
	"negz.jpg",
};

float cubeVertices[] = {
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
};

unsigned short cubeFaces[] =
{
	0, 1, 2,
	0, 2, 3,
	3, 2, 6,
	3, 6, 7,
	4, 0, 3,
	4, 3, 7,
	6, 5, 4,
	7, 6, 4,
	4, 5, 1,
	4, 1, 0,
	1, 5, 6,
	1, 6, 2,
};
/////////////
// SHADERS //
/////////////

//////////////////////////////////////////
char *vertSkyShader = R"(
   #version 440 core

   uniform mat4 projection;
   uniform mat4 modelview;

   layout(location = 0) in vec3 in_Position;      

   out vec3 texCoord;

   void main(void)
   {
      texCoord = in_Position;
      gl_Position = projection * modelview * vec4(in_Position, 1.0);            
   }
)";

//////////////////////////////////////////
char *fragSkyShader = R"(
   #version 440 core
   
   in vec3 texCoord;
   
   uniform samplerCube cubemapSampler;

   out vec4 fragOutput;

   void main(void)
   {      
      vec3 _texCoord = vec3(texCoord.x, -texCoord.y, texCoord.z); // <-- lazy way to flip JPG images...
      fragOutput = texture(cubemapSampler, _texCoord);
   }
)";

/**
* Constructor for the skybox, it also builds the shader used to render it.
*/
TE_SkyBox::TE_SkyBox()
{
	// Compile shaders:
	TE_Shader vs;
	vs.loadFromMemory(TE_Shader::TYPE_VERTEX, vertSkyShader);

	TE_Shader ps;
	ps.loadFromMemory(TE_Shader::TYPE_FRAGMENT, fragSkyShader);

	shader.build(&vs, &ps);
	shader.bind(0, "in_Position");

	projLoc = shader.getParamLocation("projection");
	mvLoc = shader.getParamLocation("modelview");
	cubemapLoc = shader.getParamLocation("cubemapSampler");
	// Load cube data into a VBO:
	glGenBuffers(1, &cubeVboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVboVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &cubeVboFaces);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVboFaces);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeFaces), cubeFaces, GL_STATIC_DRAW);
	build_cube_map();
	shader.setInt(cubemapLoc, 0);
}

/**
* Method to build the cube map
*/
void TE_SkyBox::build_cube_map()
{
	// Create and bind cubemap:   
	glGenTextures(1, &cubemapId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);

	// Set params:
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Set filters:
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP , GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

	load_textures();
}

/**
* Method to load the skybox's textures
*/
void TE_SkyBox::load_textures()
{
	// Load sides:
	for (int curSide = 0; curSide < 6; curSide++)
	{
		// Load texture:
		FIBITMAP *fBitmap = FreeImage_Load(FreeImage_GetFileType(cubemapNames[curSide].c_str(), 0), cubemapNames[curSide].c_str());
		if (fBitmap == nullptr)
			std::cout << "ERROR loading the image: " << cubemapNames[curSide].c_str() << std::endl;
		int intFormat = GL_RGB;
		GLenum extFormat = GL_BGR;
		if (FreeImage_GetBPP(fBitmap) == 32)
		{
			intFormat = GL_RGBA;
			extFormat = GL_BGRA;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + curSide, 0, intFormat, FreeImage_GetWidth(fBitmap), FreeImage_GetHeight(fBitmap), 0, extFormat, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(fBitmap));

		// Free resources:
		FreeImage_Unload(fBitmap);
	}
}

/**
* Render method.
*/
void LIB_API TE_SkyBox::render(){
	shader.render();
	glDisable(GL_CULL_FACE);
	glm::mat4 f = glm::mat4();
	f = glm::scale(f, glm::vec3(m_far_plane - m_far_plane/2.0f));
	// Get the rotation from the position of the camera
	f *= TE_MAT4(glm::inverse(TE_MAT3(m_rotation)));

	shader.setMatrix(mvLoc, f);
	shader.setMatrix(projLoc,m_proj);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVboVertices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVboFaces);
	glDrawElements(GL_TRIANGLES, sizeof(cubeFaces) / sizeof(unsigned short), GL_UNSIGNED_SHORT, nullptr);
	glEnable(GL_CULL_FACE);
}