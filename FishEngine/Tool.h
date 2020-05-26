
#pragma once
#include "yse.hpp"
#include "Character.h"
#include "assert.h"
#include "DXHandler.h"
#include <vector>
#include "InputHandler.h"
#include<cmath>
#include <iostream>

class Tool
{
private:
	InputHandler* inputhandler = nullptr;
	int WIDTH = 1920;
	int HEIGHT = 1280;

	float hookPosInGameX = (WIDTH / 2) + 62.0f;
	float hookPosInGameY = (HEIGHT / 2) - 222.0f;

	float multiplierX;
	float multiplierY;

	float hookPosOnScreenX;
	float hookPosOnScreenY;

	float throwHookVelocityX = 45.0f;
	float throwHookVelocityY = 45.0f;

	float zipBackVelocityX = 45.0f;
	float zipBackVelocityY = 45.0f;
public:
	Tool(InputHandler* handler);
	YSE::sound slapSound;
	Light* hookLight = nullptr;
	float pullback = 30.f;
	float ableToThrowHook = 10.f;
	float ropeZipBack = 1.f;
	btTransform transform;

	bool calculateRopePos = false;
	bool isActive = false;
	float xVel, yVel = 0.f;

	Mesh* model = nullptr;

	void zipBackRope(Tool* rod, Tool* hook, Tool* rope);
	void updateRope(Tool* rod, Tool* hook, Tool* rope, DirectX::XMVECTOR hookAttach);
	void throwHook(Tool* rod, Tool* hook, Tool* rope);
};
