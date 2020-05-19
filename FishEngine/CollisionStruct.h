#pragma once
#include "Tool.h"
#include "Enemy.h"

struct collisionEnums
{
	enum collisionType
	{
		Hook,
		Rod,
		Enemy,
		Player,
		Ignored
	};
};

class collisionStruct
{
public:
	collisionEnums::collisionType type;
	Enemy* enemy = nullptr;
	Tool* rod = nullptr;
	Tool* hook = nullptr;
	Player* player = nullptr;

	collisionStruct(Enemy* enemy = nullptr, collisionEnums::collisionType type = collisionEnums::Ignored)
	{
		this->type = type;
		
		if(type == collisionEnums::Enemy)
			this->enemy = enemy;

	}
	collisionStruct(Tool* tool = nullptr, collisionEnums::collisionType type = collisionEnums::Ignored)
	{
		this->type = type;

		if (type == collisionEnums::Rod)
			this->rod = tool;
		if (type == collisionEnums::Hook)
			this->hook = tool;
	}

	collisionStruct(Player* plr = nullptr, collisionEnums::collisionType type = collisionEnums::Ignored)
	{
		this->type = type;

		if (type == collisionEnums::Player)
			this->player = plr;
	}
};