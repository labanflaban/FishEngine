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
	this->startButton->mesh->readTextureFromFile(L"./Textures/ButtonStart.png");
	this->startButton->active = true;
	guiElements.push_back(this->startButton);

	this->exitButton = new GUIButton(dxHandler, inputHandler);
	this->exitButton->mesh->setScaling(DirectX::XMFLOAT3(0.3f, 0.1, 0));
	this->exitButton->mesh->setTranslation(DirectX::XMFLOAT3(0, -0.3f, 0));
	this->exitButton->mesh->readTextureFromFile(L"./Textures/ButtonQuit.png");
	this->exitButton->active = true;
	guiElements.push_back(this->exitButton);
}

void GUIhandler::showMainMenu()
{
	this->startButton->active = true;
	this->exitButton->active = true;
}

void GUIhandler::hideMainMenu()
{
	this->startButton->active = false;
	this->exitButton->active = false;
}

void GUIhandler::showHUD()
{
	for(int i = 0; i < heartCap; i++)
	{
		hearts.at(i).active = true;
	}
}

void GUIhandler::fixHUD()
{
	float offset = 0.0f;
	for(int i = 0; i < heartCap; i++)
	{
		hearts.at(i).mesh->setTranslation(DirectX::XMFLOAT3(-0.95f + offset , 0.9f, 0));
		offset += 0.1f;
	}

	healthBar->mesh->setTranslation(DirectX::XMFLOAT3(-0.75f, 0.9f, 1));
}

void GUIhandler::hideHUD()
{
	for (int i = 0; i < heartCap; i++)
	{
		hearts.at(i).active = false;
	}
}

void GUIhandler::initHUD()
{
	GUIElement* heart;

	for(int i = 0; i < heartCap; i++)
	{
		heart = new GUIElement(dxHandler);
		heart->mesh->setScaling(DirectX::XMFLOAT3(0.05f, 0.05f, 0));
		heart->mesh->readTextureFromFile(L"./Textures/HEART.png");
		heart->active = true;
		hearts.push_back(*heart);
	}
	healthBar = new GUIElement(dxHandler);
	healthBar->mesh->setScaling(DirectX::XMFLOAT3(0.25f, 0.05f, 0));
	healthBar->mesh->readTextureFromFile(L"./Textures/healthBar.png");
	healthBar->active = true;

	fixHUD();
}

void GUIhandler::updateHUD(int health)
{
	for (int i = 0; i < hearts.size(); i++)
	{
		hearts.at(i).active = false;
	}

	for (int i = 0; i < health; i++)
	{
		hearts.at(i).active = true;
	}
	
}


int GUIhandler::checkButtons()
{
	if(startButton->active && startButton->checkIfPressed())
	{
		return 1;
	}
	else if(exitButton->active && exitButton->checkIfPressed())
	{
		return 2;
	}
}

void GUIhandler::resetHealth(int healthPoints)
{
	this->nrOfHearts = healthPoints;
	for (int i = 0; i < healthPoints; i++)
	{
		hearts.at(i).active = true;
	}
}

void GUIhandler::drawGuiElements(Camera& camera)
{
	for(int i = 0; i < guiElements.size(); i++)
	{
		if(guiElements.at(i)->active)
		{
			guiElements.at(i)->draw(camera);
		}
	}

	for (int i = 0; i < hearts.size(); i++)
	{
		if (hearts.at(i).active)
		{
			hearts.at(i).draw(camera);
			//std::cout << hearts.at(i).active << std::endl;
		}
	}

	healthBar->draw(camera);
}