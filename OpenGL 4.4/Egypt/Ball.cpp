#include "Ball.h"

void Ball::Update(float barPosition)
{
	TE_MAT4 pos = m_node->get_position();
	pos[3][0] += Direction[0];
	pos[3][1] += Direction[1];
	if (pos[3][0] > 82 || pos[3][0] < -82)
		Direction[0] *= -1;
	if (pos[3][1] > 147)
		Direction[1] *= -1;
	if (pos[3][1] < 3 && pos[3][1] > 0 && pos[3][0] < barPosition + 10 && pos[3][0] > barPosition - 10)
		if (pos[3][0] < barPosition - 5){
			Direction[1] *= -1;
			Direction[0] -= .5f;
			Direction = glm::normalize(Direction);
			Direction *= 0.3f;
		}
		else if (pos[3][0] > barPosition + 5){
			Direction[1] *= -1;
			Direction[0] += .5f;
			Direction = glm::normalize(Direction);
			Direction *= 0.3f;
		}
		else{
			Direction[1] *= -1;
		}
	m_node->set_position(pos);

	if (pos[3][1] < -40)
		m_lost = true;
}

bool Ball::isLost()
{
	return m_lost;
}

TE_MAT4 Ball::get_position()
{
	return m_node->get_position();
}

Ball::Ball(TE_Node* ball)
{
	m_node = ball;
	float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX))-.5f;
	float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	Direction = TE_VEC3(x, y, 0);
	Direction = glm::normalize(Direction);
	Direction *= 0.3f;
	TE_MAT4 pos = m_node->get_position();
	pos[3][0] = 0;
	pos[3][1] = 5;
	m_node->set_position(pos);
}


Ball::~Ball()
{
}
