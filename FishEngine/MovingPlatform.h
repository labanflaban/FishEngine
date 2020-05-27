#pragma once
#include "Mesh.h"
#include "SimpleMath.h"

class MovingPlatform
{
private:
	int dirMult = 1;
public:
	Mesh* platform = nullptr;
	size_t range = 100;
	int speed = 1;
	int vectorIndex = 0;

	DirectX::SimpleMath::Vector3 startPos;

	void updatePlatform();
};