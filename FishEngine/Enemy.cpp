#include "Enemy.h"

Enemy::Enemy(ID3D11Device* device)
{
	this->model = new AnimatedMesh(device);
	//this->model->readMeshFromFile("./Models/actualCube.obj");
	this->model->setScaling(DirectX::XMFLOAT3(5, 5, 5));
	std::vector<Vertex> target1 = FIDParser::readFromFID("./Models/Fish_Right.FID");
	std::vector<Vertex> target2 = FIDParser::readFromFID("./Models/Fish_Left.FID");
	this->model->readTextureFromFile(L"./Models/FISHCOLOR.png");
	std::vector<Vertex>* fishArr[] = { &target1, &target2 };
	model->appendStructuredBuffer(fishArr, 2);
	model->createStructuredBuffer(DxHandler::devicePtr);
	model->targetPoseIndex = 1;
	model->animationSpeed = 0.3;
	//

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
	model->rigidBody->setActivationState(ACTIVE_TAG);
	model->rigidBody->setActivationState(DISABLE_DEACTIVATION);
	model->rigidBody->clearGravity();

	//model->rigidBody->setLinearVelocity(btVector3(0, 0, 0));

	float dX = model->getTranslation().x - plr->model->getTranslation().x;
	float dY = model->getTranslation().y - plr->model->getTranslation().y - 20.f;
	float angle = atan2f(dX, dY);//math.atan(deltaX, deltaY)

	this->light->setPosition(this->model->getTranslation());


	xVel = sin(angle) * 1.2f;
	yVel = cos(angle) * 0.5f;

	float baseHeight = plr->model->getTranslation().y;
	float diveRange = 30.f;

	int randomNr = rand() % 10;
	if (abs(dX) < diveRange)//If close enough to dive
	{
		//model->move(DirectX::XMFLOAT3(-xVel, -yVel, 0));
		model->rigidBody->setLinearVelocity(btVector3(-xVel * 3, -yVel * 3, 0));
	}
	else
	{
		if (model->getTranslation().y < baseHeight)//If not diving, then return to original height
		{
			//model->move(DirectX::XMFLOAT3(-xVel, 0.2*(-dY/abs(dY)), 0));
			model->rigidBody->setLinearVelocity(btVector3(-xVel, 3.0f * -dY / abs(dY), 0));
		}
		else
		{
			//model->move(DirectX::XMFLOAT3(-xVel, 0, 0));


			model->rigidBody->setLinearVelocity(btVector3(-xVel, 0, 0));

		}
	}

	//model->setRotation(DirectX::XMFLOAT3(0, 0, angle));

	if (model->getTranslation().x < plr->model->getTranslation().x)
	{
		model->setRotation(DirectX::XMFLOAT3(0, -3.14, 3.14/2));
	}
	else
		model->setRotation(DirectX::XMFLOAT3(0, 0, 3.14/2));

	//btTransform transform = model->rigidBody->getWorldTransform();
	//std::cout << transform.getOrigin().x() << " " << transform.getOrigin().y() << " " << transform.getOrigin().z() << " " << std::endl;

	model->rigidBody->clearGravity();

}