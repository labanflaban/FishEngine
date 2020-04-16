#pragma once

#include "yse.hpp"
#include "Character.h"
#include "assert.h"

class Player : public Character
{
private:

public:
	Player();
	YSE::sound jumpSound;
	float boostReserve = 10.f;
};