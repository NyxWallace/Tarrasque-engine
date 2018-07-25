#include "TE_Camera.h"

/**
* Method to get the inverse of the camera
*/
LIB_API TE_MAT4 TE_Camera::inverse(){
	return glm::inverse(m_camera_pos);
}

/**
* Constructor for the camera node.
* @param name the name of the camera node
*/
LIB_API TE_Camera::TE_Camera(string name) : TE_Node(name), m_camera_pos(TE_MAT4()){
	m_type = Object_type::CAMERA;
}

/**
* Destructor.
*/
LIB_API TE_Camera::~TE_Camera(){
}
