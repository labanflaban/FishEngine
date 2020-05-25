#include "Tool.h"

Tool::Tool(InputHandler* handler)
{

	slapSound.create("./Sounds/slap.wav");
	slapSound.setVolume(0.01);
	assert(slapSound.isValid());
	this->inputhandler = handler;

	multiplierX = hookPosInGameX / WIDTH;
	multiplierY = hookPosInGameY / HEIGHT;

	hookPosOnScreenX = multiplierX * WIDTH;
	hookPosOnScreenY = multiplierY * HEIGHT;
}

void Tool::zipBackRope(Tool* rod, Tool* hook, Tool* rope)
{
	
	float dX = hook->model->rigidBody->getWorldTransform().getOrigin().x() - rod->model->rigidBody->getWorldTransform().getOrigin().x();
	float dY = hook->model->rigidBody->getWorldTransform().getOrigin().y() - rod->model->rigidBody->getWorldTransform().getOrigin().y();
	float angle = atan2f(dX, dY);//math.atan(deltaX, deltaY)

	xVel = sin(angle) * zipBackVelocityX;
	yVel = cos(angle) * zipBackVelocityY;

	btVector3 velocityVector(-xVel, -yVel, 0);

	hook->model->rigidBody->setActivationState(ACTIVE_TAG);
	hook->model->rigidBody->setLinearVelocity(velocityVector);
	
}

void Tool::updateRope(Tool* rod, Tool* hook, Tool* rope, DirectX::XMVECTOR hookAttach)
{

	rope->model->rigidBody->setActivationState(DISABLE_DEACTIVATION);

	float deltaX = hook->model->rigidBody->getWorldTransform().getOrigin().x() - DirectX::XMVectorGetX(hookAttach);//rod->model->rigidBody->getWorldTransform().getOrigin().x();
	float deltaY = hook->model->rigidBody->getWorldTransform().getOrigin().y() - DirectX::XMVectorGetY(hookAttach); //rod->model->rigidBody->getWorldTransform().getOrigin().y();

	float hypotenuse = sqrt((pow(deltaX, 2) + pow(deltaY, 2)));

	float angleOfRope = atan2(deltaX, deltaY);

	rope->model->setRotation(DirectX::XMFLOAT3(0, 0, -angleOfRope));
	rope->model->setScaling(DirectX::XMFLOAT3(0.1, hypotenuse / 2, 0.1));
	rope->model->setTranslation(DirectX::XMFLOAT3(DirectX::XMVectorGetX(hookAttach) + (deltaX / 2), DirectX::XMVectorGetY(hookAttach) + (deltaY / 2), DirectX::XMVectorGetZ(hookAttach)));
	rope->model->rigidBody->setActivationState(DISABLE_DEACTIVATION);
}

void Tool::throwHook(Tool* rod, Tool* hook, Tool* rope)
{
	hook->isActive = true;

	float deltaX = inputhandler->getMousePosX() - hookPosOnScreenX;
	float deltaY = inputhandler->getMousePosY() - hookPosOnScreenY;

	float angle = atan2(deltaX, deltaY);

	float velocityX = sin(angle) * throwHookVelocityX;
	float velocityY = cos(angle) * throwHookVelocityY;

	hook->model->rigidBody->setActivationState(DISABLE_DEACTIVATION);
	btVector3 velocityVector(velocityX, -velocityY, 0);

	hook->model->rigidBody->setLinearVelocity(velocityVector);

}
