#pragma once
#include "yse.hpp"
#include "Player.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "DXHandler.h"
#include "RenderWindow.h"
#include "InputHandler.h"
#include <chrono>
#include <ctime>
#include "VertexShader.h"
#include "ScoreHandler.h"
#include "PixelShader.h"
#include <string>
#include <vector>
#include "GBufferHandler.h"
#include "Camera.h"
#include <stdio.h>
#include "Skybox.h"
#include "Light.h"
#include <CommonStates.h>
#include "Tool.h"
#include "Enemy.h"
#include "Particle.h"
#include <random>

#include "ObjParser.h"
#include "Level.h"
#include "CollisionStruct.h"
#include "SceneManager.h"
#include "MovingPlatform.h"


#include "GUIhandler.h"

//#include <btBulletDynamicsCommon.h>
//#include <btBulletDynamics.h>

const int WIDTH = 1920;
const int HEIGHT = 1280;

class Light;

class Engine
{
private:
	InputHandler inputHandler;
	RenderWindow window;
	DxHandler* directXHandler = nullptr;
	HWND primaryWindow;
	double limitFPS = 1.0 / 60.0;
	ScoreHandler scoreHandle;
	std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	
	GBufferHandler deferredBufferHandler;

	Camera primaryCamera;


	int clientWidth = 0;
	int clientHeight = 0;

	GUIhandler* guiHandler;
	bool shutdown = false;
	bool pause = true;

	float timer = 0.0f;
public:
	float bubbleDebounceLimit = 5;
	float bubbleDebounce = bubbleDebounceLimit;

	Engine();
	~Engine();
	void initialSetup();
	void fixedUpdate(double deltaTime, btDiscreteDynamicsWorld* dynamicWorld); //deltaTime being time in (seconds) since last frame
	void updatePlayerMovement(double deltaTime);
	void updateParticles();
	void updateGUI();
	void resetEnemies();
	void resetDrops();

	void createWindow();
	void createDirectX();
	void createInputHandler();
	void engineLoop();
	void generateRandomBubbles();

	void createGUIHandler();

	YSE::sound GameSoundtrack;

	std::unique_ptr<DirectX::CommonStates> states;

	ID3D11RenderTargetView* nullRTV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	SceneManager sceneManager;

	Mesh* particleMesh = nullptr;
	std::chrono::high_resolution_clock::time_point startedGameTimer = std::chrono::high_resolution_clock::now();
	
	Player* player = nullptr;
	Tool* fishingRod = nullptr;
	Tool* hook = nullptr;
	Tool* rope = nullptr;
	bool pull = false;

	Level* level = nullptr;

	bool gameOver = true;

	void renderFirstPass(std::vector<Mesh*>* scene);
	void renderSecondPass();
	void renderGUI();
	void renderLightVolumes();
	void renderParticles();

	//void createLevel(btDiscreteDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*> collisionShapes);
};