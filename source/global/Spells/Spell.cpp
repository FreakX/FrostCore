#include "Spell.h"

void BuffMap::AddBuff(Buff* new_buff)
{
	m_buffmap.push_back(new_buff);
};

void BuffMap::Update()
{
	std::vector<Buff*>::iterator itr;
	for( itr = m_buffmap.begin(); itr != m_buffmap.end(); itr++)
	{
		(*itr)->Update();
	}
};