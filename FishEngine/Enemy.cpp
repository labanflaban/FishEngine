#include "Enemy.h"

Enemy::Enemy(ID3D11Device* device)
{
	this->model = new Mesh(device);
	this->model->readMeshFromFile("./Models/actualCube.obj");
	this->model->setScaling(DirectX::XMFLOAT3(10, 10, 10));
	
	this->startPosX = startPosX;
	this->startPosY = startPosY;
	this->startPosZ = startPosZ;

	this->light = new Light(device);
}
 
Enemy::~Enemy()
{
	//delete this->model;
	//delete this->light;
}

//void Enemy::defaultMoveCirkel()
//{
//
//	
//	endPosX = startPosX + motionRange;
//	float dXX = model->rigidBody->getWorldTransform().getOrigin().x() - startPosX;
//	float dYY = model->rigidBody->getWorldTransform().getOrigin().y() - startPosY;
//	float angle = atan2f(dXX, dYY);//math.atan(deltaX, deltaY)
//
//	xEnemy = sin(angle) * 5.f;
//	yEnemy = cos(angle) * 5.f;
//
//	btVector3 velocityVector(-xEnemy, -yEnemy, 0);
//
//	if (dXX != startPosX && dYY != startPosY && defaultCirkelMovement == false)
//	{
//		model->rigidBody->setLinearVelocity(velocityVector);
//	}
//	
//	if (model->getTranslation().x == startPosX && model->getTranslation().y == startPosY && defaultCirkelMovement == false)
//	{
//		defaultCirkelMovement = true;
//		deufaultRotationMovement = true;
//	}
//	//bool deufaultRotationMovement = false;
//	if (deufaultRotationMovement == true)
//	{
//		if (model->getTranslation().x < startPosX) // move right
//		{
//			movementSpeed = movementSpeed * -1.0f;
//		}
//		if (model->getTranslation().x > endPosX) // move left
//		{
//			movementSpeed = movementSpeed * -1.0f;
//		}
//		model->rigidBody->setLinearVelocity(btVector3(movementSpeed, 0, 0));
//	}
//
//
//}

void Enemy::setPosOfEnemy(float startX, float startY, float startZ)
{
	startPosX = startX;
	startPosY = startY;
	startPosZ = startZ;

	model->setTranslation(DirectX::XMFLOAT3(startX, startY, startZ));
}

void Enemy::update(Player* plr)
{
	model->rigidBody->setActivationState(ACTIVE_TAG);
	model->rigidBody->clearGravity();


	float deltaX = model->getTranslation().x - plr->model->getTranslation().x;
	float deltaY = model->getTranslation().y - plr->model->getTranslation().y;

	float hypotenuse = sqrt((pow(deltaX, 2) + pow(deltaY, 2)));

	
	if (hypotenuse > engageMode && model->getTranslation().y == 100.0f)
	{
		model->rigidBody->setLinearVelocity(btVector3(0, 10.0f, 0));
	}

	float dX = model->getTranslation().x - plr->model->getTranslation().x;
<<<<<<< Updated upstream
	float dY = model->getTranslation().y - plr->model->getTranslation().y-20.f;
=======
	float dY = model->getTranslation().y - plr->model->getTranslation().y - 20.f;
>>>>>>> Stashed changes
	float angle = atan2f(dX, dY);//math.atan(deltaX, deltaY)

	this->light->setPosition(this->model->getTranslation());

	xVel = sin(angle) * 1.2f;
	yVel = cos(angle) * 0.5f;

<<<<<<< Updated upstream
	float baseHeight = plr->model->getTranslation().y - 25.f;
	float diveRange = 25.f;

	if (abs(dX) < diveRange) //If close enough to dive
		model->move(DirectX::XMFLOAT3(-xVel, -yVel, 0));
		//model->move(DirectX::XMFLOAT3(0, 0, 0));
=======
	float baseHeight = plr->model->getTranslation().y;
	float diveRange = 30.f;

	int randomNr = rand() % 10;
	
	if (abs(dX) < diveRange)//If close enough to dive
	{
		model->rigidBody->setLinearVelocity(btVector3(-xVel * 3, -yVel * 3, 0));
	}
>>>>>>> Stashed changes
	else
	{
		if (model->getTranslation().y < baseHeight)//If not diving, then return to original height
		{
			model->rigidBody->setLinearVelocity(btVector3(-xVel, 3.0f * -dY / abs(dY), 0));
		}
		else
		{
			model->rigidBody->setLinearVelocity(btVector3(-xVel, 0, 0));
		}
	}
	

	model->setRotation(DirectX::XMFLOAT3(0, 0, -angle));

	model->rigidBody->clearGravity();

}