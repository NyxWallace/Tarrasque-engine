#pragma once

#include <vector>
#include "TE_Object.h"

using namespace std;

/**
* This class represents a generic node in the scene graph.
* Objects of this class contain a matrix representing its world transformation,
* a vector containing its children, its parent node and a name so that we 
* can search the scene graph by looking for a node's name.
* It provides a set of instructions to modify the nodes properties.
*/
class LIB_API TE_Node : public TE_Object
{
public:
	void render(){};
	void link(TE_Node*);
	void unlink(TE_Node*);
	void init_mat();
	TE_Node* find_node(string);
	vector<TE_Node*>* get_children();
	

	void translate(TE_VEC3);
	void scale(TE_VEC3);
	void rotate(float, TE_VEC3);
	void trasform(TE_MAT4);

	TE_Node* get_parent();
	string get_name();
	void set_name(string);
	void set_parent(TE_Node*);
	bool has_children();

	TE_MAT4 get_position();
	void set_position(TE_MAT4);

	TE_Node(string);
	~TE_Node();

protected:
	string m_name;
	TE_Node* m_parent;
	vector<TE_Node*>* m_children;
	TE_MAT4 m_position;
};

