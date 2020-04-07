#pragma once
#include "DeferredRenderBuffer.h"

enum GBufferType { Position = 0, DiffuseColor = 1, Normal = 2 };

class GBufferHandler
{
private:
public:
	DeferredRenderBuffer buffers[3];
	void init(int width, int height);
};