#include "MovingPlatform.h"
using namespace DirectX;
using namespace SimpleMath;
void MovingPlatform::updatePlatform()
{
	if (platform)
	{
		this->platform->move(Vector3(1 * speed * dirMult, 0, 0));
		//this->platform->setTranslation(XMFLOAT3(this->platform->getTranslation().x + 0.25 * speed * dirMult, startPos.y, startPos.z));

		//this->platform->rigidBody->clearForces();
		this->platform->rigidBody->setActivationState(DISABLE_DEACTIVATION);
		//this->platform->rigidBody->clearGravity();
		//this->platform->rigidBody->clearForces();
		//this->platform->rigidBody->setActivationState(DISABLE_DEACTIVATION);
		//this->platform->rigidBody->setLinearVelocity(btVector3(4 * dirMult, 0, 0));
		
		//this->platform->rigidBody->setLinearVelocity(btVector3(3 * dirMult, -3, 0));
		btTransform worldTransform;
		worldTransform.setIdentity();
		worldTransform.setOrigin(btVector3(this->platform->getTranslation().x, startPos.y, startPos.z));
		//auto var = this->platform->rigidBody->getMotionState();
		this->platform->rigidBody->getMotionState()->setWorldTransform(worldTransform);
		this->platform->rigidBody->setLinearVelocity(btVector3(2 * speed * dirMult, 0, 0));

		//std::cout << this->platform->rigidBody->getWorldTransform().getOrigin().x() << " " << this->platform->rigidBody->getWorldTransform().getOrigin().y() << " " << this->platform->rigidBody->getWorldTransform().getOrigin().z() << " " << std::endl;

		if (abs(this->platform->getTranslation().x - startPos.x) > range)
			dirMult *= -1;
	}
	else
		throw std::exception("No model for moving platform.");
}
