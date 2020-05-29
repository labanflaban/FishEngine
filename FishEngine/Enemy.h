#pragma once
#include "Player.h"
#include "yse.hpp"
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

	int randomDirr = 0;
	int maxHealth = 100;
	int health = maxHealth;
	bool active = true;
	DirectX::SimpleMath::Vector3 startPos;
	float damageIndicatorDebounce = 5.0f;

	float justHit = false;

	float enemyHitMove = 5.f;
	float AmountOfTimeToMove = 5.0f;

	float damageDebounce = 5.0f;
	float maxDebounce = 5.0f;

	float xVel, yVel = 0.f;

	int vectorIndex = 0;

	YSE::sound GotHitSound;

	virtual void update(Player* plr);
	void getHitMove();
	void moveAway();
	void resetEnemy();
};