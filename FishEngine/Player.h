#pragma once

#include "yse.hpp"
#include "Character.h"
#include "assert.h"
#include "Tool.h"
#include "Light.h"
#include <Keyboard.h>
#include "SimpleMath.h"

using namespace DirectX;
class Player : public Character
{
private:
	bool pull;
	bool updateHook = true;
	bool returnRope = false;
	int WIDTH = 1920;
	int HEIGHT = 1280;
	InputHandler* inputhandler = nullptr;

	XMFLOAT3 hookOffset = XMFLOAT3(0, 0, 0);
public:
	SimpleMath::Matrix rodMatrix;

	int maxHealth = 5;
	int health = maxHealth;
	unsigned int points = 0;
									//Walk start								//Walk middle1						//Middle2						//Middle3							//FishingStart							Fishing Mid						//Fishing End						//Fishing mid						//Fishing start
	XMVECTOR handPositions[9] = { XMVectorSet(2.05, 10.90, 2.82, 1), XMVectorSet(3.14, 10.34, 1.64, 1), XMVectorSet(3.10, 10.05, -1.44, 1), XMVectorSet(3.07, 10.33, 1.91, 1), XMVectorSet(2.14, 12.44, 4.44, 1), XMVectorSet(2.30, 17.07, 3.97, 1), XMVectorSet(2.30, 17.45, 1.09, 1), XMVectorSet(2.30, 17.07, 3.97, 1), XMVectorSet(2.14, 12.44, 4.44, 1) };
										//Walk start							//Walk middle1									//Walk middle2						//Walk middle 3										//FishingStart								//FishingMid							//FishingEnd
	XMVECTOR handRotations[9] = { XMVectorSet(0, -114.76, 19.262, 1), XMVectorSet(101.477, -50.173, 0, 1), XMVectorSet(61.567, -25.359, -19, 1), XMVectorSet(93.726, -49.806, -30, 1), XMVectorSet(31.14, -81.223,-29.069, 1), XMVectorSet(-84.336, -11.804, 86.006, 1), XMVectorSet(-186.225, 51.287, 270, 1) };

	XMVECTOR currentHandPosition;
	double linearTime = 0.0;
	float xMov = 1;

	DirectX::XMFLOAT3 fishingRodPos = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 hookPos = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 ropePos = DirectX::XMFLOAT3(0, 0, 0);
	Player(InputHandler* handler);
	Light* playerLight = nullptr;

	float rodOffsetX = 3.0f;
	float rodOffsetY = 10.0f;

	float ropeOffsetX = 10.0f;
	float ropeOffsetY = 20.0f;

	float hookOffsetX = 10.0f;
	float hookOffsetY = 20.0f;

	float hookPositionCheck = 10.0f;

	int walkAnimPoses = 4;
	bool attacking = false;

	YSE::sound jumpSound;
	void updatePlayer(Tool* rod, Tool* hook, Tool* rope, float xVel);
	void updatePlayerTools(Tool* rod, Tool* hook, Tool* rope, double deltaTime);
	float boostReserve = 10.f;

	void stepAnim(double deltaT);
};