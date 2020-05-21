#include "Player.h"
#include <cmath>
using namespace DirectX;
using namespace SimpleMath;

Player::Player(InputHandler* handler)
{
	jumpSound.create("./Sounds/Ah.wav");
	jumpSound.setVolume(0.01);
	this->inputhandler = handler;
	assert(jumpSound.isValid());
}

void Player::updatePlayer(Tool* rod, Tool* hook, Tool* rope, float xVel)
{
	btTransform transform;
	rope->model->rigidBody->setActivationState(DISABLE_DEACTIVATION);
	XMVECTOR lerpPosition = XMVectorLerp(handPositions[this->model->fromIndex], handPositions[this->model->targetPoseIndex], this->linearTime);
	//Update position for rod
	currentHandPosition = lerpPosition;//XMVector3Transform(lerpPosition, this->model->getWorldMatrix());//XMVectorLerp(handPositions[this->model->fromIndex], handPositions[this->model->targetPoseIndex], this->linearTime);

	
	if (abs(xVel) > 0)
		this->xMov = xVel;
	
		
	if (this->xMov > 0)
	{
		//currentHandPosition += XMVectorSet(6, 0, 0, 0);
	}
	else
		if (this->xMov < 0)
		{
			//currentHandPosition -= XMVectorSet(6, 0, 0, 0);
		}
	fishingRodPos = XMFLOAT3(XMVectorGetX(this->currentHandPosition), XMVectorGetY(this->currentHandPosition), XMVectorGetZ(this->currentHandPosition));
	
	if (this->xMov > 0)
	{
		//hookPos = XMFLOAT3(fishingRodPos.x + 6, fishingRodPos.y + 10, fishingRodPos.z);
		ropePos = XMFLOAT3(fishingRodPos.x + 6, fishingRodPos.y + 10, fishingRodPos.z);
	}
	else
		if (this->xMov < 0)
		{
			//hookPos = XMFLOAT3(fishingRodPos.x - 6, fishingRodPos.y + 10, fishingRodPos.z);
			ropePos = XMFLOAT3(fishingRodPos.x - 6, fishingRodPos.y + 10, fishingRodPos.z);
		}




	if (hook->calculateRopePos)
	{
		rope->updateRope(rod, hook, rope);
	}
	if (updateHook == true)
	{
		//hook->model->setTranslation(hookPos);
		//transform.setOrigin(btVector3(hookPos.x, hookPos.y, hookPos.z));
		//hook->model->rigidBody->setWorldTransform(transform);
		Vector3 translation;
		Quaternion rotation;
		Vector3 scale;

		rodMatrix.Decompose(scale, rotation, translation);
		//hook->model->getWorldMatrix() = rodMatrix * XMMatrixTranslation(4, 3, 0);
		hook->model->setTranslation(translation + hookOffset);
		//std::cout << Vector3(translation).x << " " << Vector3(translation).y << " " << Vector3(translation).z << std::endl;

		hook->model->setRotation(XMFLOAT3(rotation.x, rotation.y, rotation.z));
		hook->model->setScaling(scale);
		hook->model->getWorldMatrix() = Matrix(XMMatrixRotationRollPitchYaw(rotation.x, rotation.y + 3.14 / 2, rotation.z)) * rodMatrix * Matrix(XMMatrixTranslation(hookOffset.x, hookOffset.y, hookOffset.z));

		btTransform transform;
		transform.setOrigin(btVector3(translation.x, translation.y, translation.z));
		//hook->model->rigidBody->setWorldTransform(transform);

		hook->zipBackRope(rod, hook, rope);
		hook->isActive = false;
	}

	//transform.setOrigin(btVector3(fishingRodPos.x, fishingRodPos.y, fishingRodPos.z));
	//transform.setOrigin(btVector3(ropePos.x, ropePos.y, ropePos.z));

	//rod->model->setTranslation(fishingRodPos);
	

	//transform = hook->model->rigidBody->getWorldTransform();
	//std::cout << transform.getOrigin().x() << " " << transform.getOrigin().y() << " " << transform.getOrigin().z() << " " << std::endl;
}


void Player::updatePlayerTools(Tool* rod, Tool* hook, Tool* rope, double deltaTime)
{
	using namespace DirectX;
	using namespace SimpleMath;

	XMFLOAT3 currentRotation = rod->model->getRotation();
	hook->model->rigidBody->setActivationState(DISABLE_DEACTIVATION);
	rope->model->rigidBody->setActivationState(DISABLE_DEACTIVATION);
	rod->model->rigidBody->setActivationState(DISABLE_DEACTIVATION);
	rod->model->rigidBody->setActivationState(ACTIVE_TAG);
	hook->model->rigidBody->setActivationState(ACTIVE_TAG);

	rod->model->rigidBody->clearGravity();
	hook->model->rigidBody->clearGravity();

	if (this->xMov > 0)
	{
		this->hookOffset = Vector3(12, 5, 0);

		Vector3 translation;
		Quaternion rotation;
		Vector3 scale;
		rodMatrix.Decompose(scale, rotation, translation);
		btTransform rodTransform;
		rodTransform.setOrigin(btVector3(translation.x, translation.y, translation.z) + btVector3(hookOffset.x, hookOffset.y, hookOffset.z));
		rod->model->rigidBody->setWorldTransform(rodTransform);
	}
	else
	{
		if (this->xMov < 0)
		{
			this->hookOffset = Vector3(-12, 5, 0);

			Vector3 translation;
			Quaternion rotation;
			Vector3 scale;
			rodMatrix.Decompose(scale, rotation, translation);
			btTransform rodTransform;
			rodTransform.setOrigin(btVector3(translation.x, translation.y, translation.z) + btVector3(hookOffset.x, hookOffset.y, hookOffset.z));
			rod->model->rigidBody->setWorldTransform(rodTransform);
		}
	}

	XMVECTOR lerpedRot = XMVectorLerp(handRotations[this->model->fromIndex], handRotations[this->model->targetPoseIndex], this->linearTime);
	XMFLOAT3 rot = XMFLOAT3(XMVectorGetX(lerpedRot * (3.14 / 180.f)), XMVectorGetY(lerpedRot * (3.14 / 180.f)), XMVectorGetZ(lerpedRot * (3.14 / 180.f)));

	Matrix charAttach = //XMMatrixRotationRollPitchYaw(0, 0, rot.z) * 
		XMMatrixTranslation(XMVectorGetX(currentHandPosition),
        XMVectorGetY(currentHandPosition),
        XMVectorGetZ(currentHandPosition));

	Matrix offset = Matrix(XMMatrixRotationAxis(XMVectorSet(0, 0, 1, 0), rot.z)) * Matrix(XMMatrixRotationAxis(XMVectorSet(0, 1, 0, 0), -3.14/2)) * Matrix(XMMatrixTranslation(5, 0, 0));
	
	Matrix translationMatrix = this->model->getTransMatrix();
	Matrix scalingMatrix = XMMatrixScaling(rod->model->getScaling().x, rod->model->getScaling().y, rod->model->getScaling().z);
	Matrix rotationMatrix = this->model->getRotationMatrix();

	rodMatrix = offset * charAttach * this->model->getWorldMatrix();//scalingMatrix * rotationMatrix * translationMatrix;//this->model->getWorldMatrix();
    rod->model->getWorldMatrix() = rodMatrix;

	


	//rod->model->rotation


	if (this->model->targetPoseIndex < 5)
	{
		this->attacking = false;
		rod->isActive = false;
	}

	if (GetAsyncKeyState(0x01) && !this->attacking) //Left mouse btn
	{
		attacking = true;
		this->model->targetPoseIndex = 5;
		this->model->animationSpeed = 5;
		rod->isActive = true;
		rod->slapSound.play();
	}

	if (GetAsyncKeyState(0x02) && hook->ableToThrowHook >= 10.0f)// right mouse buttom
	{
		//std::cout << "Throw!" << std::endl;
		hook->ableToThrowHook -= 10.f;
		hook->ropeZipBack -= 10.f;
		hook->calculateRopePos = true;
		updateHook = false;

		if (updateHook == false)
		{

			hook->throwHook(rod, hook, rope);

		}
	}
	
	if (hook->ropeZipBack >= 2 && updateHook == false)
	{
		hook->zipBackRope(rod, hook, rope);
		if (((hook->model->getTranslation().y) - (rod->model->getTranslation().y)) < hookPositionCheck && ((hook->model->getTranslation().x) - (rod->model->getTranslation().x)) < hookPositionCheck && ((rod->model->getTranslation().x) - (hook->model->getTranslation().x)) < hookPositionCheck && ((rod->model->getTranslation().y) - (hook->model->getTranslation().y)) < hookPositionCheck)
		{
			updateHook = true;
			hook->calculateRopePos = false;
		}
	}

	if (currentRotation.z <= -1)
	{
		//pull = true;
		
	}

	if (currentRotation.z >= 0)
	{
		pull = false;
	}
}

void Player::stepAnim(double deltaT)
{
	this->linearTime += deltaT * this->model->animationSpeed;
	
	this->model->t = this->model->t + deltaT * this->model->animationSpeed;
	

	if (this->model->t > 0.99f)
	{
		this->model->t = 0.f;
		this->model->remaining = 0.f;
		this->linearTime = 0.0;

		this->model->fromIndex = this->model->targetPoseIndex;

		if (!attacking)
			this->model->targetPoseIndex = ((++this->model->targetPoseIndex) % this->walkAnimPoses);
		else
			this->model->targetPoseIndex = ((++this->model->targetPoseIndex) % this->model->nrOfPoses); //When attacking, use entire range of poses

		if (this->model->targetPoseIndex >= 5)
			this->model->animationSpeed = 12;
		else
			this->model->animationSpeed = 5;
		//std::cout << "Index: " << this->model->targetPoseIndex << std::endl;
		//this->model->decrementT = true;
	}
	else
		this->model->remaining = this->model->animationSpeed * deltaT / (1 - this->model->t);

}
