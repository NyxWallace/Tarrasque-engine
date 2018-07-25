#include "TE_Object.h"

/**
* Static int for every object id.
*/
int TE_Object::staticId = 0;

/**
* Object constructor.
*/
LIB_API TE_Object::TE_Object() : m_id(staticId++), m_type(Object_type::OBJECT){
	//std::cout << "ID: " << m_id << std::endl;
}

/**
* Getter for an object's id.
* @return the object's id
*/
int LIB_API TE_Object::get_id(){
	return m_id;
}

/**
* Getter for an object's type.
* @return the object type
*/
Object_type LIB_API TE_Object::get_type(){
	return m_type;
}

/**
* Destructor
*/
LIB_API TE_Object::~TE_Object(){
}

/**
* Render method.
*/
void LIB_API TE_Object::render(){
}
