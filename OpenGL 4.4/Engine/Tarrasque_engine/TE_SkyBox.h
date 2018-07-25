#pragma once
#include "TE_Dll_interface.h"

#include "TE_Includes.h"
#include "TE_Object.h"
#include "TE_Shader.h"

class LIB_API TE_SkyBox :public TE_Object
{
public:
	TE_SkyBox();
	~TE_SkyBox(){};
	void build_cube_map();
	void load_textures();
	void render() override;
	void set_proj(TE_MAT4 proj){ m_proj = proj; };
	void set_mv(TE_MAT4 mv){ m_mv = mv; };
	void set_far_plane(float far_plane){ m_far_plane = far_plane; };
	void set_rotation(TE_MAT4 rotation){ m_rotation = rotation; };
private:
	unsigned int m_tex_id;
	// Textures:
	unsigned int cubemapId;

	// Cube VBO:
	unsigned int cubeVboVertices;

	unsigned int cubeVboFaces;

	// Shaders:
	TE_Shader shader;
	float m_far_plane = 8.0f;
	TE_MAT4 m_rotation = TE_MAT4();
	int projLoc;
	int mvLoc;
	int cubemapLoc;
	TE_MAT4 m_mv, m_proj;
};

