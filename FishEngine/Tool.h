#pragma once
#pragma once
#include "yse.hpp"
#include "Character.h"
#include "assert.h"
<<<<<<< Updated upstream
=======
#include "DXHandler.h"
#include <vector>
#include "InputHandler.h"
#include<cmath>
#include <iostream>
#include "Light.h"
>>>>>>> Stashed changes

class Tool
{
private:
<<<<<<< Updated upstream
	bool isAttack;
=======
	InputHandler* inputhandler = nullptr;
	float WIDTH = 1920.0f;
	float HEIGHT = 1280.0f;

	float hookPosInGameX = 835.0f;
	float hookPosInGameY = 330.0f;

	float multiplierX;
	float multiplierY;

	float hookPosOnScreenX;
	float hookPosOnScreenY;

	float throwHookVelocityX = 25.0f;
	float throwHookVelocityY = 25.0f;

	float zipBackVelocityX = 25.0f;
	float zipBackVelocityY = 25.0f;

	float hookSpeed = 8.0;
>>>>>>> Stashed changes
public:
	Tool(InputHandler* handler);
	YSE::sound slapSound;
<<<<<<< Updated upstream
	float pullback = 30.f;
	void attack();
	void pullbackRod();
=======
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
	void updateRope(Tool* rod, Tool* hook, Tool* rope);
	void throwHook(Tool* rod, Tool* hook, Tool* rope);
>>>>>>> Stashed changes
};
