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
	DxHandler* directXHandler;
	HWND primaryWindow;
	double limitFPS = 1.0 / 60.0;
	
	std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	
	GBufferHandler deferredBufferHandler;

	Camera primaryCamera;

	int clientWidth = 0;
	int clientHeight = 0;
	
public:
	Engine();
	~Engine();
	void initialSetup();
	void fixedUpdate(double deltaTime); //deltaTime being time in (seconds) since last frame
	void updatePlayerMovement(double deltaTime);
	void updatePlayerTools(double deltaTime);
	void updateParticles();

	void createWindow();
	void createDirectX();
	void createInputHandler();
	void engineLoop();

	std::unique_ptr<DirectX::CommonStates> states;

	ID3D11RenderTargetView* nullRTV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	//Mesh* playerMesh = nullptr;
	
	Player* player = nullptr;
	Tool* fishingRod = nullptr;
	Tool* hook = nullptr;
	Tool* rope = nullptr;
	bool pull = false;

	void renderFirstPass(std::vector<Mesh*>* scene);
	void renderSecondPass();
	void renderLightVolumes();
	void renderParticles();

	
	std::vector<Mesh*> scene;
	std::vector<AnimatedMesh*> animatedMeshes;
	std::vector<Light*> lights;
	std::vector<Mesh*> transparentSceneObjects;
	std::vector<Enemy*> enemies;
	std::vector<Particle*> particles;

	//void createLevel(btDiscreteDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*> collisionShapes);
};