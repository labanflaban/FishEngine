#include "Enemy.h"

Enemy::Enemy(ID3D11Device* device)
{
	//this->model->readMeshFromFile("./Models/actualCube.obj");
	//this->model->setScaling(DirectX::XMFLOAT3(5, 5, 5));
	//std::vector<Vertex> target1 = FIDParser::readFromFID("./Models/Fish_Right.FID");
	//std::vector<Vertex> target2 = FIDParser::readFromFID("./Models/Fish_Left.FID");

	
	//
	GotHitSound.create("./Sounds/FishSplash.wav");
	GotHitSound.setVolume(0.05);
	this->light = new Light(device);
	light->lightColor = DirectX::XMVectorSet(0, 0, 3, 0); //Blue
}

Enemy::~Enemy()
{
	//delete this->model;
	//delete this->light;
}

void Enemy::update(Player* plr)
{
	if (enemyHitMove < 5.f)
		enemyHitMove += 0.1f;
	if (AmountOfTimeToMove < 5.f)
		AmountOfTimeToMove += 0.1f;
	if (damageIndicatorDebounce < 5.f)
		damageIndicatorDebounce += 0.1f;

	if (enemyHitMove >= 5.0f)
	{
		enemyHitMove -= 5.0f;
		randomDirr = rand() % 2 + 1;
	}

	if (damageIndicatorDebounce < 5.0f)
	{
		light->lightColor = XMVectorSet(3.f, 0, 0, 0.f);
	}
	if (damageIndicatorDebounce >= 5.0f)
	{
		light->lightColor = XMVectorSet(0.f, 0, 1, 0.f);
	}
	//Notice that enemy is hit and moves with getHitMove function
	if (justHit == true && AmountOfTimeToMove >= 5.0f)
	{
		GotHitSound.play();
		int randomNumber;
		randomNumber = rand() % 2 + 1;
		AmountOfTimeToMove -= 5.0f;
		getHitMove();
		if (randomNumber == 1)
		{
			justHit = false;
		}
	}
	model->rigidBody->setActivationState(ACTIVE_TAG);
	//model->rigidBody->setActivationState(DISABLE_DEACTIVATION);
	model->rigidBody->clearGravity();


	float dX = model->getTranslation().x - plr->model->getTranslation().x;
	float dY = model->getTranslation().y - plr->model->getTranslation().y;
	float angle = atan2f(dX, dY);//math.atan(deltaX, deltaY)

	this->light->setPosition(this->model->getTranslation());


	xVel = sin(angle) * 2.f;
	yVel = cos(angle) * 2.f;

	float baseHeight = plr->model->getTranslation().y;
	float diveRange = 50.f;

	int randomNr = rand() % 10;


	if (justHit == false)
	{
		if (dX < 500)
		{
			if (dX < diveRange)//If close enough to dive
			{
				model->rigidBody->setLinearVelocity(btVector3(-xVel * 2, -yVel * 3, 0));
			}
			else
			{

				if (model->getTranslation().y < baseHeight + 25.0f)//If not diving, then return to original height
				{
					model->rigidBody->setLinearVelocity(btVector3(-xVel, 5.0f, 0));
				}
				else
				{
					model->rigidBody->setLinearVelocity(btVector3(-xVel, 0, 0));
				}
			}
			model->setRotation(DirectX::XMFLOAT3(0, 0, angle));
		}
	}

	//Rotate the model to direction of player
	if (model->getTranslation().x < plr->model->getTranslation().x)
	{
		model->setRotation(DirectX::XMFLOAT3(0, 3.14/2.f, 0));
	}
	else
		model->setRotation(DirectX::XMFLOAT3(0, -3.14 / 2.f, 0));

}


void Enemy::getHitMove()
{
	if (randomDirr == 1)
	{
		model->rigidBody->setLinearVelocity(btVector3(25.0f, 1.0f, 0));
	}
	else
	{
		model->rigidBody->setLinearVelocity(btVector3(-25.0f, 1.0f, 0));
	}
}

void Enemy::moveAway()
{
	btTransform trans;
	trans.setIdentity();

	std::uniform_real_distribution<> randomNumPlacement(-1000.f, 1000.f);
	std::random_device randomSeed;
	std::mt19937 numberGenerator(randomSeed());

	trans.setOrigin(btVector3(randomNumPlacement(numberGenerator), 100, randomNumPlacement(numberGenerator)));

	this->model->rigidBody->setLinearVelocity(btVector3(0, 0, 0));
	this->model->rigidBody->setWorldTransform(trans);
	this->health = maxHealth;
	this->model->rigidBody->setActivationState(ISLAND_SLEEPING);
}

void Enemy::resetEnemy()
{
	this->model->rigidBody->setActivationState(ACTIVE_TAG);
	this->active = true;
	
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(startPos.x, startPos.y, startPos.z));

	this->model->rigidBody->setWorldTransform(trans);
	this->health = maxHealth;
}
