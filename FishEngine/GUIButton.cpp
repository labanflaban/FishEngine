#include "GUIButton.h"
GUIButton::GUIButton(DxHandler* dxHandler, InputHandler* inputHandler)
	:GUIElement(dxHandler), inputHandler(inputHandler)
{

}

void GUIButton::draw(Camera& camera)
{
	PS_CONSTANT_GUI_BUFFER guiBuff;
	guiBuff.isButton = true;
	if (isSelected(this->mesh))
	{
		guiBuff.selected = true;
	}
	else
	{
		guiBuff.selected = false;
	}
	DxHandler::contextPtr->UpdateSubresource(DxHandler::guiBuffer, 0, NULL, &guiBuff, 0, 0);
	this->dxHandler->draw(mesh, camera, false, nullptr);
}

bool GUIButton::isSelected(Mesh* guiElement)
{
	float pixelPosX = ((guiElement->getTranslation().x + 1) / 2) * DxHandler::WIDTH;
	float pixelPosY = ((guiElement->getTranslation().y + 1) / 2) * DxHandler::HEIGHT;
	float sizeX = guiElement->getScaling().x * DxHandler::WIDTH;
	float sizeY = guiElement->getScaling().y * DxHandler::HEIGHT;

	if ((inputHandler->getMousePosX() < pixelPosX + sizeX / 2 && inputHandler->getMousePosX() > pixelPosX - sizeX / 2) &&
		(DxHandler::HEIGHT - (inputHandler->getMousePosY()) < pixelPosY + sizeY / 2 && DxHandler::HEIGHT - (inputHandler->getMousePosY()) > pixelPosY - sizeY / 2))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool GUIButton::checkIfPressed()
{
	if ((GetAsyncKeyState(0x01)) && isSelected(this->mesh))
	{
		return true;
	}
	else return false;
}
