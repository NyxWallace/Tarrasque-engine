#pragma once

#include "TE_Node.h"
#include "TE_Mesh.h"
#include "TE_List_istance.h"
#include <map>
#include <vector>


/**
* Class used to store our list of objects to render in world coordinates.
* This class provides a render method that renders all objects inside
* the vector of TE_List_istance.
*/
class LIB_API TE_List
{
public:
	void render(TE_MAT4);
	void pass(TE_Node*);
	vector<TE_List_istance>* get_istances();
	vector<TE_List_istance>*  get_depth_sorted_transparent_objects();

	TE_List();
	~TE_List();
private:
	vector<TE_List_istance>* m_istances;
	vector<TE_MAT4>* m_mat_stack;
};

