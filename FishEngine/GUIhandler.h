#pragma once
#include "Mesh.h"
#include "DxHandler.h"
#include "InputHandler.h"
#include "GUIElement.h"
#include "GUIButton.h"
class GUIhandler
{
private :
	DxHandler* dxHandler;
	InputHandler* inputHandler = nullptr;

	GUIButton* startButton = nullptr;
	GUIButton* exitButton = nullptr;
	GUIButton* resumeButton = nullptr;
	GUIElement* healthBar = nullptr;
	GUIElement* title = nullptr;
	GUIElement* gameOver = nullptr;
	GUIElement* pointsFrame = nullptr;
	int heartCap = 5;
	int nrOfHearts = 5;
	std::vector<GUIElement> hearts;
public :
	int currentHealth = 5;
	std::vector<GUIElement*> guiElements;
	void drawGuiElements(Camera &camera);
	GUIhandler(DxHandler* dxHandler, InputHandler* inputHandler);
	void initMainMenu();
	void showMainMenu();
	void hideMainMenu();
	void showPauseMenu();
	void hidePauseMenu();
	void showHUD();
	void fixHUD();
	void hideHUD();
	void initHUD();
	void updateHUD(int health);
	void showGameOver();
	void hideGameOver();
	int checkButtons();

	void resetHealth(int healthPoints);
};