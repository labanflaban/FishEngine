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
	float dX = model->getTranslation().x - plr->model->getTranslation().x;
	float dY = model->getTranslation().y - plr->model->getTranslation().y-20.f;
	float angle = atan2f(dX, dY);//math.atan(deltaX, deltaY)

	this->light->setPosition(this->model->getTranslation());

	//std::cout << dX << " " << dY << std::endl;

	xVel = sin(angle) * 0.5f;
	yVel = cos(angle) * 0.5f;

	float baseHeight = plr->model->getTranslation().y - 25.f;
	float diveRange = 25.f;

	if (abs(dX) < diveRange) //If close enough to dive
		model->move(DirectX::XMFLOAT3(-xVel, -yVel, 0));
		//model->move(DirectX::XMFLOAT3(0, 0, 0));
	else
		if (model->getTranslation().y < baseHeight ) //If not diving, then return to original height
			model->move(DirectX::XMFLOAT3(-xVel, 0.2*(-dY/abs(dY)), 0));
		else
			model->move(DirectX::XMFLOAT3(-xVel, 0, 0));

	model->setRotation(DirectX::XMFLOAT3(0, 0, -angle));
}
