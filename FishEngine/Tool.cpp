#include "Tool.h"

Tool::Tool(InputHandler* handler)
{
	slapSound.create("./Sounds/slap.wav");
	slapSound.setVolume(0.01);
	assert(slapSound.isValid());
	this->inputhandler = handler;
}

void Tool::zipBackRope(Tool* rod, Tool* hook, Tool* rope)
{
	float dX = hook->model->rigidBody->getWorldTransform().getOrigin().x() - rod->model->rigidBody->getWorldTransform().getOrigin().x();

	float dY = hook->model->rigidBody->getWorldTransform().getOrigin().y() - rod->model->rigidBody->getWorldTransform().getOrigin().y();
	float angle = atan2f(dX, dY);//math.atan(deltaX, deltaY)

	//std::cout << dX << " " << dY << std::endl;

	xVel = sin(angle) * 25.f;
	yVel = cos(angle) * 25.f;

	btVector3 velocityVector(-xVel, -yVel, 0);

	hook->model->rigidBody->setActivationState(ACTIVE_TAG);
	hook->model->rigidBody->setLinearVelocity(velocityVector);
	//float baseHeight = rod->model->getTranslation().y + 10.f;
	//float diveRange = 25.f;

	//if (abs(dX) < diveRange) //If close enough to dive
		//hook->model->move(DirectX::XMFLOAT3(-xVel, -yVel, 0));

	//else
	//	if (rod->model->getTranslation().y < baseHeight) //If not diving, then return to original height
	//		hook->model->move(DirectX::XMFLOAT3(-xVel, 0.2 * (-dY / abs(dY)), 0));
	//	else
	//		hook->model->move(DirectX::XMFLOAT3(-xVel, 0, 0));
	 

}

void Tool::updateRope(Tool* rod, Tool* hook, Tool* rope)
{

	rope->model->rigidBody->setActivationState(DISABLE_DEACTIVATION);

	float deltaX = hook->model->rigidBody->getWorldTransform().getOrigin().x() - rod->model->rigidBody->getWorldTransform().getOrigin().x();

	float deltaY = hook->model->rigidBody->getWorldTransform().getOrigin().y() - rod->model->rigidBody->getWorldTransform().getOrigin().y();
	//float deltaX = hook->model->getTranslation().x - rod->model->getTranslation().x;

	//float deltaY = hook->model->getTranslation().y - rod->model->getTranslation().y;
	
	float hypotenuse = sqrt((pow(deltaX,2) + pow(deltaY,2)));

	float angleOfRope = atan2(deltaX, deltaY);
	

	rope->model->setRotation(DirectX::XMFLOAT3(0,0,-angleOfRope));
	rope->model->setScaling(DirectX::XMFLOAT3(0.1, hypotenuse / 2, 0.1));

	rope->model->setTranslation(DirectX::XMFLOAT3(rod->model->getTranslation().x + (deltaX/2), rod->model->getTranslation().y + (deltaY / 2), rod->model->getTranslation().z));

	rope->model->rigidBody->setActivationState(DISABLE_DEACTIVATION);
}

void Tool::throwHook(Tool* rod, Tool* hook, Tool* rope)
{
	hook->isActive = true;
	float deltaX = inputhandler->getMousePosX() - WIDTH/2;

	float deltaY = inputhandler->getMousePosY() - HEIGHT/2;

	float angle = atan2(deltaX, deltaY);
	

	float velocityX = sin(angle) * 50;
	float velocityY = cos(angle) * 50;

	hook->model->rigidBody->setActivationState(DISABLE_DEACTIVATION);
	btVector3 velocityVector(velocityX , -velocityY, 0);
	
	hook->model->rigidBody->setActivationState(ACTIVE_TAG);
	hook->model->rigidBody->setLinearVelocity(velocityVector);
	

}
