#pragma once
#include "Mesh.h"
#include "DxHandler.h"
#include "InputHandler.h"
class GUIhandler
{
private :
	DxHandler* dxHandler;
	InputHandler* inputHandler;
public :
	std::vector<Mesh*> GuiElements;
	Mesh* generateGUIElement();
	void drawGuiElements(Camera &camera);
	GUIhandler(DxHandler* dxHandler);
	bool isSelected(Mesh* guiElement);
};