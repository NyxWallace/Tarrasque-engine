#include "Block.h"


int Block::isHit(TE_VEC2 vec, TE_Node* root)
{
	//Hit on top or on bottom
	if ((vec[1] < by + 1 && vec[1] > by - 1) || (vec[1] < ty + 1 && vec[1] > ty - 1))
	{
		if (vec[0] < rx && vec[0] > lx)
		{
			remove_node(root);
			return 1;
		}
	}

	//Hit on left or right
	if ((vec[0] < lx + 1 && vec[0] > lx - 1) || (vec[0] < rx + 1 && vec[0] > rx - 1))
	{
		if (vec[1] < ty && vec[1] > by)
		{
			remove_node(root);
			return 2;
		}
	}
	return 0;
}

void Block::remove_node(TE_Node* root){
	root->unlink(this->m_node);
	removed = true;
}

Block::Block(TE_Node* node)
{
	m_node = node;
	lx = node->get_position()[3][0] - 9;
	rx = node->get_position()[3][0] + 9;
	ty = node->get_position()[3][1] + 4;
	by = node->get_position()[3][1] - 4;
}


Block::~Block()
{
}
