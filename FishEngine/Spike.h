#pragma once
#include "Mesh.h"
class Spike
{
private:

public:
	Mesh* model = nullptr;
	int vectorIndex = 0;

	float debounceLimit = 2;
	float debounce = debounceLimit;
};