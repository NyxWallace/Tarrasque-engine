#pragma once

#include "TE_Dll_interface.h"

#include "TE_Material.h"
#include "TE_Node.h"
#include "TE_Includes.h"

/**
* Class that represents a Mesh.
* The mesh is composed of a material, its vertexes (in object coordinates), normal vectors
* and texture coordinates (the texture is  inside the material object).
*/
class LIB_API TE_Mesh : public TE_Node
{
public:
	void load(std::string, int);
	void render();

	TE_Material* get_material();

	TE_Mesh();
	TE_Mesh(std::string, int, unsigned int, unsigned int, unsigned int,unsigned int, TE_Material*);
	~TE_Mesh();
private:
	TE_Material* m_material;
	unsigned int m_vertex;
	unsigned int m_normals;
	unsigned int m_colors;
	unsigned int m_texture_coords;
	int m_vertex_size;
};