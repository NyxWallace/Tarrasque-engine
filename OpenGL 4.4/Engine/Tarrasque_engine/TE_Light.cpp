#include "TE_Light.h"

int TE_Light::light_index = 0;

/**
* Default constructor.
*/
LIB_API TE_Light::TE_Light(): TE_Node("default_light") {
	m_type = Object_type::LIGHT;
}


LIB_API TE_Light::TE_Light(std::string name, aiLightSourceType lightType) : TE_Node(name), m_direction(TE_VEC3(1.0f)){
	m_type = Object_type::LIGHT;
	m_light_id = light_index++;
	m_lightType = lightType;
	//glEnable(lights[m_light_id]);
}

/**
* Setter for a light's direction.
* @param direction vector representing the light's direction
*/
LIB_API void TE_Light::set_direction(TE_VEC3 direction){
	m_direction = direction;
}

/**
* Setter for a light's position.
* @param light_position the vector containing the light's position
*/
LIB_API void TE_Light::set_light_pos(TE_VEC3 light_position){
	this->m_light_pos = light_position;
}

/**
* Setter for a light's attenuation.
* @param constant constant value of the attenuation
* @param linear linear value of the attenuation
* @param quadratic quadratic value of the attenuation
*/
LIB_API void TE_Light::set_attenuation(float constant, float linear, float quadratic){
	m_const_atten = constant;
	m_linear_atten = linear;
	m_quad_atten = quadratic;
}

/**
* Getter for a light's direction.
* @return the light's direction
*/
LIB_API TE_VEC3 TE_Light::get_direction(){
	return m_direction;
}

/**
* Method to disable a light source.
*/
LIB_API bool TE_Light::is_enabled(){
	return m_enabled;
}

/**
* Method to enable a light source.
*/
LIB_API void TE_Light::enable(){
	m_enabled = true;
	//glEnable(lights[m_light_id]);
}

/**
* Method to disable a light source.
*/
LIB_API void TE_Light::disable(){
	m_enabled = false;
	glDisable(lights[m_light_id]);
}

LIB_API void TE_Light::set_shader(TE_Shader* shader){
	m_shader = shader;
}

/**
* Function to render a light. 
* The transformation matrix has to be loaded before this instruction is called.
*/
LIB_API void TE_Light::render(){

	//std::cout << "rendering light" << std::endl;
	int light_id = lights[this->m_light_id];
	//std::cout << "GL_LIGHT: " << light_id << std::endl;
	//std::cout << "cutoff: " << m_cutoff << std::endl;

	TE_VEC4 position;

	switch (m_lightType){
		case aiLightSource_SPOT:
			//std::cout << "spotlight" << std::endl;
			//position = glm::vec4(m_position[3][0], m_position[3][1], m_position[3][2], 1.0f);
			m_shader->render();
			position = TE_VEC4(m_light_pos, 1.0f);
			m_shader->setVec3(m_shader->spotDirection, TE_VEC3(m_direction));
			m_shader->setFloat(m_shader->spotCutoff, m_cutoff);
			/*glLightfv(light_id, GL_POSITION, glm::value_ptr(position));
			glLightf(light_id, GL_SPOT_CUTOFF, m_cutoff);
			glLightfv(light_id, GL_SPOT_DIRECTION, glm::value_ptr(m_direction));*/
			break;
		case aiLightSource_DIRECTIONAL:
			//std::cout << "directional" << std::endl;
			position = TE_VEC4(m_light_pos, 0.0f);
			//float cutoff = 180.0f;

			//glLightfv(light_id, GL_POSITION, glm::value_ptr(position));
			////glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, &cutoff);
			break;
		case aiLightSource_POINT:
			//std::cout << "omnidirectional" << std::endl;
			m_shader->render();
			position = TE_VEC4(m_light_pos, 1.0f);
			//position = glm::vec4(m_position[3][0], m_position[3][1], m_position[3][2], 1.0f);
			//m_cutoff = 180.0f;

			/*glLightfv(light_id, GL_POSITION, glm::value_ptr(position));
			glLightf(light_id, GL_SPOT_CUTOFF, m_cutoff);*/
			break;
	}


	TE_MAT4 mat = TE_MAT4(m_parent->get_position()*m_position);
	//printf("%f - %f - %f\n", mat[3][0], mat[3][1], mat[3][2]);

	m_shader->setVec3(m_shader->lightPositionLoc, TE_VEC3(mat[3][0],mat[3][1],mat[3][2]));
	m_shader->setVec3(m_shader->lightAmbientLoc, TE_VEC3(m_ambient));
	m_shader->setVec3(m_shader->lightDiffuseLoc, TE_VEC3(m_diffuse));
	m_shader->setVec3(m_shader->lightSpecularLoc, TE_VEC3(m_specular));
}

LIB_API void TE_Light::set_ambient(TE_VEC4 ambient){
	m_ambient = ambient;
}

LIB_API void TE_Light::set_diffuse(TE_VEC4 diffuse){
	m_diffuse = diffuse;
}

LIB_API void TE_Light::set_specular(TE_VEC4 specular){
	m_specular = specular;
}

LIB_API void TE_Light::set_cutoff(float cutoff){
	m_cutoff = cutoff;
}
/**
* Destructor.
*/
LIB_API TE_Light::~TE_Light(){
}
