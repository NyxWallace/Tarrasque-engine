#pragma once

#include "TE_Dll_interface.h"
#include "TE_Node.h"
#include "TE_Mesh.h"
#include "TE_List_istance.h"
#include "TE_Shader.h"

/**
* Class used to store our list of objects to render in world coordinates.
* This class provides a render method that renders all objects inside
* the vector of TE_List_istance.
*/
class LIB_API TE_List
{
public:
	void render(TE_MAT4, TE_Shader* shader);
	void pass(TE_Node*);
	std::vector<TE_List_istance>* get_istances();
	std::vector<TE_List_istance>*  get_depth_sorted_transparent_objects();

	TE_List();
	~TE_List();
private:
	
	std::vector<TE_List_istance>* m_istances;
	std::vector<TE_MAT4>* m_mat_stack;
	static int pass_number;
};