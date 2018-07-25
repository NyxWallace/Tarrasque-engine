#include "TE_Material.h"


/**
* Default constructor for a material.
*/
LIB_API TE_Material::TE_Material()
{
	m_type = Object_type::MATERIAL;
	m_ambient = TE_VEC4(0.3f,0.3f,0.3f,1.0f);
	m_diffuse = TE_VEC4(0.5f, 0.5f, 0.5f,1.0f);
	m_specular = TE_VEC4(0.6f, 0.6f, 0.6f,1.0f);
	m_shininess = 10;
}

/**
* Constructor for a material.
* @param ambient ambient factor of the material
* @param diffuse diffuse factor of the material
* @param specular specular factor of the material
* @param emissive emissive factor of the material
* @param opacity  opacity of the material
* @param shininess shininess factor of the material
* @param texture a pointer to a texture
*
*/
LIB_API TE_Material::TE_Material(TE_VEC4 ambient, TE_VEC4 diffuse, TE_VEC4 specular, TE_VEC4 emissive, float opacity, float shininess, TE_Texture *texture, TE_Shader* shader){
	m_type = Object_type::MATERIAL;
	m_ambient = ambient;
	m_diffuse = diffuse;
	m_specular = specular;
	m_emissive = emissive;
	m_shininess = shininess;
	m_texture = texture;
	m_opacity = opacity;
	m_shader = shader;
}

/**
* Destructor.
*/
LIB_API TE_Material::~TE_Material(){
}


/**
* Method to set a material from another material
* @param material the material to set
*/
LIB_API void TE_Material::set_material(TE_Material* material){
	m_ambient = material->m_ambient;
	m_diffuse = material->m_diffuse;
	m_specular = material->m_specular;
	m_emissive = material->m_emissive;
	m_shininess = material->m_shininess;
	m_texture = material->m_texture;
	// Opacity has the same value for all alpha channels
	m_opacity = material->m_opacity;
}

/**
* Method to render a material.
* We set the material properties for the mesh being rendered with this material.
* If the material has a texture we render it before setting the material properties.
*/
LIB_API void TE_Material::render(){
	if (this->m_texture != nullptr){
		this->m_texture->render();
	}

	m_shader->setVec3(m_shader->matAmbientLoc, TE_VEC3(m_ambient));
	m_shader->setVec3(m_shader->matDiffuseLoc, TE_VEC3(m_diffuse));
	m_shader->setVec3(m_shader->matSpecularLoc, TE_VEC3(m_specular));
	m_shader->setFloat(m_shader->matShininessLoc, m_shininess);
}

/**
* Method to check is a material is solid or not.
* @return true if material is trasparent
*/
LIB_API bool TE_Material::is_transparent(){
	return m_opacity < 1.0f;
}

/**
* Method to set the opacity level of a material.
* @param opacity a value between 0 and 1 to represent a material's opacity level
*/
LIB_API void TE_Material::set_transparent(float opacity){
	m_opacity = opacity;
	m_ambient[3] = opacity;
	m_diffuse[3] = opacity;
	m_specular[3] = opacity;
}

/**
* Method to set the emissive property of a material.
* @param emission the material emissiv value
*/
LIB_API void TE_Material::set_emission(TE_VEC4 emission){
	m_emissive = emission;
}

LIB_API void TE_Material::set_shader(TE_Shader* shader){
	m_shader = shader;
}

LIB_API void TE_Material::set_ambient(TE_VEC4 ambient){
	m_ambient = ambient;
}

LIB_API void TE_Material::set_diffuse(TE_VEC4 diffuse){
	m_diffuse = diffuse;
}

LIB_API void TE_Material::set_specular(TE_VEC4 specular){
	m_specular = specular;
}

LIB_API void TE_Material::set_texture(TE_Texture *texture){
	m_texture = texture;
}

LIB_API void TE_Material::set_opacity(float opacity){
	m_opacity = opacity;
}

LIB_API void TE_Material::set_shininess(float shininess){
	m_shininess = shininess;
}
