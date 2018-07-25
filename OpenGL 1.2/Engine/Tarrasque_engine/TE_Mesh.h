#pragma once

#include "TE_Material.h"
#include "TE_Node.h"


/**
* Class that represents a Mesh.
* The mesh is composed of a material, its vertexes (in object coordinates), normal vectors
* and texture coordinates (the texture is  inside the material object).
*/
class LIB_API TE_Mesh : public TE_Node
{
public:
	void load(std::string,int);
	void render();

	TE_Material* get_material();

	TE_Mesh();
	TE_Mesh(string,vector<TE_VEC3>*, vector<TE_VEC3>*, vector<TE_VEC2>*, TE_Material*);
	~TE_Mesh();
private:
	TE_Material* m_material;
	vector<TE_VEC3>* m_vertex;
	vector<TE_VEC3>* m_normals;
	vector<TE_VEC2>* m_texture_coords;
};

