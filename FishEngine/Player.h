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
	bool updateHook = true;
	bool returnRope = false;
	int WIDTH = 1920;
	int HEIGHT = 1280;
	InputHandler inputhandler;
public:
	DirectX::XMFLOAT3 fishingRodPos = DirectX::XMFLOAT3(0,0,0);
	DirectX::XMFLOAT3 hookPos = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 ropePos = DirectX::XMFLOAT3(0, 0, 0);
	Player();
	YSE::sound jumpSound;
	void updatePlayer(Tool* tool, Tool* hook, Tool* rope);
	void updatePlayerTools(Tool * rod, Tool* hook, Tool* rope, double deltaTime);

	
	float boostReserve = 10.f;
};