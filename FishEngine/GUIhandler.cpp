#include "GUIhandler.h"

GUIhandler::GUIhandler(DxHandler* dxHandler, InputHandler* inputHandler)
{
	this->dxHandler = dxHandler;
	this->inputHandler = inputHandler;
}

void GUIhandler::initMainMenu()
{
	this->startButton = new GUIButton(dxHandler, inputHandler);
	this->startButton->mesh->setScaling(DirectX::XMFLOAT3(0.3f, 0.1f, 0));
	this->startButton->mesh->setTranslation(DirectX::XMFLOAT3(0, 0, 0));
	this->startButton->mesh->readTextureFromFile(L"./Textures/ButtonCombo.png");
	guiElements.push_back(this->startButton);

	this->exitButton = new GUIButton(dxHandler, inputHandler);
	this->exitButton->mesh->setScaling(DirectX::XMFLOAT3(0.3f, 0.1, 0));
	this->exitButton->mesh->setTranslation(DirectX::XMFLOAT3(0, -0.3f, 0));
	this->exitButton->mesh->readTextureFromFile(L"./Textures/ButtonCombo.png");
	guiElements.push_back(this->exitButton);
}

void GUIhandler::initHUD()
{
	GUIElement* element = new GUIElement(dxHandler);
	element->mesh->setScaling(DirectX::XMFLOAT3(0.05f, 0.05f, 0));
	element->mesh->setTranslation(DirectX::XMFLOAT3(-0.95f, 0.9f, 0));
	element->mesh->readTextureFromFile(L"./Textures/HEART.png");
	guiElements.push_back(element);

	GUIElement* element1 = new GUIElement(dxHandler);
	element1->mesh->setScaling(DirectX::XMFLOAT3(0.05f, 0.05f, 0));
	element1->mesh->setTranslation(DirectX::XMFLOAT3(-0.85f, 0.9f, 0));
	element1->mesh->readTextureFromFile(L"./Textures/HEART.png");
	guiElements.push_back(element1);

	GUIElement* element2 = new GUIElement(dxHandler);
	element2->mesh->setScaling(DirectX::XMFLOAT3(0.05f, 0.05f, 0));
	element2->mesh->setTranslation(DirectX::XMFLOAT3(-0.75f, 0.9f, 0));
	element2->mesh->readTextureFromFile(L"./Textures/HEART.png");
	guiElements.push_back(element2);
}

int GUIhandler::checkButtons()
{
	if(startButton->checkIfPressed())
	{
		return 1;
	}
	else if(exitButton->checkIfPressed())
	{
		return 2;
	}
}

void GUIhandler::drawGuiElements(Camera& camera)
{
	for(GUIElement* e : this->guiElements)
	{
		e->draw(camera);
	}
}