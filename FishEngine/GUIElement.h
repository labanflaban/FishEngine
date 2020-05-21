#pragma once
#include "DxHandler.h"
class GUIElement
{
protected:
	DxHandler* dxHandler;
public:
	Mesh* mesh;
	GUIElement(DxHandler* dxHandler);
	GUIElement();
	void setDxHandler(DxHandler* dxHandler);
	bool active = false;
	virtual void draw(Camera& camera);
};