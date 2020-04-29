#pragma once

#include "yse.hpp"
#include "Character.h"
#include "assert.h"
#include "Tool.h"
#include <Keyboard.h>


class Player : public Character
{
private:
	bool pull;
public:
	DirectX::XMFLOAT3 fishingRodPos = DirectX::XMFLOAT3(0,0,0);
	DirectX::XMFLOAT3 hookPos = DirectX::XMFLOAT3(0, 0, 0);
	Player();
	YSE::sound jumpSound;
	void updatePlayer(Tool* tool, Tool* hook);
	void updatePlayerTools(Tool * rod, Tool* hook, double deltaTime);

	float boostReserve = 10.f;
};