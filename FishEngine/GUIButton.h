#pragma once
#include "DxHandler.h"
#include "GUIElement.h"
#include "InputHandler.h"
class GUIButton : public GUIElement
{
private:
	InputHandler* inputHandler;
	bool isSelected(Mesh* guiElement);

public:
	void draw(Camera& camera);
	GUIButton(DxHandler* dxHandler, InputHandler* inputHandler);
	bool checkIfPressed();
};