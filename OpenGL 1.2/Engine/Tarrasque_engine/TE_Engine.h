#pragma once

/////////////
// VERSION //
/////////////

// Generic info:
#define LIB_NAME      "Tarrasque TE_Engine Library v0.1a"  ///< Library credits
#define LIB_VERSION   10                              ///< Library version (divide by 10)

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

// TE_Engine Elements
#include "TE_Object.h"
#include "TE_Node.h"
#include "TE_Camera.h"
#include "TE_Mesh.h"
#include "TE_Material.h"
#include "TE_Texture.h"
#include "TE_Light.h"
#include "TE_List.h"

// Assimp:
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/version.h>

// C/C++:
#include <iostream>     
#include <time.h>
#include <stddef.h>

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

/**
* Main engine class we use a signleton to get a single istance an engine.
* This class provides various methods to change the GL context after it 
* is setup with the init() method.
*/
class LIB_API TE_Engine
{
public:
	static TE_Engine* init();
	static TE_Engine* get_istance();
	static TE_Node* load_scene(string);
	static TE_Camera* get_camera();
	static TE_List* get_scene();
	static int get_anis_level();

	static void smooth_shading();
	static void flat_shading();

	static void set_display_callback(void (*)());
	static void set_keyboard_callback(void(*)(unsigned char, int, int));
	static void set_special_callback(void(*)(int, int, int));
	static void set_idle_callback(void(*)());
	static void free_engine();

	static void render(TE_Camera*, TE_List*);
	static void start();

	static void tri_start();
	static void tri_end();
	static void bi_start();
	static void bi_end();
	static void write_string(const char*,float,float);
	static void depth_function(GLenum);
	static void cw_face_culling();
	static void ccw_face_culling();
	static void disable_lighting();
	static void enable_lighting();
	static void swap();
	static void display();

	int m_windowId;
	int m_frames;
	float m_fps;
	void(*m_displayfPtr)();
	void(*m_keyboardfPtr)(unsigned char, int, int);
	void(*m_specialfPtr)(int, int, int);
	void(*m_idlefPtr)();

	~TE_Engine();
private:
	static TE_Node* load_node(aiNode*, aiScene*);
	static TE_Camera* m_camera;
	static TE_List* m_scene;
	static int m_anis_level;

	//Singleton
	static TE_Engine* m_istance;


	TE_Engine();
};

