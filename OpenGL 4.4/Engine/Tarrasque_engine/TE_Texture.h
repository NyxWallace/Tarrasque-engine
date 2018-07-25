#pragma once
#include "TE_Dll_interface.h"

#include "TE_Includes.h"
#include "TE_Object.h"

/**
* Class that represents a texture.
* Using freeimage we load the bitmap from the path provided in the constructor.
* We store an id for the texture so we can swap to it when rendering
* again the texture.
*/
class LIB_API TE_Texture :public TE_Object
{
public:
	void render();

	TE_Texture(){};
	TE_Texture(unsigned int);
	void create(int, int);
	TE_Texture(std::string,int);
	~TE_Texture();
private:
	unsigned int m_tex_id;
	int m_anis_level;
	FIBITMAP *bitmap;
};

