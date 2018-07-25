#pragma once

#include "TE_Node.h"

static int lights[] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT7 };

/**
* Class that represents a Light node in the scene graph.
* All settings can be set by the client.
*/
class LIB_API TE_Light : public TE_Node
{
public:
	TE_Light();
	TE_Light(string,aiLightSourceType, TE_VEC4, TE_VEC4, TE_VEC4,float);
	~TE_Light();

	bool is_enabled();
	void render();
	void enable();
	void disable();
	void set_direction(TE_VEC3);
	void set_light_pos(TE_VEC3);
	void set_attenuation(float, float, float);
	TE_VEC3 get_direction();
private:
	bool m_enabled = true;
	static int light_index;
	int m_light_id;
	float m_cutoff;
	float m_const_atten;
	float m_linear_atten;
	float m_quad_atten;
	TE_VEC3 m_light_pos;
	TE_VEC4 m_ambient;
	TE_VEC4 m_diffuse;
	TE_VEC4 m_specular;
	aiLightSourceType m_lightType;
	TE_VEC3 m_direction;
};

