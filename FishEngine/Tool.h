
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

	float hookSpeed = 8.0;
public:
	Tool(InputHandler* handler);
	YSE::sound slapSound;

	float pullback = 30.f;
	float ableToThrowHook = 10.f;
	float ropeZipBack = 1.f;
	btTransform transform;

	bool isActive = false;
	float xVel, yVel = 0.f;

	Mesh* model = nullptr;

	void zipBackRope(Tool* rod, Tool* hook, Tool* rope);
	void updateRope(Tool* rod, Tool* hook, Tool* rope);
	void throwHook(Tool* rod, Tool* hook, Tool* rope);
};
