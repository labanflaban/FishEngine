#include "Enemy.h"

Enemy::Enemy(ID3D11Device* device)
{
	this->model = new Mesh(device);
	this->model->readMeshFromFile("./Models/actualCube.obj");
	this->model->setScaling(DirectX::XMFLOAT3(3, 3, 3));

	this->light = new Light(device);
	light->lightColor = DirectX::XMVectorSet(0, 0, 1, 0); //Blue
}

Enemy::~Enemy()
{
	delete this->model;
	delete this->light;
}

void Enemy::update(Player* plr)
{
	model->rigidBody->setActivationState(ACTIVE_TAG);
	model->rigidBody->clearGravity();

	//model->rigidBody->setLinearVelocity(btVector3(0, 0, 0));

	float dX = model->getTranslation().x - plr->model->getTranslation().x;
	float dY = model->getTranslation().y - plr->model->getTranslation().y-20.f;
	float angle = atan2f(dX, dY);//math.atan(deltaX, deltaY)

	this->light->setPosition(this->model->getTranslation());

	//std::cout << dX << " " << dY << std::endl;

	xVel = sin(angle) * 1.2f;
	yVel = cos(angle) * 0.5f;

	float baseHeight = plr->model->getTranslation().y;
	float diveRange = 30.f;

	int randomNr = rand() % 10;
	if (abs(dX) < diveRange)//If close enough to dive
	{ 
		//model->move(DirectX::XMFLOAT3(-xVel, -yVel, 0));
		model->rigidBody->setLinearVelocity(btVector3(-xVel*5, -yVel*5, 0));
	}
	else
	{
		if (model->getTranslation().y < baseHeight)//If not diving, then return to original height
		{ 
			//model->move(DirectX::XMFLOAT3(-xVel, 0.2*(-dY/abs(dY)), 0));
			model->rigidBody->setLinearVelocity(btVector3(-xVel, 3.0f* -dY / abs(dY), 0));
		}
		else
		{
			//model->move(DirectX::XMFLOAT3(-xVel, 0, 0));
			

			model->rigidBody->setLinearVelocity(btVector3(-xVel, 0, 0));



		}
	}

	model->setRotation(DirectX::XMFLOAT3(0, 0, -angle));

	model->rigidBody->clearGravity();
	
}
