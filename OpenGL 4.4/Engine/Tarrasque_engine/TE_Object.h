#pragma once

//////////////
// #INCLUDE //
//////////////
#include "TE_Dll_interface.h"

#include "TE_Includes.h"

////////////////
// #TYPEDEF   //
////////////////
typedef glm::vec2 TE_VEC2;
typedef glm::vec3 TE_VEC3;
typedef glm::vec4 TE_VEC4;
typedef glm::mat4 TE_MAT4;
typedef glm::mat3 TE_MAT3;

///////////////////////////////////
// #REDEFINE OF GLM FUNCTIONS   //
//////////////////////////////////
#define te_inverse(arg)(glm::inverse(arg))
#define	te_rotate(angle,axis)(glm::rotate(glm::mat4(),glm::radians(angle),axis))
#define te_translate(vector)(glm::translate(glm::mat4(),vector))
#define te_scale(factor)(glm::scale(glm::mat4(),factor))

////////////////
// #ENUM	  //
////////////////
enum class LIB_API Object_type{ OBJECT, NODE, LIGHT, CAMERA, MESH, TEXTURE, MATERIAL };

/**
* Abstract class to represent an object of our Engine, every object gets a
* unique ID and all the subclasses need to implement the render node.
*/
class LIB_API TE_Object
{
public:
	virtual void render() = 0;
	int get_id();
	Object_type get_type();

	TE_Object();
	~TE_Object();
protected:
	static int staticId;
	int m_id;
	Object_type m_type;
};