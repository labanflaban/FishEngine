#pragma once
#include "Mesh.h"

class Pointdrop
{
private:

public:
	Mesh* model = nullptr;
	bool usedUp = false;
	int vectorIndex = 0;
};