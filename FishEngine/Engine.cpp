#include "Engine.h"
#include "SimpleMath.h"
#include <Windows.h>
using namespace DirectX;

Engine::Engine()
{
}

Engine::~Engine()
{
	delete DxHandler::firstPassPixel;
	delete DxHandler::secondPassPixel;

	delete DxHandler::firstPassVertex;
	delete DxHandler::secondPassVertex;

	lights.clear();

	YSE::System().close();
}

void Engine::initialSetup()
{

	//DxHandler::WIDTH = WIDTH;
	//DxHandler::HEIGHT = HEIGHT;

	YSE::System().init();

	this->createWindow();
	createDirectX();
	DxHandler::firstPassPixel->compileShader(L"./FirstPassPixel.hlsl", DxHandler::devicePtr);
	DxHandler::firstPassVertex->compileShader(L"./FirstPassVertex.hlsl", DxHandler::devicePtr);

	DxHandler::skyboxPixel->compileShader(L"./SkyboxPixel.hlsl", DxHandler::devicePtr);
	DxHandler::skyboxVertex->compileShader(L"./SkyboxVertex.hlsl", DxHandler::devicePtr);

	DxHandler::secondPassPixel->compileShader(L"./SecondPassPixel.hlsl", DxHandler::devicePtr);
	DxHandler::secondPassVertex->compileShader(L"./SecondPassVertex.hlsl", DxHandler::devicePtr);

	DxHandler::lightPixel->compileShader(L"./LightVolumeShader.hlsl", DxHandler::devicePtr);

	DxHandler::transparencyPixel->compileShader(L"./ForwardTransparencyPixel.hlsl", DxHandler::devicePtr);
	DxHandler::transparencyVertex->compileShader(L"./ForwardTransparencyVertex.hlsl", DxHandler::devicePtr);

	DxHandler::animVertex->compileShader(L"./AnimatedVertexShader.hlsl", DxHandler::devicePtr);

	DxHandler::backfaceCullShader->compileShader(L"./BackfaceCullerGeometry.hlsl", DxHandler::devicePtr);

	DxHandler::particlePixel->compileShader(L"./ForwardParticlePixel.hlsl", DxHandler::devicePtr);

	//DxHandler::skyboxVertexShader->compileShader(L"./SkyboxVertex", DxHandler::devicePtr);
	//DxHandler::skyboxPixelShader->compileShader(L"./SkyboxPixel.hlsl", DxHandler::devicePtr);
	deferredBufferHandler.init(clientWidth, clientHeight);

	createInputHandler();

	directXHandler->setupInputLayout();
	directXHandler->setupDepthBuffer();
	DxHandler::firstPassPixel->useThis(DxHandler::contextPtr);
	DxHandler::firstPassVertex->useThis(DxHandler::contextPtr);

	DxHandler::contextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DxHandler::contextPtr->IASetInputLayout((ID3D11InputLayout*)DxHandler::input_layout_ptr);

	directXHandler->generateFullScreenQuad();

	PS_CONSTANT_LIGHT_BUFFER ps_buff;
	VS_CONSTANT_MATRIX_BUFFER vs_buff;
	VS_CONSTANT_ANIM_BUFFER vs_anim_buff;
	directXHandler->createPSConstBuffer(ps_buff);
	directXHandler->createVSConstBuffer(vs_buff);
	directXHandler->createVSAnimBuffer(vs_anim_buff);
	directXHandler->createGSConstBuffer();

	directXHandler->initAdditiveBlendState();
	
	this->primaryCamera = Camera(WIDTH, HEIGHT);

	states = std::make_unique<DirectX::CommonStates>(DxHandler::devicePtr);
	DxHandler::alphaBlendState = states->AlphaBlend();

	D3D11_SAMPLER_DESC sampleDesc = {};
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT sampleSucc = DxHandler::devicePtr->CreateSamplerState(&sampleDesc, &DxHandler::standardSampler);
	assert(SUCCEEDED(sampleSucc));

	DxHandler::contextPtr->PSSetSamplers(0, 1, &DxHandler::standardSampler);

	//DxHandler::standardSampler = states->LinearWrap();

	for (int i = 0; i < 50; i++)
	{
		Particle* particlePtr = new Particle(DxHandler::devicePtr);
		particlePtr->readTextureFromFile(L"./Textures/bubble.png");
		particlePtr->setScaling(DirectX::XMFLOAT3(2, 2, 2));
		particlePtr->setTranslation(DirectX::XMFLOAT3(-30, 25, 0));
		particles.push_back(particlePtr);
	}
}

void Engine::fixedUpdate(double deltaTime) //time in seconds since last frame
{
	//game logic here thanks
	
	updatePlayerMovement(deltaTime);
	player->updatePlayerTools(fishingRod,hook,rope, deltaTime);
	if (fishingRod->pullback < 10.f)
	fishingRod->pullback += 0.1f;

	if (hook->ableToThrowHook < 10.f)
		hook->ableToThrowHook += 0.1f;
	
	for(int i = 0; i < sceneManager.enemies.size(); i++)
	{
		if (sceneManager.enemies.at(i)->damageDebounce < sceneManager.enemies.at(i)->maxDebounce) //L‰gg 3 i enemy i en variabel
			sceneManager.enemies.at(i)->damageDebounce += 0.1;
	}
	
	if (hook->ropeZipBack < 10.f)
		hook->ropeZipBack += 0.1f;

	if (player->boostReserve < 10.f)
		player->boostReserve += 0.1f;
}

void Engine::createWindow()
{
	this->window = RenderWindow();
	this->primaryWindow = this->window.createWindow(WIDTH, HEIGHT, "Fish Engine", "Title");
}

void Engine::createDirectX()
{
	this->directXHandler = new DxHandler(primaryWindow);
	RECT rect;

	GetClientRect(primaryWindow, &rect);
	int myWidth = rect.right - rect.left;
	int myHeight = rect.bottom - rect.top;

	std::cout << "WINDOW SIZE: " << myWidth << " " << myHeight << std::endl;

	clientWidth = myWidth;
	clientHeight = myHeight;

	DxHandler::WIDTH = myWidth;
	DxHandler::HEIGHT = myHeight;
}

void Engine::createInputHandler()
{
	this->inputHandler = InputHandler(primaryWindow, &primaryCamera);
}

void Engine::updatePlayerMovement(double deltaTime)
{
	
	player->model->rigidBody->setActivationState(ACTIVE_TAG);
	fishingRod->model->rigidBody->setActivationState(ACTIVE_TAG);

	btVector3 movementVector(0,0,0);
	if (inputHandler.isKeyDown(VK_SPACE) && player->boostReserve >= 10.f)
	{

		//std::cout << "Space" << std::endl;
		movementVector += btVector3(0, 10.f, 0);

		movementVector += btVector3(0, 5.f, 0);

		player->boostReserve -= 10.f;
		player->jumpSound.play();

		//for (Particle* p : particles)
		std::cout << "JUMP YEEET" << std::endl;

		for(int i = 0; i < particles.size(); i++)
		{
			std::random_device randomSeed;
			std::mt19937 numberGenerator(randomSeed());
			std::uniform_real_distribution<> randomNum(-1.f, 1.f); //Between -1 - 1
			std::uniform_real_distribution<> randomNumPlus(0.8f, 1.f); //Between 0.8 - 1

			particles.at(i)->setTranslation(player->model->getTranslation());
			float rNum = randomNum(numberGenerator);
			float rNumY = randomNumPlus(numberGenerator);
			particles.at(i)->velocity = DirectX::XMFLOAT3(rNum * 0.5, 0.5*rNumY, 0);
		}
	}
	if (GetAsyncKeyState(0x41)) //A-key
	{

		//std::cout << GetAsyncKeyState(0x41) << std::endl;
		//std::cout << "A" << std::endl;
		movementVector += btVector3(-10 * deltaTime * 40, 0, 0);
	}
	if(GetAsyncKeyState(0x44)) // D-key
	{
		//std::cout << "D" << std::endl;
		movementVector += btVector3(10 * deltaTime * 40, 0, 0);

		movementVector += btVector3(-30 * deltaTime * 40, 0, 0);
	}
	if(GetAsyncKeyState(0x44)) // D-key
	{
		movementVector += btVector3(30 * deltaTime * 40, 0, 0);

	}
	player->updatePlayer(fishingRod,hook, rope);

	btVector3 orgVel = player->model->rigidBody->getLinearVelocity();
	player->model->rigidBody->setLinearVelocity(btVector3(0, orgVel.y(), 0) + movementVector);
	if (!movementVector.isZero())
		player->model->stepAnim(deltaTime);

	XMFLOAT3 playerPos = player->model->getTranslation();
	primaryCamera.cameraPosition = XMVectorSet(playerPos.x, playerPos.y+30, playerPos.z - 150, 0);
	primaryCamera.cameraTarget = XMVectorSet(playerPos.x, playerPos.y, playerPos.z, 0);

	Skybox::sphereModel->setTranslation(DirectX::XMFLOAT3(DirectX::XMVectorGetX(primaryCamera.cameraPosition), DirectX::XMVectorGetY(primaryCamera.cameraPosition), DirectX::XMVectorGetZ(primaryCamera.cameraPosition)));
	player->playerLight->setPosition(XMFLOAT3(playerPos.x, playerPos.y + 25, playerPos.z));

	
	
}

	

void Engine::updateParticles()
{
	for (Particle* p : particles)
	{
		p->updateParticle();
	}
}

collisionStruct* getCollStruct(void* ptr)
{
	if (ptr != nullptr)
		return static_cast<collisionStruct*>(ptr);
}

bool isValidStruct(void* ptr)
{
	bool retVal = false;
	if (ptr)
	{
		collisionStruct* collStruct = static_cast<collisionStruct*>(ptr);
		if (collStruct != nullptr)
		{
			if (collStruct->type != collisionEnums::Ignored)
				return true;
		}
	}
}

void myTickCallback(btDynamicsWorld* myWorld, btScalar timeStep) {
	int manifoldCount = myWorld->getDispatcher()->getNumManifolds(); //Manifolds cache all contact points between objects
	//printf("numManifolds = %d\n", manifoldCount);

	for (int i = 0; i < manifoldCount; i++)
	{
		btPersistentManifold* manif = myWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* colliderA = const_cast<btCollisionObject*>(manif->getBody0());
		btCollisionObject* colliderB = const_cast<btCollisionObject*>(manif->getBody1());
		//btCollisionObject* colliderA = static_cast<btCollisionObject*>(manif->getBody0());
		//btCollisionObject* colliderB = static_cast<btCollisionObject*>(manif->getBody1());

		collisionStruct* collision = nullptr;
		collisionStruct* collision1 = nullptr;

		if (isValidStruct(colliderA->getUserPointer()) && isValidStruct(colliderB->getUserPointer())) //If both have an enum that is not ignore, ie is a rod, hook or enemy
		{
			collision = getCollStruct(manif->getBody0()->getUserPointer());
			collision1 = getCollStruct(manif->getBody1()->getUserPointer());

			//collision->type = collisionEnums::Hook;

			if (((collision->type == collisionEnums::Hook) && (collision1->type == collisionEnums::Enemy)) || ((collision1->type == collisionEnums::Hook) && (collision->type == collisionEnums::Enemy)))
			{
				//H‰mta hooken h‰r, kolla om den ‰r active, kolla enum pÂ bÂda collision och se vilken som har enum hook och sedan h‰mta d‰r
				Tool* hook = nullptr;
				if(collision->type == collisionEnums::Hook)
				{
					hook = collision->hook;
				}
				else
				{
					hook = collision1->hook;
				}

				Enemy* myEnemy = nullptr; //S‰tt denna i if-checken
				if(collision->type == collisionEnums::Enemy)
				{
					myEnemy = collision->enemy;
				}
				else
				{
					myEnemy = collision1->enemy;
				}

				
				if(myEnemy->damageDebounce >= myEnemy->maxDebounce)
				{
					myEnemy->health -= 10;
					myEnemy->damageDebounce = 0;
					//std::cout << "Enemy hit" << std::endl;


				}
				//std::cout << "Enemy Health: " << myEnemy->health << "\nDebounce: " << myEnemy->damageDebounce << std::endl;
			
				

				//std::cout << "Hook hit enemy" << std::endl;
			}

			if (((collision->type == collisionEnums::Rod) && (collision1->type == collisionEnums::Enemy)) || ((collision1->type == collisionEnums::Rod) && (collision->type == collisionEnums::Enemy)))
			{
				Tool* rod = nullptr;
				if(collision->type == collisionEnums::Rod)
				{
					rod = collision->rod;
				}
				else
				{
					rod = collision1->rod;
				}

				Enemy* myEnemy = nullptr;
				if(collision->type == collisionEnums::Enemy)
				{
					myEnemy = collision->enemy;
				}
				else
				{
					myEnemy = collision1->enemy;
				}

				if (myEnemy->damageDebounce >= myEnemy->maxDebounce)
				{
					myEnemy->health -= 30;
					myEnemy->damageDebounce = 0;
					//std::cout << "Enemy hit" << std::endl;
				}

				if (myEnemy->health < 0)
					std::cout << "Should be dead" << std::endl;

				//std::cout << "Enemy Health: " << myEnemy->health << "\nDebounce: " << myEnemy->damageDebounce << std::endl;
			}
		}

	
			
	}
}

void Engine::engineLoop()
{
	//--------------------------------------------------------------------------------------------------- physics 
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	//btCollisionWorld* collisionWorld = new btCollisionWorld(dispatcher, overlappingPairCache, collisionConfiguration);
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	dynamicsWorld->setInternalTickCallback(myTickCallback);
	dynamicsWorld->setGravity(btVector3(0, -1.5, 0));
	//--------------------------------------------------------------------------------------------------- physics 

	RECT viewportRect;
	GetClientRect(primaryWindow, &viewportRect);
	D3D11_VIEWPORT port = {
		0.f,
		0.f,
		(float)(viewportRect.right - viewportRect.left),
		(float)(viewportRect.bottom - viewportRect.top),0.f,1.f
	};
	port.MinDepth = 0.0f; //Closest possible to screen Z depth
	port.MaxDepth = 1.0f; //Furthest possible
	DxHandler::contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL);
	//--------------------------------------------------------------------------// 
	AnimatedMesh* debugObject = new AnimatedMesh(DxHandler::devicePtr); //Body
	//debugObject->readMeshFromFile("./Models/character.obj");
	std::vector<Vertex> vertVectorPlr = FIDParser::readFromFID("./Models/Fish_Right.FID");
	std::vector<Vertex> vertVectorPlr1 = FIDParser::readFromFID("./Models/Fish_Left.FID");
	std::vector<Vertex>* plrArr[] = { &vertVectorPlr, &vertVectorPlr1 };
	debugObject->appendStructuredBuffer(plrArr, 2);
	debugObject->createStructuredBuffer(DxHandler::devicePtr);
	debugObject->setTranslation(DirectX::XMFLOAT3(3, 0, 4));
	debugObject->setScaling(DirectX::XMFLOAT3(4, 4, 4));
	debugObject->setRotation(DirectX::XMFLOAT3(0,  3.14, 3.14/2));
	debugObject->manualUpdate = true;
	debugObject->readTextureFromFile(L"./Models/FISHCOLOR.png");
	this->player = new Player(&inputHandler);
	this->player->model = debugObject;
	debugObject->initRigidbody(dynamicsWorld, &collisionShapes, 10);
	this->sceneManager.addAnimatedMesh(debugObject);
	debugObject->targetPoseIndex = 1;

	player->playerLight = new Light(DxHandler::devicePtr);
	player->playerLight->lightColor = XMVectorSet(1.f, 249 / 255.f, 10 / 255.f, 0.f);
	//lights.push_back(player->playerLight);
	sceneManager.addLight(player->playerLight);

	//debugObject->setRotation(DirectX::XMFLOAT3(0, 3.14 / 2, 0));

	//this->sceneManager.addMesh(debugObject);

	Mesh* hookObject = new Mesh(DxHandler::devicePtr); //Hook
	hookObject->readMeshFromFile("./Models/actualCube.obj");
	hookObject->setScaling(DirectX::XMFLOAT3(1, 1, 1));
	hookObject->setRotation(DirectX::XMFLOAT3(0, 0, 0.5));
	this->hook = new Tool(&inputHandler);
	this->hook->model = hookObject;
	collisionStruct* hookCollStruct = new collisionStruct(hook, collisionEnums::Hook);
	hookCollStruct->hook = hook;
	hookObject->initRigidbody(dynamicsWorld, &collisionShapes, 5);
	hookObject->rigidBody->setUserPointer(hookCollStruct);
	this->sceneManager.addMesh(hookObject);

	Mesh* rope = new Mesh(DxHandler::devicePtr); //Rope
	rope->readMeshFromFile("./Models/actualCube.obj");
	rope->setRotation(DirectX::XMFLOAT3(0, 0, 8));
	this->rope = new Tool(&inputHandler);
	this->rope->model = rope;
	rope->initRigidbody(dynamicsWorld, &collisionShapes, 5);
	this->sceneManager.addMesh(rope);
	
	Enemy* enemy = new Enemy(DxHandler::devicePtr);
	//this->sceneManager.enemies.push_back(enemy);
	this->sceneManager.addEnemy(enemy);

	this->sceneManager.addMesh(enemy->model);
	sceneManager.addLight(enemy->light);
	//this->lights.push_back(enemy->light);
	enemy->model->setTranslation(XMFLOAT3(30, 20, 0));
	collisionStruct* enemy1CollStruct = new collisionStruct(enemy, collisionEnums::Enemy);
	
	enemy->model->initRigidbody(dynamicsWorld, &collisionShapes, 2);
	enemy->model->rigidBody->setUserPointer(enemy1CollStruct);
	enemy->model->rigidBody->setActivationState(ACTIVE_TAG);
	enemy->model->rigidBody->setGravity(btVector3(0, 0, 0));

	Mesh* fishingRodObject = new Mesh(DxHandler::devicePtr); // fishing rod
	fishingRodObject->readMeshFromFile("./Models/rod.obj");
	fishingRodObject->setScaling(DirectX::XMFLOAT3(1, 1, 1));
	this->fishingRod = new Tool(&inputHandler);
	this->fishingRod->model = fishingRodObject;
	collisionStruct* fishingRodCollStruct = new collisionStruct(fishingRod, collisionEnums::Rod);
	fishingRodObject->initRigidbody(dynamicsWorld, &collisionShapes, 1);
	fishingRodObject->rigidBody->setUserPointer(fishingRodCollStruct);
	this->sceneManager.addMesh(fishingRodObject);
	

	Mesh* groundObject4 = new Mesh(DxHandler::devicePtr); //Ground
	groundObject4->readMeshFromFile("./Models/JellyFishObj.obj");
	//groundObject4->readNormalMapFromFile(L"./Models/TegelNormMap.png");
	groundObject4->setTranslation(DirectX::XMFLOAT3(130, 15, 25));
	groundObject4->setScaling(DirectX::XMFLOAT3(10, 10, 10));
	//groundObject4->initRigidbody(dynamicsWorld, &collisionShapes, 0);
	sceneManager.addTransparentObject(groundObject4);

	Mesh* groundObject5= new Mesh(DxHandler::devicePtr); //Ground
	groundObject5->readMeshFromFile("./Models/JellyFishObj.obj");
	//groundObject5->readNormalMapFromFile(L"./Models/TegelNormMap.png");
	groundObject5->setTranslation(DirectX::XMFLOAT3(130, 15, 75));
	groundObject5->setScaling(DirectX::XMFLOAT3(60, 60, 60));
	//groundObject4->initRigidbody(dynamicsWorld, &collisionShapes, 0);
	sceneManager.addTransparentObject(groundObject5);

	//Mesh* groundObject8 = new Mesh(DxHandler::devicePtr); //Ground
	//groundObject8->readMeshFromFile("./Models/actualCube.obj");
	//groundObject8->readNormalMapFromFile(L"./Models/TegelNormMap.png");
	//groundObject8->setTranslation(DirectX::XMFLOAT3(250, -50, 4));
	//groundObject8->setScaling(DirectX::XMFLOAT3(500, 10, 10));
	//groundObject8->initRigidbody(dynamicsWorld, &collisionShapes, 0);
	//this->sceneManager.addMesh(groundObject8);

	Skybox::loadSkybox(DxHandler::devicePtr);
	Skybox::sphereModel->setTranslation(XMFLOAT3(1, 50, 4));
	Skybox::sphereModel->setScaling(XMFLOAT3(3000, 3000, 3000));
	Skybox::sphereModel->isSky = true;
	//sceneManager.addMesh(Skybox::sphereModel);

	Light* light = new Light(DxHandler::devicePtr);
	light->setPosition(XMFLOAT3(0, 0, -10));
	//this->lights.push_back(light);
	sceneManager.addLight(light);

	Level* level = new Level();
	level->createLevel(dynamicsWorld, collisionShapes, &sceneManager);


	//--------------------------------------------------------------------------------------------------- 
	std::chrono::high_resolution_clock::time_point newTime = std::chrono::high_resolution_clock::now(); //Set new time
	std::chrono::duration<double> frameTime = std::chrono::duration_cast<std::chrono::duration<double>>(newTime - currentTime); //Get deltaTime for frame

	MSG msg;
	bool shutdown = false;

	//std::vector<Vertex> vertVector = ObjParser::readFromObj("./Models/actualCube.obj");
	//std::vector<Vertex> vertVector2 = ObjParser::readFromObj("./Models/targetCube.obj");
	
	std::vector<Vertex> vertVector = FIDParser::readFromFID("./Models/Fish_Right.FID");
	std::vector<Vertex> vertVector2 = FIDParser::readFromFID("./Models/Fish_Left.FID");

	AnimatedMesh* animMesh = new AnimatedMesh(DxHandler::devicePtr);
	animMesh->readTextureFromFile(L"./Models/FISHCOLOR.png");

	std::vector<Vertex>* arr[] = { &vertVector, &vertVector2 };
	animMesh->appendStructuredBuffer(arr, 2);
	animMesh->createStructuredBuffer(DxHandler::devicePtr);
	animMesh->setTranslation(XMFLOAT3(-50, -20, 0));
	animMesh->setScaling(XMFLOAT3(10, 10, 10));
	animMesh->setRotation(XMFLOAT3(0, 0, 3.14 / 2));
	animMesh->targetPoseIndex = 1;
	//animatedMeshes.push_back(animMesh);
	sceneManager.addAnimatedMesh(animMesh);
	
	while (!shutdown)
	{
		directXHandler->contextPtr->RSSetViewports(1, &port);
		inputHandler.handleInput();
		currentTime = std::chrono::high_resolution_clock::now(); //Reset time - always needs to be at top / limitFstd::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastTime) / limitFPS;
		primaryCamera.updateCamera();
		renderFirstPass(&sceneManager.sceneMeshes);
		

		renderSecondPass();
		renderLightVolumes();
		renderParticles();

		//upp upp och iv‰‰‰‰g
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_DESTROY)
		{
			PostQuitMessage(0);
		}

		///-----stepsimulation_start-----
		for (int i = 0; i < 15; i++)
		{
			dynamicsWorld->stepSimulation(1/60.f);
			//std::cout << frameTime.count() << std::endl;
			//dynamicsWorld->stepSimulation(1/60, 1);

			//print positions of all objects
			for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
			{
				//std::cout << "Looping" << j << std::endl;
				btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
				btRigidBody* body = btRigidBody::upcast(obj);

				btTransform trans;
				if (body && body->getMotionState())
				{
					body->getMotionState()->getWorldTransform(trans);
					//printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
				}
				else
				{
					trans = obj->getWorldTransform();
				}
				//printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));

				//if (j == 1)
				//sceneManager.sceneMeshes.at(0).setTranslation(DirectX::XMFLOAT3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()))); //Set visual position to the same
				//primaryCamera.cameraPosition = DirectX::XMVectorSet(DirectX::XMVectorGetX(primaryCamera.cameraPosition), float(trans.getOrigin().getY()), DirectX::XMVectorGetZ(primaryCamera.cameraPosition), 0);
				//primaryCamera.cameraTarget = DirectX::XMVectorSet(DirectX::XMVectorGetX(primaryCamera.cameraPosition), float(trans.getOrigin().getY()), 1, 0);
				//primaryCamera.updateCamera();
			}
		}

		//XMFLOAT3 debugpos = DirectX::XMFLOAT3(sceneManager.sceneMeshes.at(0)->rigidBody->getWorldTransform().getOrigin().x(), sceneManager.sceneMeshes.at(0)->rigidBody->getWorldTransform().getOrigin().getY(), sceneManager.sceneMeshes.at(0)->rigidBody->getWorldTransform().getOrigin().z());
		//std::cout << debugpos.x << " " << debugpos.y << " " << debugpos.z << " " << std::endl;
		for (int i = 0; i < sceneManager.sceneMeshes.size(); i++) //Update according to rigid body
		{
			bool check = sceneManager.sceneMeshes.at(i)->rigidBody != nullptr;

			btTransform transform;
			if (sceneManager.sceneMeshes.at(i)->rigidBody != nullptr && sceneManager.sceneMeshes.at(i)->rigidBody->getMotionState())
			{
				sceneManager.sceneMeshes.at(i)->setTranslation(DirectX::XMFLOAT3(sceneManager.sceneMeshes.at(i)->rigidBody->getWorldTransform().getOrigin().x(), sceneManager.sceneMeshes.at(i)->rigidBody->getWorldTransform().getOrigin().getY(), sceneManager.sceneMeshes.at(i)->rigidBody->getWorldTransform().getOrigin().z()));
				XMFLOAT3 pos = DirectX::XMFLOAT3(sceneManager.sceneMeshes.at(i)->rigidBody->getWorldTransform().getOrigin().x(), sceneManager.sceneMeshes.at(i)->rigidBody->getWorldTransform().getOrigin().getY(), sceneManager.sceneMeshes.at(i)->rigidBody->getWorldTransform().getOrigin().z());
			}
		}

		for (int i = 0; i < sceneManager.animatedMeshes.size(); i++) //Update according to rigidbody
		{
			bool check = sceneManager.animatedMeshes.at(i)->rigidBody != nullptr;

			btTransform transform;
			if (sceneManager.animatedMeshes.at(i)->rigidBody != nullptr && sceneManager.animatedMeshes.at(i)->rigidBody->getMotionState())
			{
				sceneManager.animatedMeshes.at(i)->setTranslation(DirectX::XMFLOAT3(sceneManager.animatedMeshes.at(i)->rigidBody->getWorldTransform().getOrigin().x(), sceneManager.animatedMeshes.at(i)->rigidBody->getWorldTransform().getOrigin().getY(), sceneManager.animatedMeshes.at(i)->rigidBody->getWorldTransform().getOrigin().z()));
				XMFLOAT3 pos = DirectX::XMFLOAT3(sceneManager.animatedMeshes.at(i)->rigidBody->getWorldTransform().getOrigin().x(), sceneManager.animatedMeshes.at(i)->rigidBody->getWorldTransform().getOrigin().getY(), sceneManager.animatedMeshes.at(i)->rigidBody->getWorldTransform().getOrigin().z());
			}
		}
        		                				
		YSE::System().update();
		
		

		directXHandler->spriteBatch->Begin();
		std::wstring string1;
		std::wstring string2;
		std::wstring string3;
		string1 = std::to_wstring((int)player->boostReserve) + L" jump reserve remaining";
		string2 = std::to_wstring((int)hook->ableToThrowHook) + L" \nhook recharge remaining";
		string3 = std::to_wstring((int)hook->ropeZipBack) + L" \nRope zipback ";
		directXHandler->spriteFont->DrawString(directXHandler->spriteBatch.get(), string1.data(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));
		directXHandler->spriteFont->DrawString(directXHandler->spriteBatch.get(), string2.data(), DirectX::XMFLOAT2(0, 50), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));
		directXHandler->spriteFont->DrawString(directXHandler->spriteBatch.get(), string3.data(), DirectX::XMFLOAT2(0, 100), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));

		directXHandler->spriteBatch->End();

		DxHandler::swapChainPtr->Present(1, 0); 
		DxHandler::contextPtr->ClearState();

		directXHandler->setDefaultState();

		for (int i = 0; i < sceneManager.enemies.size(); i++)
		{
			Enemy* enemy = sceneManager.enemies.at(i);
			if (enemy)
			{
				enemy->update(player);
				//std::cout << enemy->health << std::endl;
				if (enemy->health < 0)
				{
					std::cout << "Died" << std::endl;

					collisionStruct* collStruct = getCollStruct(enemy->model->rigidBody->getUserPointer());
					delete collStruct;

					sceneManager.removeEnemy(enemy);
				}
			}
		}

		updateParticles();

		newTime = std::chrono::high_resolution_clock::now(); //Set new time
		frameTime = std::chrono::duration_cast<std::chrono::duration<double>>(newTime - currentTime); //Get deltaTime for frame
		fixedUpdate(frameTime.count());
	}

	
	/*
		//remove the rigidbodies from the dynamics world and delete them
	for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();
	
	*/
}

void Engine::renderFirstPass(std::vector<Mesh*>* scene)
{
	DxHandler::backfaceCullShader->useThis(DxHandler::contextPtr);

	float background_color[4] = { 0.f, 0.f, 0.f, 1.f };
	//Clear RTVs again
	directXHandler->contextPtr->ClearRenderTargetView(deferredBufferHandler.buffers[GBufferType::Position].renderTargetView, background_color);
	directXHandler->contextPtr->ClearRenderTargetView(deferredBufferHandler.buffers[GBufferType::DiffuseColor].renderTargetView, background_color);
	directXHandler->contextPtr->ClearRenderTargetView(deferredBufferHandler.buffers[GBufferType::Normal].renderTargetView, background_color);
	directXHandler->contextPtr->ClearRenderTargetView(DxHandler::renderTargetPtr, background_color);
	DxHandler::contextPtr->ClearDepthStencilView(DxHandler::depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//--------------------------------------


	ID3D11RenderTargetView* arr[3] =
	{
		deferredBufferHandler.buffers[GBufferType::Position].renderTargetView,
		deferredBufferHandler.buffers[GBufferType::DiffuseColor].renderTargetView,
		deferredBufferHandler.buffers[GBufferType::Normal].renderTargetView,
	};
	directXHandler->contextPtr->OMSetRenderTargets(3, arr, DxHandler::depthStencil); //DEPTH

	

	DxHandler::firstPassPixel->useThis(DxHandler::contextPtr);
	DxHandler::firstPassVertex->useThis(DxHandler::contextPtr);

	for (auto model : *scene) //Draw all meshes 
	{
		directXHandler->draw(model, primaryCamera);
	}

	DxHandler::contextPtr->GSSetShader(NULL, NULL, NULL);
	auto rot = sceneManager.animatedMeshes.at(0)->getRotation();
	rot = XMFLOAT3(rot.x + 0.01f, rot.y + 0.01, 0);
	//animatedMeshes.at(0)->setRotation(rot);
	directXHandler->animVertex->useThis(DxHandler::contextPtr); //Animation vertex shader
	for (auto animMesh : sceneManager.animatedMeshes) //Draw all meshes 
	{
		if (!animMesh->manualUpdate)
		{
			animMesh->stepAnim(1/60);
		}
		directXHandler->draw(animMesh, primaryCamera);
	}

	//Set to null
	ID3D11RenderTargetView* arrNull[1] =
	{
		nullRTV
	};
	directXHandler->contextPtr->OMSetRenderTargets(1, arrNull, NULL); //DEPTH
	// 

	directXHandler->contextPtr->ClearRenderTargetView(DxHandler::renderTargetPtr, background_color);

	DxHandler::contextPtr->GSSetShader(NULL, NULL, NULL);
}

void Engine::renderSecondPass()
{
	
	//directXHandler->setDefaultState();
	//--------------------------------------------------------

	DxHandler::secondPassPixel->useThis(DxHandler::contextPtr);
	DxHandler::secondPassVertex->useThis(DxHandler::contextPtr);
	//--------------------------------------------------------

	directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL); //Application screen

	DxHandler::contextPtr->PSSetShaderResources(0, 1, &deferredBufferHandler.buffers[GBufferType::DiffuseColor].shaderResourceView); //Color
	DxHandler::contextPtr->PSSetShaderResources(1, 1, &deferredBufferHandler.buffers[GBufferType::Normal].shaderResourceView); //Normal
	DxHandler::contextPtr->PSSetShaderResources(2, 1, &deferredBufferHandler.buffers[GBufferType::Position].shaderResourceView); //Position

	
	directXHandler->drawFullscreenQuad(this->primaryCamera);
	
	
	
	float background_color[4] = { 0.f, 0.f, 0.f, 0.f };
	//directXHandler->contextPtr->OMSetRenderTargets(1, &nullRTV, NULL); //Application screen

	//Sky
	directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, DxHandler::depthStencil); //Application screen

	PS_CONSTANT_LIGHT_BUFFER lightBuff;
	lightBuff.camPos = primaryCamera.cameraPosition;
	lightBuff.isSky = true;

	DxHandler::contextPtr->UpdateSubresource(DxHandler::constantPixelBuffer, 0, NULL, &lightBuff, 0, 0);

	DxHandler::skyboxPixel->useThis(DxHandler::contextPtr);
	DxHandler::skyboxVertex->useThis(DxHandler::contextPtr);

	DxHandler::contextPtr->PSSetShaderResources(3, 1, &Skybox::srv); //Skybox
	//directXHandler->drawFullscreenQuad(primaryCamera);
	//DxHandler::contextPtr->OMSetBlendState(NULL, NULL, NULL);
	directXHandler->draw(Skybox::sphereModel, primaryCamera);
	//End sky

	directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL); //Application screen
}

void Engine::renderLightVolumes()
{
	DxHandler::firstPassVertex->useThis(DxHandler::contextPtr);
	DxHandler::lightPixel->useThis(DxHandler::contextPtr);

	float blendingFactor[4] = { 0.5f, 0.5f, 0.5f, 0.0f };
	//DxHandler::contextPtr->OMSetBlendState(states->Additive(), blendingFactor, 0xFFFFFFFF);
	DxHandler::contextPtr->OMSetBlendState(DxHandler::additiveBlendState, blendingFactor, 0xFFFFFFFF);
	for (auto model : sceneManager.lights) //Draw all lights
	{
		directXHandler->draw(model->lightVolume, primaryCamera, false, model);
	}
	DxHandler::contextPtr->OMSetBlendState(NULL, NULL, NULL);

	//Transparent stuff - move later.
	DxHandler::contextPtr->OMSetBlendState(DxHandler::alphaBlendState, blendingFactor, 0xFFFFFFFF);
	DxHandler::transparencyPixel->useThis(DxHandler::contextPtr);
	DxHandler::transparencyVertex->useThis(DxHandler::contextPtr);
	directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, DxHandler::depthStencil); //Application screen
	DxHandler::contextPtr->GSSetShader(NULL, NULL, NULL);
	for (auto model : sceneManager.transparentSceneObjects) //Draw transparent stuff
	{
		directXHandler->draw(model, primaryCamera, false);
	}
	//DxHandler::backfaceCullShader->useThis(DxHandler::contextPtr);
	DxHandler::contextPtr->OMSetBlendState(NULL, NULL, NULL);
	//End of transparency 

	DxHandler::contextPtr->PSSetShaderResources(0, 1, &nullSRV); //Color
	DxHandler::contextPtr->PSSetShaderResources(1, 1, &nullSRV); //Normal
	DxHandler::contextPtr->PSSetShaderResources(2, 1, &nullSRV); //Position

	
}

void Engine::renderParticles()
{
	float blendingFactor[4] = { 0.f, 1.0f, 0.0f, 1.f };

	//Transparent stuff - move later.
	DxHandler::contextPtr->OMSetBlendState(DxHandler::alphaBlendState, blendingFactor, 0xFFFFFFFF);
	DxHandler::particlePixel->useThis(DxHandler::contextPtr);
	DxHandler::transparencyVertex->useThis(DxHandler::contextPtr);
	directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL);//, DxHandler::depthStencil); //Application screen
	//DxHandler::contextPtr->GSSetShader(NULL, NULL, NULL);
	DxHandler::contextPtr->PSSetShaderResources(0, 1, &particles.at(0)->textureView);
	for (Particle* model : particles) //Draw transparent stuff
	{
		directXHandler->draw(model, primaryCamera, false);
	}
	//DxHandler::backfaceCullShader->useThis(DxHandler::contextPtr);
	DxHandler::contextPtr->OMSetBlendState(NULL, NULL, NULL);
}


