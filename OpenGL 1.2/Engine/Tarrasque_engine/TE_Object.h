#pragma once

#ifdef WIN32 	
// Export API:
// Specifies i/o linkage (VC++ spec):
#ifdef TARRASQUE_ENGINE_EXPORTS
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif      	
#else // Under Linux
#define LIB_API  // Dummy declaration
#endif

//////////////
// #INCLUDE //
//////////////

// GLM:
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

// Assimp:
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/version.h>

// FreeGLUT:   
#include <GL/freeglut.h>

// FreeImage
#include <FreeImage.h>

// C/C++:
#include <iostream>     
#include <time.h>
#include <string>

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF


////////////////
// #TYPEDEF   //
////////////////
typedef glm::vec2 TE_VEC2;
typedef glm::vec3 TE_VEC3;
typedef glm::vec4 TE_VEC4;
typedef glm::mat4 TE_MAT4;

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
enum class LIB_API Object_type{OBJECT,NODE,LIGHT,CAMERA,MESH,TEXTURE,MATERIAL};

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

