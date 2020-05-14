#include "GUIhandler.h"

GUIhandler::GUIhandler(DxHandler* dxHandler, InputHandler* inputHandler)
{
	this->dxHandler = dxHandler;
	this->inputHandler = inputHandler;
}

Mesh* GUIhandler::generateGUIElement()
{
	Mesh* tempMesh = new Mesh(this->dxHandler->devicePtr);	  //X  Y  Z   R	 G  B  A, U, V  nX nY nZ
	tempMesh->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
	tempMesh->vertices.push_back(Vertex{ 1, -1, 0.1f,    1, 1, 1, 1, 1, 1, 0, 0, -1 });
	tempMesh->vertices.push_back(Vertex{ -1,  -1, 0.1f,  1, 1, 1, 1, 0, 1, 0, 0, -1 });

	tempMesh->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
	tempMesh->vertices.push_back(Vertex{ 1,  1, 0.1f,   1, 1, 1, 1, 1, 0, 0, 0, -1 });
	tempMesh->vertices.push_back(Vertex{ 1,  -1, 0.1f, 1, 1, 1, 1, 1, 1, 0, 0, -1 });

	tempMesh->createVertexBuffer();

	this->GuiElements.push_back(tempMesh);
	return tempMesh;
}

void GUIhandler::drawGuiElements(Camera& camera)
{
	for(Mesh* m : this->GuiElements)
	{
		PS_CONSTANT_GUI_BUFFER guiBuff;
		if (isSelected(m))
		{
			guiBuff.selected = true;
		}
		else 
		{
			guiBuff.selected = false;
		}
		DxHandler::contextPtr->UpdateSubresource(DxHandler::guiBuffer, 0, NULL, &guiBuff, 0, 0);
		dxHandler->draw(m, camera, false, nullptr);
	}
}

bool GUIhandler::isSelected(Mesh* guiElement)
{
	float pixelPosX = ((guiElement->getTranslation().x + 1) / 2) * DxHandler::WIDTH;
	float pixelPosY = ((guiElement->getTranslation().y + 1) / 2) * DxHandler::HEIGHT;
	float sizeX = guiElement->getScaling().x * DxHandler::WIDTH;
	float sizeY = guiElement->getScaling().y * DxHandler::HEIGHT;

	if((inputHandler->getMousePosX() < pixelPosX + sizeX / 2 && inputHandler->getMousePosX() > pixelPosX - sizeX / 2) &&
	(DxHandler::HEIGHT-(inputHandler->getMousePosY()) < pixelPosY + sizeY / 2 && DxHandler::HEIGHT-(inputHandler->getMousePosY()) > pixelPosY - sizeY / 2))
	{
		return true;
	}
	else
	{
		return false;
	}
}
