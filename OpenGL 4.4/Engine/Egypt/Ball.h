#pragma once
#include "TE_Engine.h"

class Ball
{
public:
	TE_VEC3 Direction;
	TE_MAT4 get_position();
	void Update(float);
	bool isLost();
	Ball(TE_Node*);
	~Ball();
private:
	TE_Node* m_node;
	bool m_lost = false;
};

