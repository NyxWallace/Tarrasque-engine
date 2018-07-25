#include "TE_Node.h"

/**
* Method to link a node to the caller of the method
* @param node the node to link to the caller of the method
*/
void LIB_API TE_Node::link(TE_Node* node)
{
	//node->m_children.push_back(this);
	if (node == nullptr)
		return;

	// Check if node already has a parent
	// Unlink the node if it is the case
	if (node->get_parent() != nullptr){
		node->get_parent()->unlink(node);
	}

	m_children->push_back(node);
	node->set_parent(this);
}

/**
* Method to unlink a node from the caller of the method.
* If the node is not a children of the caller no changes are made.
* @param node the children to unlink
*/
void LIB_API TE_Node::unlink(TE_Node* node)
{
	for (std::vector<TE_Node*>::iterator iter = m_children->begin(); iter != m_children->end(); ++iter) {
		if (*iter == node){
			node->set_parent(nullptr);
			m_children->erase(iter);
			break;
		}
	}
}

/**
* Method to recursively find a node inside the graph that starts from the caller of the method.
* @param name the name of the node to find
* @return the found node or nullptr
*/
LIB_API TE_Node* TE_Node::find_node(std::string name){
	for (std::vector<TE_Node*>::iterator iter = m_children->begin(); iter != m_children->end(); ++iter) {
		TE_Node* tmp = nullptr;
		if ((*iter)->get_name() == name){
			return (*iter);
		}
		else{
			tmp = (*iter)->find_node(name);
		}
		if (tmp != nullptr)
			return tmp;
	}
	return nullptr;
}

/**
* Getter for a node's children.
* @return the vector containing the children
*/
LIB_API std::vector<TE_Node*>* TE_Node::get_children(){
	return m_children;
}

/**
* Method to set a node's name.
*/
void LIB_API TE_Node::set_name(std::string name){
	m_name = name;
}

/**
* Getter for a node's name.
* @return the node's name
*/
LIB_API std::string TE_Node::get_name(){
	return m_name;
}

/**
* Method that return if a node has children.
* @return true if node has children
*/
bool LIB_API TE_Node::has_children(){
	return m_children->size() > 0;
}

/**
* Method to get a node's parent.
* @return the node's parent
*/
LIB_API TE_Node* TE_Node::get_parent(){
	return m_parent;
}

/**
* Method to set a node's parent.
*/
LIB_API void TE_Node::set_parent(TE_Node* parent){
	m_parent = parent;
}

/**
* Method to get a node's transform matrix.
* @return the node's trasform matrix
*/
LIB_API TE_MAT4 TE_Node::get_position(){
	return m_position;
}

/**
* Method to reset a node's matrix to an identity matrix.
*/
void LIB_API TE_Node::init_mat(){
	m_position = TE_MAT4();
}

/**
* Method to translate a node.
* @param traslation a vector containing the desired translation.
*/
void LIB_API TE_Node::translate(TE_VEC3 traslation){
	m_position *= glm::translate(TE_MAT4(),traslation);
}

/**
* Method to rotate a note around itself, to rotate around another
* pivot use trasform.
* @param angle the angle of the rotation expressed in degrees
* @param axis the axis of the rotation
*/
void LIB_API TE_Node::rotate(float angle, TE_VEC3 axis){
	m_position *= glm::rotate(TE_MAT4(), glm::radians(angle), axis);
}

/**
* Method to scale a node.
* @param axis vector containing the axis of the scaling and the factor of scaling
*/
void LIB_API TE_Node::scale(TE_VEC3 axis){
	m_position = m_position*glm::scale(TE_MAT4(), axis);
}

/**
* Method to transform a node. The matrix passed to this method will be the last
* transformation applied to the node
* @param trasform the matrix representing the transformation
*/
void LIB_API TE_Node::trasform(TE_MAT4 trasform){
	m_position = trasform*m_position;
}

/**
* Method to set a node's trasform matrix.
* @param position the matrix to set as current node trasform matrix.
*/
void LIB_API TE_Node::set_position(TE_MAT4 position){
	m_position = position;
}

/**
* Constructor for a node.
* @param name the name of the node.
*/
LIB_API TE_Node::TE_Node(std::string name) : TE_Object(), m_name(name), m_children(new std::vector<TE_Node*>()), m_parent(nullptr), m_position(TE_MAT4()){
	m_type = Object_type::NODE;
}

/**
* Destructor.
*/
LIB_API TE_Node::~TE_Node(){
	delete m_children;
}
