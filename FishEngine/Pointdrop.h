#pragma once
#include "Mesh.h"

class Pointdrop
{
private:

public:
	Mesh* model = nullptr;
	bool usedUp = false;
	int vectorIndex = 0;

	DirectX::SimpleMath::Vector3 startPos;

	void moveAway();
	void resetDrop();
};