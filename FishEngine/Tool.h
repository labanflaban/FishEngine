#pragma once
#pragma once
#include "yse.hpp"
#include "Character.h"
#include "assert.h"

class Tool: public Character
{
private:
	bool isAttack;
public:
	Tool();
	YSE::sound slapSound;
	float pullback = 30.f;
	void attack();
	void pullbackRod();
};
