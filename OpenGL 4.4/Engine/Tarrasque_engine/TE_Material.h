#pragma once

#include "TE_Dll_interface.h"

#include "TE_Object.h"
#include "TE_Texture.h"
#include "TE_Shader.h"

/**
* Class that represents a material.
* Other than all its properties, a material can contain a texture
* stored in a TE_Texture object.
*/
class LIB_API TE_Material : public TE_Object
{
public:
	TE_Material();
	TE_Material(TE_VEC4, TE_VEC4, TE_VEC4, TE_VEC4, float, float, TE_Texture*, TE_Shader*);
	~TE_Material();

	void render();
	void set_material(TE_Material*);

	bool is_transparent();
	void set_transparent(float);							  
	void set_emission(TE_VEC4);
	void set_ambient(TE_VEC4);
	void set_diffuse(TE_VEC4);
	void set_specular(TE_VEC4);
	void set_opacity(float);
	void set_texture(TE_Texture*);
	void set_shininess(float);

	void set_shader(TE_Shader*);
private:
	TE_VEC4 m_ambient;
	TE_VEC4 m_diffuse;
	TE_VEC4 m_specular;
	TE_VEC4 m_emissive;
	float m_shininess;
	float m_opacity;

	TE_Texture* m_texture;
	TE_Shader* m_shader;
};