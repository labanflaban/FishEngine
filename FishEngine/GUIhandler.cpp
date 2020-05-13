#include "GUIhandler.h"

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

GUIhandler::GUIhandler(DxHandler* dxHandler)
{
	this->dxHandler = dxHandler;
}

bool GUIhandler::isSelected(Mesh* guiElement)
{
	float pixelPosX = ((guiElement->getTranslation().x + 1) / 2) * DxHandler::WIDTH;
	float pixelPosY = ((guiElement->getTranslation().y + 1) / 2) * DxHandler::HEIGHT;
	float sizeX = guiElement->getScaling().x;
	float sizeY = guiElement->getScaling().y;


	//if(InputHandler::lastMousePos.x < sizeX * pixelPosX && InputHandler::lastMousePos.y < sizeY * pixelPosY)
	//{
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}
	return false;
}
