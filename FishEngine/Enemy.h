#pragma once
#include "Player.h"
#include "Mesh.h"
#include <math.h>
#include "Light.h"
#define _USE_MATH_DEFINES

class Enemy
{
private:

public:
	AnimatedMesh* model = nullptr;
	Light* light = nullptr;
	Enemy(ID3D11Device* device);
	~Enemy();

	bool justHit = false;

	int health = 100;
	
	float damageDebounce = 5.0f;
	float maxDebounce = 5.0f;

	float xVel, yVel = 0.f;

	int vectorIndex = 0;

	virtual void update(Player* plr);
};