#pragma once
#include "TE_Engine.h"

class Block
{
public:
	int isHit(TE_VEC2, TE_Node*);
	bool removed = false;
	Block(TE_Node*);
	~Block();
private:
	void remove_node(TE_Node*);
	TE_Node* m_node;
	float lx, rx, ty, by;
};

