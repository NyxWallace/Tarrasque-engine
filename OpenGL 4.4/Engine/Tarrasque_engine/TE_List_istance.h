#pragma once

#include "TE_Dll_interface.h"
#include "TE_Node.h"

/**
* This class represents a pair of Node,Matrix that we store in TE_List.
* We store a pointer to a node and a matrix representing the object's
* world coordinates.
*/
class LIB_API TE_List_istance{
public:
	TE_Node* m_node;
	TE_MAT4 m_world_matrix;

	TE_List_istance(TE_Node*, TE_MAT4);
	TE_List_istance(){};
	~TE_List_istance(){};
};

