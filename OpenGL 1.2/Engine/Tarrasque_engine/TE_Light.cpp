#include "TE_Light.h"

int TE_Light::light_index = 0;

/**
* Default constructor.
*/
LIB_API TE_Light::TE_Light(): TE_Node("default_light") {
	m_type = Object_type::LIGHT;
}

/**
* TE_Light constructor.
*
* @param name the node name
* @param lightType the type of the light (spotlight, omni or directional)
* @param ambient the ambient factor of the light
* @param diffuse the diffuse actor of the light
* @param specular the specular factor of the light
* @param cutoff the angle of the light (180 for omnidirectional and between 0 and 90 for spotlight)
*
*/
LIB_API TE_Light::TE_Light(string name,aiLightSourceType lightType, TE_VEC4 ambient, TE_VEC4 diffuse, TE_VEC4 specular, float cutoff) : TE_Node(name), m_direction(TE_VEC3(1.0f)){
	m_type = Object_type::LIGHT;
	m_light_id = light_index++;
	m_cutoff = cutoff;
	m_ambient = ambient;
	m_diffuse = diffuse;
	m_specular = specular;
	m_lightType = lightType;
	glEnable(lights[m_light_id]);
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
	glEnable(lights[m_light_id]);
}

/**
* Method to disable a light source.
*/
LIB_API void TE_Light::disable(){
	m_enabled = false;
	glDisable(lights[m_light_id]);
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

	//glLoadMatrixf(glm::value_ptr(m_parent->get_position()*m_position))

	TE_VEC4 position;

	switch (m_lightType){
		case aiLightSource_POINT:
			//std::cout << "omnidirectional" << std::endl;
			position = TE_VEC4(m_light_pos, 1.0f);
			 //position = glm::vec4(m_position[3][0], m_position[3][1], m_position[3][2], 1.0f);
			//m_cutoff = 180.0f;

			glLightfv(light_id, GL_POSITION, glm::value_ptr(position));
			glLightf(light_id, GL_SPOT_CUTOFF, m_cutoff);
			break;
		case aiLightSource_SPOT:
			//std::cout << "spotlight" << std::endl;
			//position = glm::vec4(m_position[3][0], m_position[3][1], m_position[3][2], 1.0f);
			position = TE_VEC4(m_light_pos, 1.0f);
			
			glLightfv(light_id, GL_POSITION, glm::value_ptr(position));
			glLightf(light_id, GL_SPOT_CUTOFF, m_cutoff);
			glLightfv(light_id, GL_SPOT_DIRECTION, glm::value_ptr(m_direction));
			break;
		case aiLightSource_DIRECTIONAL:
			//std::cout << "directional" << std::endl;
			position = TE_VEC4(m_light_pos, 0.0f);
			//float cutoff = 180.0f;

			glLightfv(light_id, GL_POSITION, glm::value_ptr(position));
			//glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, &cutoff);
			break;
	}

	glLightfv(light_id, GL_AMBIENT, glm::value_ptr(m_ambient));
	glLightfv(light_id, GL_DIFFUSE, glm::value_ptr(m_diffuse));
	glLightfv(light_id, GL_SPECULAR, glm::value_ptr(m_specular));
	glLightf(light_id, GL_CONSTANT_ATTENUATION, m_const_atten);
	glLightf(light_id, GL_LINEAR_ATTENUATION, m_linear_atten);
	glLightf(light_id, GL_QUADRATIC_ATTENUATION, m_quad_atten);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f); // default is 0.0f
}

/**
* Destructor.
*/
LIB_API TE_Light::~TE_Light(){
}
