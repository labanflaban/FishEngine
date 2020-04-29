#include "Player.h"
using namespace DirectX;

Player::Player()
{
	jumpSound.create("./Sounds/Ah.wav");
	jumpSound.setVolume(0.1);
	assert(jumpSound.isValid());
}

void Player::updatePlayer(Tool* tool, Tool* hook)
{
	

	fishingRodPos = XMFLOAT3(model->getTranslation().x + 3, model->getTranslation().y+10, model->getTranslation().z);
	hookPos = XMFLOAT3(model->getTranslation().x + 10, model->getTranslation().y + 20, model->getTranslation().z);

	btTransform transform;
	transform.setOrigin(btVector3(fishingRodPos.x, fishingRodPos.y, fishingRodPos.z));
	transform.setOrigin(btVector3(hookPos.x, hookPos.y, hookPos.z));

	hook->model->setTranslation(hookPos);
	hook->model->rigidBody->setWorldTransform(transform);
	tool->model->setTranslation(fishingRodPos);
	tool->model->rigidBody->setWorldTransform(transform);
}

void Player::updatePlayerTools(Tool* rod, Tool* hook ,double deltaTime)
{
	hook->throwHook(hook);
	XMFLOAT3 currentRotation = rod->model->getRotation();
	XMFLOAT3 currentPosistion = hook->model->getTranslation();

	if (GetAsyncKeyState(0x01) && currentRotation.z > -1 && pull == false && rod->pullback >= 10.f)// left mouse buttom
	{
		XMFLOAT3 rotationRod(0, 0, currentRotation.z - 1);
		rod->model->setRotation(rotationRod);
		rod->pullback -= 10.f;
		hook->model->setTranslation(DirectX::XMFLOAT3(hook->model->getTranslation().x + 5, hook->model->getTranslation().y - 10, hook->model->getTranslation().z));
	}
	if (currentRotation.z <= -1)
	{
		pull = true;
		rod->slapSound.play();
	}
	if (currentRotation.z < 0 && rod->pullback >= 1.f && pull == true)
	{
		//std::cout << "UPDATED" << std::endl;
		rod->model->setRotation(DirectX::XMFLOAT3(0, 0, currentRotation.z + 0.5));
		//fishingRod->pullback -= 3.f;
	}
	if (currentRotation.z >= 0)
	{
		pull = false;
	}
}
