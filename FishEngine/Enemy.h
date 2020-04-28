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
	Mesh* model = nullptr;
	Light* light = nullptr;
	Enemy(ID3D11Device* device);
	~Enemy();

	int health = 100;
	float xVel, yVel = 0.f;

	virtual void update(Player* plr);
};