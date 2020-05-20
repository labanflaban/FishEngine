#include "Player.h"
using namespace DirectX;

Player::Player()
{
	jumpSound.create("./Sounds/Ah.wav");
	jumpSound.setVolume(0.1);
	assert(jumpSound.isValid());
}
void Player::updatePlayer(Tool* tool)
{
	

	fishingRodPos = XMFLOAT3(model->getTranslation().x + 3, model->getTranslation().y+10, model->getTranslation().z);
=======
	if (hook->calculateRopePos)
	{
		rope->updateRope(tool, hook, rope);
	}
	if (updateHook == true)
	{
		hook->model->setTranslation(hookPos);
		transform.setOrigin(btVector3(hookPos.x, hookPos.y, hookPos.z));
		hook->model->rigidBody->setWorldTransform(transform);
		hook->zipBackRope(tool, hook, rope);
		hook->isActive = false;
	}

	btTransform transform;
	transform.setOrigin(btVector3(fishingRodPos.x, fishingRodPos.y, fishingRodPos.z));

	tool->model->setTranslation(fishingRodPos);
	tool->model->rigidBody->setWorldTransform(transform);
}