#include "TE_List_istance.h"

/**
* Constructor for a list istance object.
*/
TE_List_istance::TE_List_istance(TE_Node* node, TE_MAT4 world_matrix){
	m_node = node;
	m_world_matrix = world_matrix;
}
