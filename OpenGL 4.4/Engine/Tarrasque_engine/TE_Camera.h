#pragma once

#include "TE_Dll_interface.h"
#include "TE_Node.h"

/**
* Class to represent a camera node.
*/
class LIB_API TE_Camera : public TE_Node
{
public:
	void render(){};
	TE_MAT4 inverse();
	TE_Camera(std::string);
	~TE_Camera();

private:
	TE_MAT4 m_camera_pos;
};