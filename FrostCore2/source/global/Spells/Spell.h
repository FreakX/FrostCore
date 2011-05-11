
#ifndef SPELL_H_
#define SPELL_H_

#include <vector>
#include "Types.h"
#include "SpellConsts.h"

struct SpellSchoolModifier
{
	SpellSchool m_schooltype;
	float value;
};

class Spell
{
public:
	Spell();
	~Spell();

	void OnCast();
	void OnCancelCast();
	void OnUpdate();
	void OnInterrupt();

private:
	uint64 m_caster;
	uint64 m_target;

	SpellCostPowerType m_costpowertype;
	SpellCostType m_costcalctype;

	uint32 m_spellrange;
	uint32 m_casttime;
};

class Buff
{
public:
	Buff();
	~Buff();

	void Update();
	void OnOwnerHeal();
	void OnOwnerDamaged();
	void OnCasterHeal();
	void OnCasterDamaged();

private:
	uint64 m_owner;
	uint64 m_caster;
	uint64 m_byspell;
};

class BuffMap
{
public:
	BuffMap();
	~BuffMap();

	void AddBuff(Buff* new_buff);
	void RemoveBuff(uint64 from_spell);

	void Update();
	void OnOwnerHeal();
	void OnOwnerDamaged();
	void OnCasterHeal();
	void OnCasterDamaged();

private:
	std::vector<Buff*> m_buffmap;
};

#endif