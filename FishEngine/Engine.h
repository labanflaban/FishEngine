#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "DXHandler.h"
#include "RenderWindow.h"
#include "InputHandler.h"
#include <chrono>
#include <ctime>
#include "VertexShader.h"
#include "PixelShader.h"
#include <string>
#include <vector>
#include "GBufferHandler.h"
#include "Camera.h"

//#include <btBulletDynamicsCommon.h>
//#include <btBulletDynamics.h>
#include "bullet\btBulletDynamicsCommon.h"

const int WIDTH = 1920;
const int HEIGHT = 1280;

class Engine
{
private:
	InputHandler inputHandler;
	RenderWindow window;
	DxHandler* directXHandler;
	HWND primaryWindow;

	std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	
	GBufferHandler deferredBufferHandler;

	Camera primaryCamera;


	
public:
	Engine();
	~Engine();
	void initialSetup();
	void fixedUpdate(double deltaTime); //deltaTime being time in (seconds) since last frame

	void createWindow();
	void createDirectX();
	void createInputHandler();
	void engineLoop();

	void renderFirstPass(std::vector<Mesh>* scene);
	void renderSecondPass();
	std::vector<Mesh> scene;
};