#pragma once
#include "Mesh.h"
#include "DxHandler.h"
#include "InputHandler.h"
class GUIhandler
{
private :
	DxHandler* dxHandler;
	InputHandler* inputHandler = nullptr;
public :
	std::vector<Mesh*> GuiElements;
	Mesh* generateGUIElement();
	void drawGuiElements(Camera &camera);
	GUIhandler(DxHandler* dxHandler, InputHandler* inputHandler);
	bool isSelected(Mesh* guiElement);
};
//class GUIElement
//{
//
//};
//
//class GUIButton : public GUIElement
//{
//
//};