#pragma once
#include "DxHandler.h"
class GUIElement
{
protected:
	DxHandler* dxHandler;
public:
	Mesh* mesh;
	GUIElement(DxHandler* dxHandler);
	virtual void draw(Camera& camera);
};