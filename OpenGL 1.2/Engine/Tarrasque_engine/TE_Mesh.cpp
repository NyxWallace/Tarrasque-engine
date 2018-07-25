#include "TE_Mesh.h"


/**
* Default mesh constructor.
*/
LIB_API TE_Mesh::TE_Mesh() : TE_Node("default_mesh"),m_material(new TE_Material()), m_vertex(new vector<TE_VEC3>), m_normals(new vector<TE_VEC3>), m_texture_coords(new vector<TE_VEC2>){
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
LIB_API TE_Mesh::TE_Mesh(string name,vector<TE_VEC3> *vertex, vector<TE_VEC3> *normals, vector<TE_VEC2> *texture_coords, TE_Material *material) : TE_Node(name) {
	m_type = Object_type::MESH;
	this->m_vertex = vertex;
	this->m_material = material;
	this->m_normals = normals;
	this->m_texture_coords = texture_coords;
}

/**
* Destructor.
*/
LIB_API TE_Mesh::~TE_Mesh(){
	delete m_material;
	delete m_vertex;
	delete m_normals;
	delete m_texture_coords;
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
	//std::cout << "rendering mesh" << std::endl;
	this->m_material->render();

	if (m_material->is_transparent()){
		glCullFace(GL_FRONT);
		//printf("Rendering transparent!!");
		for (unsigned int i = 0; i < m_vertex->size(); i = i + 3){
			glBegin(GL_TRIANGLES);
			glNormal3f(m_normals->at(i).x, m_normals->at(i).y, m_normals->at(i).z);
			glTexCoord2f(m_texture_coords->at(i).x, m_texture_coords->at(i).y);
			glVertex3f(m_vertex->at(i).x, m_vertex->at(i).y, m_vertex->at(i).z);

			glNormal3f(m_normals->at(i + 1).x, m_normals->at(i + 1).y, m_normals->at(i + 1).z);
			glTexCoord2f(m_texture_coords->at(i + 1).x, m_texture_coords->at(i + 1).y);
			glVertex3f(m_vertex->at(i + 1).x, m_vertex->at(i + 1).y, m_vertex->at(i + 1).z);

			glNormal3f(m_normals->at(i + 2).x, m_normals->at(i + 2).y, m_normals->at(i + 2).z);
			glTexCoord2f(m_texture_coords->at(i + 2).x, m_texture_coords->at(i + 2).y);
			glVertex3f(m_vertex->at(i + 2).x, m_vertex->at(i + 2).y, m_vertex->at(i + 2).z);
			glEnd();
		}

		glCullFace(GL_BACK);
	}

	for (unsigned int i = 0; i < m_vertex->size(); i=i+3){
		glBegin(GL_TRIANGLES);
		//glColor3f(0.25 + ((float)rand()) / (float)(RAND_MAX), 0.25 + ((float)rand()) / (float)(RAND_MAX), 0.25 + ((float)rand()) / (float)(RAND_MAX));
		//glNormal3f(0.0f, 0.0f, 1.0f);
		glNormal3f(m_normals->at(i).x, m_normals->at(i).y, m_normals->at(i).z);
		glTexCoord2f(m_texture_coords->at(i).x, m_texture_coords->at(i).y);
		glVertex3f(m_vertex->at(i).x, m_vertex->at(i).y, m_vertex->at(i).z);

		glNormal3f(m_normals->at(i + 1).x, m_normals->at(i + 1).y, m_normals->at(i + 1).z);
		glTexCoord2f(m_texture_coords->at(i + 1).x, m_texture_coords->at(i + 1).y);
		glVertex3f(m_vertex->at(i + 1).x, m_vertex->at(i + 1).y, m_vertex->at(i + 1).z);

		glNormal3f(m_normals->at(i + 2).x, m_normals->at(i + 2).y, m_normals->at(i + 2).z);
		glTexCoord2f(m_texture_coords->at(i + 2).x, m_texture_coords->at(i + 2).y);
		glVertex3f(m_vertex->at(i + 2).x, m_vertex->at(i + 2).y, m_vertex->at(i + 2).z);
		glEnd();
	}
	//Disable textures so the next object doesn't keep the one used by this mesh
	glDisable(GL_TEXTURE_2D);
}
