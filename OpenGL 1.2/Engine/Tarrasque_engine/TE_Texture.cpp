#include "TE_Texture.h"

/**
* Contructor for a texture
* @param textureName string containing the textureName
* @param anisotropy the anisotropy level available, default 1 if not available
*/
LIB_API TE_Texture::TE_Texture(std::string textureName, int anisotropy) : TE_Object(){
	m_type = Object_type::TEXTURE;
	m_anis_level = anisotropy;

	//We get just the name of the texture from the path provided in the DAE file
	std::size_t char_pos = textureName.find_last_of("/\\");
	textureName = textureName.substr(char_pos + 1);
	//std::cout << "TEXTURE IMAGE" << textureName << std::endl;
	
	glGenTextures(1, &(this->m_tex_id));

	this->bitmap = FreeImage_Load(FreeImage_GetFileType(textureName.c_str(), 0),
		textureName.c_str());

	this->bitmap = FreeImage_ConvertTo32Bits(bitmap);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, this->m_tex_id);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap), GL_BGRA_EXT, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(bitmap));
	glDisable(GL_TEXTURE_2D);
}

/**
* Texture destructor.
*/
LIB_API TE_Texture::~TE_Texture(){
	FreeImage_Unload(this->bitmap);
	glDeleteTextures(1, &(this->m_tex_id));
}

/**
* Render method of a texture.
* First we enable the texturing and then we bind the texture we loaded in the contructor,
* then we set all the texture properties.
*/
LIB_API void TE_Texture::render()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, this->m_tex_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	if (m_anis_level){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_anis_level);
	}
}
