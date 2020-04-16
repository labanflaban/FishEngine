#include "Engine.h"
#include "SimpleMath.h"
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

	YSE::System().close();
}

void Engine::initialSetup()
{
	DxHandler::WIDTH = WIDTH;
	DxHandler::HEIGHT = HEIGHT;

	YSE::System().init();

	this->createWindow();
	createDirectX();
	DxHandler::firstPassPixel->compileShader(L"./FirstPassPixel.hlsl", DxHandler::devicePtr);
	DxHandler::firstPassVertex->compileShader(L"./FirstPassVertex.hlsl", DxHandler::devicePtr);

	DxHandler::secondPassPixel->compileShader(L"./SecondPassPixel.hlsl", DxHandler::devicePtr);
	DxHandler::secondPassVertex->compileShader(L"./SecondPassVertex.hlsl", DxHandler::devicePtr);

	//DxHandler::skyboxVertexShader->compileShader(L"./SkyboxVertex", DxHandler::devicePtr);
	//DxHandler::skyboxPixelShader->compileShader(L"./SkyboxPixel.hlsl", DxHandler::devicePtr);
	deferredBufferHandler.init(WIDTH, HEIGHT);

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
	directXHandler->createPSConstBuffer(ps_buff);
	directXHandler->createVSConstBuffer(vs_buff);

	
	this->primaryCamera = Camera(WIDTH, HEIGHT);
}

void Engine::fixedUpdate(double deltaTime) //time in seconds since last frame
{
	//game logic here thanks
	updatePlayerMovement(deltaTime);

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
}

void Engine::createInputHandler()
{
	this->inputHandler = InputHandler(primaryWindow, &primaryCamera);
}

void Engine::updatePlayerMovement(double deltaTime)
{
	/*
	#define ACTIVE_TAG 1
	#define ISLAND_SLEEPING 2
	#define WANTS_DEACTIVATION 3
	#define DISABLE_DEACTIVATION 4
	#define DISABLE_SIMULATION 5
	*/
	player->model->rigidBody->setActivationState(ACTIVE_TAG);

	btVector3 movementVector(0,0,0);
	if (inputHandler.isKeyDown(VK_SPACE) && player->boostReserve >= 10.f)
	{
		//std::cout << "Space" << std::endl;
		movementVector += btVector3(0, 40.f, 0);
		player->boostReserve -= 10.f;
		player->jumpSound.play();
	}
	if (GetAsyncKeyState(0x41)) //A-key
	{
		//std::cout << GetAsyncKeyState(0x41) << std::endl;
		//std::cout << "A" << std::endl;
		movementVector += btVector3(-30, 0, 0);
	}
	if(GetAsyncKeyState(0x44)) // D-key
	{
		//std::cout << "D" << std::endl;
		movementVector += btVector3(30, 0, 0);
	}

	btVector3 orgVel = player->model->rigidBody->getLinearVelocity();
	player->model->rigidBody->setLinearVelocity(btVector3(0, orgVel.y(), 0) + movementVector);

	XMFLOAT3 playerPos = player->model->getTranslation();
	primaryCamera.cameraPosition = XMVectorSet(playerPos.x, playerPos.y+30, playerPos.z - 150, 0);
	primaryCamera.cameraTarget = XMVectorSet(playerPos.x, playerPos.y, playerPos.z, 0);

	Skybox::sphereModel->setTranslation(DirectX::XMFLOAT3(DirectX::XMVectorGetX(primaryCamera.cameraPosition), DirectX::XMVectorGetY(primaryCamera.cameraPosition), DirectX::XMVectorGetZ(primaryCamera.cameraPosition)));
	//::cout << movementVector.x() << " " << movementVector.z() << " " << movementVector.z() << std::endl;
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
	dynamicsWorld->setGravity(btVector3(0, -20, 0));
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
	directXHandler->contextPtr->RSSetViewports(1, &port);
	DxHandler::contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL);
	//--------------------------------------------------------------------------// 
	Mesh* debugObject = new Mesh; //Body
	debugObject->readMeshFromFile("./Models/monkey.obj");
	debugObject->setTranslation(DirectX::XMFLOAT3(3, 0, 4));
	debugObject->setScaling(DirectX::XMFLOAT3(10, 10, 10));
	debugObject->setRotation(DirectX::XMFLOAT3(0, 3.14 / 2, 0));
	this->player = new Player;
	this->player->model = debugObject;
	debugObject->initRigidbody(dynamicsWorld, &collisionShapes, 10);
	this->scene.push_back(debugObject);
	//this->scene.push_back(debugObject);

	Mesh* groundObject = new Mesh; //Ground
	groundObject->readMeshFromFile("./Models/actualCube.obj");
	groundObject->setTranslation(DirectX::XMFLOAT3(3, -25, 4));
	groundObject->setScaling(DirectX::XMFLOAT3(30, 10, 10));
	groundObject->initRigidbody(dynamicsWorld, &collisionShapes, 0);
	this->scene.push_back(groundObject);

	Mesh* groundObject2= new Mesh; //Ground
	groundObject2->readMeshFromFile("./Models/actualCube.obj");
	groundObject2->setTranslation(DirectX::XMFLOAT3(100, -25, 4));
	groundObject2->setScaling(DirectX::XMFLOAT3(30, 10, 10));
	groundObject2->initRigidbody(dynamicsWorld, &collisionShapes, 0);
	this->scene.push_back(groundObject2);
	
	Mesh* groundObject3 = new Mesh; //Ground
	groundObject3->readMeshFromFile("./Models/actualCube.obj");
	groundObject3->setTranslation(DirectX::XMFLOAT3(150, 0, 4));
	groundObject3->setScaling(DirectX::XMFLOAT3(30, 10, 10));
	groundObject3->initRigidbody(dynamicsWorld, &collisionShapes, 0);
	this->scene.push_back(groundObject3);

	Skybox::loadSkybox(DxHandler::devicePtr);
	Skybox::sphereModel->setTranslation(XMFLOAT3(1, 50, 4));
	Skybox::sphereModel->setScaling(XMFLOAT3(3000, 3000, 200));
	Skybox::sphereModel->isSky = true;
	scene.push_back(Skybox::sphereModel);
	//--------------------------------------------------------------------------------------------------- 
	std::chrono::high_resolution_clock::time_point newTime = std::chrono::high_resolution_clock::now(); //Set new time
	std::chrono::duration<double> frameTime = std::chrono::duration_cast<std::chrono::duration<double>>(newTime - currentTime); //Get deltaTime for frame

	MSG msg;
	while (true)
	{
		inputHandler.handleInput();

		currentTime = std::chrono::high_resolution_clock::now(); //Reset time - always needs to be at top
		primaryCamera.updateCamera();
		renderFirstPass(&scene);
		renderSecondPass();

		//upp upp och ivההההg
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
			dynamicsWorld->stepSimulation(frameTime.count(), 1);
			//std::cout << frameTime.count() << std::endl;
			//dynamicsWorld->stepSimulation(1/60, 10);

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
				//scene.at(0).setTranslation(DirectX::XMFLOAT3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()))); //Set visual position to the same
				//primaryCamera.cameraPosition = DirectX::XMVectorSet(DirectX::XMVectorGetX(primaryCamera.cameraPosition), float(trans.getOrigin().getY()), DirectX::XMVectorGetZ(primaryCamera.cameraPosition), 0);
				//primaryCamera.cameraTarget = DirectX::XMVectorSet(DirectX::XMVectorGetX(primaryCamera.cameraPosition), float(trans.getOrigin().getY()), 1, 0);
				//primaryCamera.updateCamera();
			}
		}

		//XMFLOAT3 debugpos = DirectX::XMFLOAT3(scene.at(0)->rigidBody->getWorldTransform().getOrigin().x(), scene.at(0)->rigidBody->getWorldTransform().getOrigin().getY(), scene.at(0)->rigidBody->getWorldTransform().getOrigin().z());
		//std::cout << debugpos.x << " " << debugpos.y << " " << debugpos.z << " " << std::endl;
		for (int i = 0; i < scene.size(); i++)
		{
			bool check = scene.at(i)->rigidBody != nullptr;

			btTransform transform;
			if (scene.at(i)->rigidBody != nullptr && scene.at(i)->rigidBody->getMotionState())
			{
				scene.at(i)->setTranslation(DirectX::XMFLOAT3(scene.at(i)->rigidBody->getWorldTransform().getOrigin().x(), scene.at(i)->rigidBody->getWorldTransform().getOrigin().getY(), scene.at(i)->rigidBody->getWorldTransform().getOrigin().z()));
				XMFLOAT3 pos = DirectX::XMFLOAT3(scene.at(i)->rigidBody->getWorldTransform().getOrigin().x(), scene.at(i)->rigidBody->getWorldTransform().getOrigin().getY(), scene.at(i)->rigidBody->getWorldTransform().getOrigin().z());
			}
		}


		newTime = std::chrono::high_resolution_clock::now(); //Set new time
		frameTime = std::chrono::duration_cast<std::chrono::duration<double>>(newTime - currentTime); //Get deltaTime for frame
		YSE::System().update();
		fixedUpdate(frameTime.count());
		DxHandler::swapChainPtr->Present(1, 0); 
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

	float background_color[4] = { 0.f, 0.f, 0.f, 1.f };
	//Clear RTVs again
	directXHandler->contextPtr->ClearRenderTargetView(deferredBufferHandler.buffers[GBufferType::Position].renderTargetView, background_color);
	directXHandler->contextPtr->ClearRenderTargetView(deferredBufferHandler.buffers[GBufferType::DiffuseColor].renderTargetView, background_color);
	directXHandler->contextPtr->ClearRenderTargetView(deferredBufferHandler.buffers[GBufferType::Normal].renderTargetView, background_color);
	directXHandler->contextPtr->ClearRenderTargetView(DxHandler::renderTargetPtr, background_color);
	DxHandler::contextPtr->ClearDepthStencilView(DxHandler::depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//--------------------------------------
	
	DxHandler::contextPtr->PSSetShaderResources(1, 1, &Skybox::srv); //Skybox

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
		directXHandler->draw(model, primaryCamera, model->isSky);
	}

	directXHandler->contextPtr->ClearRenderTargetView(DxHandler::renderTargetPtr, background_color);
}

void Engine::renderSecondPass()
{

	directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL);

	//Draw Text
	//directXHandler->drawText();
	//directXHandler->spriteBatch->Begin();
	//directXHandler->spriteFont->DrawString(directXHandler->spriteBatch.get(), L"Test", DirectX::XMFLOAT2(0, 0), DirectX::Colors::Red, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(4.0f, 4.0f));
	//directXHandler->spriteBatch->End();

	DxHandler::contextPtr->PSSetShaderResources(0, 1, &deferredBufferHandler.buffers[GBufferType::DiffuseColor].shaderResourceView); //Color
	DxHandler::contextPtr->PSSetShaderResources(1, 1, &deferredBufferHandler.buffers[GBufferType::Normal].shaderResourceView); //Normal
	DxHandler::contextPtr->PSSetShaderResources(2, 1, &deferredBufferHandler.buffers[GBufferType::Position].shaderResourceView); //Position


	DxHandler::secondPassPixel->useThis(DxHandler::contextPtr);
	DxHandler::secondPassVertex->useThis(DxHandler::contextPtr);

	directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL);

	DxHandler::contextPtr->PSSetShaderResources(0, 1, &deferredBufferHandler.buffers[GBufferType::DiffuseColor].shaderResourceView); //Color
	DxHandler::contextPtr->PSSetShaderResources(1, 1, &deferredBufferHandler.buffers[GBufferType::Normal].shaderResourceView); //Normal
	DxHandler::contextPtr->PSSetShaderResources(2, 1, &deferredBufferHandler.buffers[GBufferType::Position].shaderResourceView); //Position

	
	directXHandler->drawFullscreenQuad();
	
	
	float background_color[4] = { 0.f, 0.f, 0.f, 1.f };
}
