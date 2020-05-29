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

	this->title = new GUIElement(dxHandler);
	this->title->mesh->setScaling(DirectX::XMFLOAT3(0.4f, 0.1f, 0));
	this->title->mesh->setTranslation(DirectX::XMFLOAT3(0.0f, 0.5f, 0));
	this->title->mesh->readTextureFromFile(L"./Textures/GameTitle.png");
	this->title->active = true;
	guiElements.push_back(this->title);

	this->resumeButton = new GUIButton(dxHandler, inputHandler);
	this->resumeButton->mesh->setScaling(DirectX::XMFLOAT3(0.3f, 0.1f, 0));
	this->resumeButton->mesh->setTranslation(DirectX::XMFLOAT3(0.0f, 0.0f, 0));
	this->resumeButton->mesh->readTextureFromFile(L"./Textures/ButtonResume.png");
	this->resumeButton->active = false;
	guiElements.push_back(this->resumeButton);

	
}

void GUIhandler::showMainMenu()
{
	this->startButton->active = true;
	this->exitButton->active = true;
	this->title->active = true;
	this->gameOver->active = false;
}

void GUIhandler::hideMainMenu()
{
	this->startButton->active = false;
	this->exitButton->active = false;
	this->title->active = false;
}

void GUIhandler::showPauseMenu()
{
	this->resumeButton->active = true;
	this->exitButton->active = true;
}

void GUIhandler::hidePauseMenu()
{
	this->resumeButton->active = false;
	this->exitButton->active = false;
}

void GUIhandler::showHUD()
{
	for(int i = 0; i < heartCap; i++)
	{
		hearts.at(i).active = true;
	}
	healthBar->active = true;
	pointsFrame->active = true;
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

	gameOver->mesh->setTranslation(DirectX::XMFLOAT3(0.0f, 0.5f, 0));

	pointsFrame->mesh->setTranslation(DirectX::XMFLOAT3(1 - pointsFrame->mesh->getScaling().x, 0.9f, 0)); //1 - pointsFrame->mesh->getScaling().x
}

void GUIhandler::hideHUD()
{
	for (int i = 0; i < heartCap; i++)
	{
		hearts.at(i).active = false;
	}

	healthBar->active = false;
	pointsFrame->active = false;
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

	gameOver = new GUIElement(dxHandler);
	gameOver->mesh->setScaling(DirectX::XMFLOAT3(0.4f, 0.3f, 0));
	gameOver->mesh->readTextureFromFile(L"./Textures/GameOver.png");
	gameOver->active = false;

	this->pointsFrame = new GUIElement(dxHandler);
	this->pointsFrame->mesh->setScaling(DirectX::XMFLOAT3(0.15f, 0.05f, 0));
	this->pointsFrame->mesh->readTextureFromFile(L"./Textures/pointsbar.png");
	this->pointsFrame->active = true;
	guiElements.push_back(this->pointsFrame);

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
	
	this->healthBar->active = true;
}

void GUIhandler::showGameOver()
{
	this->gameOver->active = true;
}

void GUIhandler::hideGameOver()
{
	this->gameOver->active = false;
}

int GUIhandler::checkButtons()
{
	if(startButton->active && startButton->checkIfPressed() || resumeButton->active && resumeButton->checkIfPressed())
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

	if (pointsFrame->active)
		pointsFrame->draw(camera);

	if(healthBar->active)
	{
		healthBar->draw(camera);
	}

	if(gameOver->active)
	{
		gameOver->draw(camera);
	}
}