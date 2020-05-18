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
	
	float startPosX;
	float startPosY;
	float startPosZ;

	float movementSpeed = 5.0f;
	float motionRange = 100.0f; // Lenght of back and forth movement of enemy
	float endPosX;
	float damageDebounce = 5.0f;
	float maxDebounce = 5.0f;
	float engageMode = 100.0f; // if in this range to enemy, enemy will engage player




	float xVel, yVel = 0.f;
	float xEnemy, yEnemy = 0.f;
	int vectorIndex = 0;

	void defaultMoveCirkel();
	void setPosOfEnemy(float startX, float startY, float startZ);
	virtual void update(Player* plr);
};