#include "Player.h"
using namespace DirectX;

Player::Player()
{
	jumpSound.create("./Sounds/Ah.wav");
	jumpSound.setVolume(0.01);
	assert(jumpSound.isValid());
}

void Player::updatePlayer(Tool* tool, Tool* hook, Tool* rope)
{
	btTransform transform;
	rope->model->rigidBody->setActivationState(DISABLE_DEACTIVATION);
	rope->updateRope(tool, hook, rope);
	//fishingrod position
	fishingRodPos = XMFLOAT3(model->getTranslation().x + 3, model->getTranslation().y+10, model->getTranslation().z);
	//Rope position
	ropePos = XMFLOAT3(model->getTranslation().x + 10, model->getTranslation().y + 20, model->getTranslation().z);
	//Hook position
	hookPos = XMFLOAT3(model->getTranslation().x + 10, model->getTranslation().y + 20, model->getTranslation().z);


	if (updateHook == true)
	{
		hook->model->setTranslation(hookPos);
		transform.setOrigin(btVector3(hookPos.x, hookPos.y, hookPos.z));
		hook->model->rigidBody->setWorldTransform(transform);
		hook->zipBackRope(tool, hook, rope);
	}

	transform.setOrigin(btVector3(fishingRodPos.x, fishingRodPos.y, fishingRodPos.z));
	transform.setOrigin(btVector3(ropePos.x, ropePos.y, ropePos.z));

	tool->model->setTranslation(fishingRodPos);
	tool->model->rigidBody->setWorldTransform(transform);

	//transform = hook->model->rigidBody->getWorldTransform();
	//std::cout << transform.getOrigin().x() << " " << transform.getOrigin().y() << " " << transform.getOrigin().z() << " " << std::endl;
}

void Player::updatePlayerTools(Tool* rod, Tool* hook, Tool* rope, double deltaTime)
{
	XMFLOAT3 currentRotation = rod->model->getRotation();
	XMFLOAT3 currentPosistion = hook->model->getTranslation();
	hook->model->rigidBody->setActivationState(DISABLE_DEACTIVATION);
	rope->model->rigidBody->setActivationState(DISABLE_DEACTIVATION);

	if (GetAsyncKeyState(0x01) && currentRotation.z > -1 && pull == false && rod->pullback >= 10.f )// left mouse buttom
	{
		XMFLOAT3 rotationRod(0, 0, currentRotation.z - 1);
		rod->model->setRotation(rotationRod);
		rod->pullback -= 10.f;
		hook->model->setTranslation(DirectX::XMFLOAT3(hook->model->getTranslation().x + 5, hook->model->getTranslation().y - 10, hook->model->getTranslation().z));
	}
	if (GetAsyncKeyState(0x02) && hook->ableToThrowHook >= 10.0f)// right mouse buttom
	{
		hook->ableToThrowHook -= 10.f;
		hook->ropeZipBack -= 10.f;
		updateHook = false;

		if (updateHook == false)
		{

			hook->throwHook(rod, hook, rope);

		}
	}
	if (hook->ropeZipBack >= 1 && updateHook == false)
	{
		hook->zipBackRope(rod, hook, rope);
		if (rod->model->getTranslation().y + 10 - hook->model->getTranslation().y < 0.001f && rod->model->getTranslation().x + 3- hook->model->getTranslation().x < 0.001f)
		{
			
			updateHook = true;
		}
	}

	if (currentRotation.z <= -1)
	{
		pull = true;
		rod->slapSound.play();
	}
	if (currentRotation.z < 0 && rod->pullback >= 1.f && pull == true)
	{
		rod->model->setRotation(DirectX::XMFLOAT3(0, 0, currentRotation.z + 0.5));
	}
	if (currentRotation.z >= 0)
	{
		pull = false;
	}

}
