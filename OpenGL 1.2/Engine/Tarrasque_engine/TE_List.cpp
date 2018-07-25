#include "TE_List.h"

/**
* Method to pass a graph to the list. It recursively parses
* all the nodes children and calculates its world coordinates
* depending on the hierarcy of the scene graph, then adds it
* to the vector of list istances.
* @param root the root of the graph to parse
*/
LIB_API void TE_List::pass(TE_Node* root){
	vector<TE_Node*>* children = root->get_children();
	TE_MAT4 tmp = root->get_position();
	if (root->get_parent() != nullptr && !m_mat_stack->empty()){
		tmp = m_mat_stack->back()*tmp;
	}
	m_mat_stack->push_back(tmp);

	for (int i = 0; i < children->size(); i++){
		pass((*children)[i]);
	}

	m_mat_stack->pop_back();


	//std::cout << glm::to_string(tmp) << std::endl;
	//std::map<TE_Node*, TE_MAT4>::iterator iter = m_istances->find(root);
	m_istances->push_back(TE_List_istance(root, tmp));
}

/**
* Method to get the vector of list istances.
* @return the vector of istances
*/
LIB_API vector<TE_List_istance>* TE_List::get_istances(){
	return m_istances;
}

/**
* Method to compare list istances used for sorting transparent object.
*/
bool istance_comp(const TE_List_istance& elem1, const TE_List_istance& elem2){
	return elem1.m_world_matrix[3][2] > elem2.m_world_matrix[3][2];
}

/**
* Method to get a depth sorted vector of list istances from furthest to nearest.
* We use this method to determine the render order of
* the transparent objects in the scene.
* @return the depth sorted vector of transparent objects
*/
LIB_API vector<TE_List_istance>* TE_List::get_depth_sorted_transparent_objects(){
		//map<float, TE_Node*>* transparentObjs = new map<float, TE_Node*>();
		//vector<TE_List_istance>* result = new vector<TE_List_istance>();
		vector<TE_List_istance>* transparentObjs = new vector<TE_List_istance>();

		for (auto& item : (*m_istances))
		{
			if (item.m_node->get_type() == Object_type::MESH)
				if (((TE_Mesh*)item.m_node)->get_material()->is_transparent()){
					//std::cout << item.m_node->get_name() << std::endl;
					//transparentObjs->insert(pair<float, TE_Node*>(item.m_world_matrix[3][2], item.m_node));
					transparentObjs->push_back(TE_List_istance(item.m_node,item.m_world_matrix));
				}
		}

		sort(transparentObjs->begin(),transparentObjs->end(), istance_comp);
		return transparentObjs;
}

/**
* Method to render all the objects in the list.
* @param inverse the inverse of the camera we want to use.
*/
LIB_API void TE_List::render(TE_MAT4 inverse){
	// Iterate over the array of istances to find light sources and render them
	for (auto& item : (*m_istances))
	{
		if (item.m_node->get_type() == Object_type::LIGHT){
			//glLoadMatrixf(glm::value_ptr(inverse*get_istances->at(item.first->get_parent())));
			glLoadMatrixf(glm::value_ptr(inverse*item.m_world_matrix));
			item.m_node->render();
		}
	}

	// Iterate again to render solid meshes
	for (auto& item : (*m_istances))
	{
		if (item.m_node->get_type() == Object_type::MESH){
			if (!((TE_Mesh*)item.m_node)->get_material()->is_transparent()){
				//std::cout << "world coordinates " << glm::to_string(inverse*item.second) << std::endl;
				glLoadMatrixf(glm::value_ptr(inverse*item.m_world_matrix));
				item.m_node->render();
			}
        }
	}

	// Render transparent meshes
	glDepthMask(GL_FALSE);
	vector<TE_List_istance>* transparentObjs = this->get_depth_sorted_transparent_objects();
	for (auto& item : (*transparentObjs)){
		glLoadMatrixf(glm::value_ptr(inverse*item.m_world_matrix));
		item.m_node->render();
	}
	glDepthMask(GL_TRUE);

}

/**
* Constructor of the list.
*/
LIB_API TE_List::TE_List() : m_istances(new vector<TE_List_istance>()), m_mat_stack(new vector<TE_MAT4>()) {
}

/**
* Destructor.
*/
LIB_API TE_List::~TE_List(){
	delete m_istances;
	delete m_mat_stack;
}
