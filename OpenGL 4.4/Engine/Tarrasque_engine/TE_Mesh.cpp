#include "TE_Mesh.h"


/**
* Default mesh constructor.
*/
LIB_API TE_Mesh::TE_Mesh() : TE_Node("default_mesh"),m_material(new TE_Material()), m_vertex(0), m_normals(0), m_texture_coords(0){
	m_type = Object_type::MESH;
}

/**
* Mesh constructor.
* @param name the mesh name
* @param vertex the mesh vertexes in object coordinates
* @param normals the mesh normal vectors
* @param texture_coords the texture coordinates to apply a texture
* @param material the mesh's material
*/
LIB_API TE_Mesh::TE_Mesh(std::string name, int vertex_size, unsigned int vertex, unsigned int normals, unsigned int texture_coords, unsigned int colors,TE_Material *material) : TE_Node(name) {
	m_type = Object_type::MESH;
	this->m_vertex = vertex;
	this->m_material = material;
	this->m_normals = normals;
	this->m_texture_coords = texture_coords;
	this->m_vertex_size = vertex_size;
	this->m_colors = colors;
}

/**
* Destructor.
*/
LIB_API TE_Mesh::~TE_Mesh(){
	delete m_material;
}

/**
* Getter for a node's material.
* @return the mesh's material
*/
LIB_API TE_Material* TE_Mesh::get_material(){
	return m_material;
}

/**
* Method to render a mesh. We build a triangle every 3 vertexes and set their texture coordinates.
* Before drawing the triangles we render the mesh material and texture.
* We also check if the mesh material is trasparent, in that case we first render its back side
* by calling glCullFace(GL_FRONT) before drawing triangles and after that we use glCullFace(GL_BACK)
* to reset it to normal and render its front faces, if the material is solid we just render the front
* faces.
*/
LIB_API void TE_Mesh::render(){
	this->m_material->render();

	if (m_material->is_transparent()){
		glCullFace(GL_FRONT);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertex);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, m_normals);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, m_texture_coords);
		glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(2);

		glDrawArrays(GL_TRIANGLES, 0, m_vertex_size);

		glCullFace(GL_BACK);
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vertex);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_normals);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, m_texture_coords);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(2);

	glDrawArrays(GL_TRIANGLES, 0, m_vertex_size);
}
