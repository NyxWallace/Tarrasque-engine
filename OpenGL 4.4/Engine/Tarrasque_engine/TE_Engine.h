#pragma once

//////////////
// #INCLUDE //
//////////////

//DLL
#include "TE_Dll_interface.h"

//global includes

#include "TE_Includes.h"

// TE_Engine Elements
#include "TE_Object.h"
#include "TE_Node.h"
#include "TE_Camera.h"
#include "TE_Mesh.h"
#include "TE_Material.h"
#include "TE_Texture.h"
#include "TE_Light.h"
#include "TE_List.h"
#include "TE_Shader.h"
#include "TE_Fbo.h"
#include "TE_SkyBox.h"


/**
* Main engine class we use a signleton to get a single istance an engine.
* This class provides various methods to change the GL context after it
* is setup with the init() method.
*/
class LIB_API TE_Engine
{
public:
	int m_width, m_height;

	static TE_Engine* init();
	static TE_Engine* get_istance();
	static TE_Node* load_scene(std::string);
	static TE_Camera* get_camera();
	static TE_List* get_scene();
	static int get_anis_level();

	static void smooth_shading();
	static void flat_shading();

	static void set_display_callback(void(*)());
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
	static void write_string(const char*, float, float);
	static void depth_function(GLenum);
	static void cw_face_culling();
	static void ccw_face_culling();
	static void disable_lighting();
	static void enable_lighting();
	static void swap();
	static void display();

	TE_MAT4 get_perspective() const { return m_perspective; };
	TE_MAT4 get_fbo_perspective() const { return m_fbo_perspective; };
	TE_MAT4 get_ortho() const { return m_ortho; }
	void set_perspective(TE_MAT4 perspective) { m_perspective = perspective; };
	void set_ortho(TE_MAT4 ortho){ this->m_ortho = ortho; };
	void set_fbo_perspective(TE_MAT4 fbo_perspective){ this->m_fbo_perspective = fbo_perspective; };

	void build_shaders();
	void gen_fbo_ids();
	void gen_global_texture();
	void gen_fbo_texture();
	void gen_fbo_vbo();
	void set_stereo_rendering(bool);
	void toggle_stereo_rendering();
	bool get_stereo_rendering();

	void set_near_plane(float near_plane){ this->m_near_plane = near_plane; };
	void set_far_plane(float far_plane){ this->m_far_plane = far_plane; };
	void set_fov(float fov){ this->m_fov = fov; };
	void set_convergence(float convergence){ this->m_convergence = convergence; };
	void set_eye_separation(float eye_separation){ this->m_eye_separation = eye_separation; };
	void set_aspect_ratio(float aspect_ratio){ this->m_aspect_ratio = aspect_ratio; };

	float get_near_plane(){ return m_near_plane; };
	float get_far_plane(){ return m_far_plane; };
	float get_fov(){ return m_fov; };
	float get_convergence(){ return m_convergence; };
	float get_eye_separation(){ return m_eye_separation; };
	float get_aspect_ratio(){ return m_aspect_ratio; };

	TE_SkyBox* get_skybox(){ return SkyBox; }


	void apply_right_frustum();
	void apply_left_frustum();

	TE_SkyBox* SkyBox;

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
	TE_MAT4 m_perspective;
	TE_MAT4 m_fbo_perspective;
	TE_MAT4 m_ortho;
	static int m_anis_level;
	static unsigned int vao;
	bool m_stereo_rendering = false;
	float m_near_plane, m_far_plane, m_fov, m_convergence, m_eye_separation, m_aspect_ratio;

	//Singleton
	static TE_Engine* m_istance;
	

	TE_Engine();
};

