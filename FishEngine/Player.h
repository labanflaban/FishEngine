#pragma once

#include "yse.hpp"
#include "Character.h"
#include "assert.h"
#include "Tool.h"
#include "Light.h"

class Player : public Character
{
private:
public:
	DirectX::XMFLOAT3 fishingRodPos = DirectX::XMFLOAT3(0,0,0);
	Player();
	Light* playerLight = nullptr;

	YSE::sound jumpSound;
	void updatePlayer(Tool* tool);
	float boostReserve = 10.f;
};