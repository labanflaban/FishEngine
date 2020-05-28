#include "Heartdrop.h"

void Heartdrop::moveAway()
{
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(-3000, 0, 0));
	this->model->setTranslation(DirectX::XMFLOAT3(-3000, 0, 0));

	this->model->rigidBody->setWorldTransform(trans);
}

void Heartdrop::resetDrop()
{
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(startPos.x, startPos.y, startPos.z));
	this->model->setTranslation(startPos);
	usedUp = false;
	this->model->rigidBody->setWorldTransform(trans);
}
