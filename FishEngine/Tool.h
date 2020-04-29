#pragma once
#pragma once
#include "yse.hpp"
#include "Character.h"
#include "assert.h"
#include "InputHandler.h"
#include <iostream>

class Tool: public Character
{
private:
	InputHandler inputhandler;
	bool isAttack;
public:
	Tool();
	YSE::sound slapSound;
	float pullback = 30.f;

	void throwHook(Tool* hook);
};
