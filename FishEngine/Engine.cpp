#include "Engine.h"

Engine::Engine()
{
}

Engine::~Engine()
{
	delete DxHandler::firstPassPixel;
	delete DxHandler::secondPassPixel;

	delete DxHandler::firstPassVertex;
	delete DxHandler::secondPassVertex;
}

void Engine::initialSetup()
{
	DxHandler::WIDTH = WIDTH;
	DxHandler::HEIGHT = HEIGHT;

	this->createWindow();
	createDirectX();
	DxHandler::firstPassPixel->compileShader(L"./FirstPassPixel.hlsl", DxHandler::devicePtr);
	DxHandler::firstPassVertex->compileShader(L"./FirstPassVertex.hlsl", DxHandler::devicePtr);

	DxHandler::secondPassPixel->compileShader(L"./SecondPassPixel.hlsl", DxHandler::devicePtr);
	DxHandler::secondPassVertex->compileShader(L"./SecondPassVertex.hlsl", DxHandler::devicePtr);
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

	//physics

	//
	this->primaryCamera = Camera(WIDTH, HEIGHT);
}

void Engine::fixedUpdate(double deltaTime) //time in seconds since last frame
{
	//game logic here thanks
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

void Engine::engineLoop()
{
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
	Mesh debugObject; //Body
	debugObject.readMeshFromFile("./Models/actualCube.obj");
	debugObject.setTranslation(DirectX::XMFLOAT3(3, 0, 4));
	debugObject.setScaling(DirectX::XMFLOAT3(10, 10, 10));
	//this->scene.push_back(debugObject);

	Mesh groundObject; //Ground
	groundObject.readMeshFromFile("./Models/actualCube.obj");
	groundObject.setTranslation(DirectX::XMFLOAT3(3, -25, 4));
	groundObject.setScaling(DirectX::XMFLOAT3(30, 10, 10));
	//this->scene.push_back(groundObject);

	//Camera primaryCamera = Camera(WIDTH, HEIGHT);
	//--------------------------------------------------------------------------------------------------- physics 
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	//btCollisionWorld* collisionWorld = new btCollisionWorld(dispatcher, overlappingPairCache, collisionConfiguration);
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	//Ground stuff
	btTransform groundTransform;
	//groundTransform.setOrigin(btVector3(groundObject.getTranslation().x, groundObject.getTranslation().y, groundObject.getTranslation().z));
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(groundObject.getTranslation().x, groundObject.getTranslation().y, groundObject.getTranslation().z));
	btCollisionShape* groundCollider = new btBoxShape(btVector3(btScalar(groundObject.getScaling().x), btScalar(groundObject.getScaling().y), btScalar(groundObject.getScaling().z)));
	collisionShapes.push_back(groundCollider);

	btDefaultMotionState* groundState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo2(0, groundState, groundCollider, btVector3(0, 0, 0));
	btRigidBody* groundBody = new btRigidBody(rbInfo2);
	groundObject.rigidBody = groundBody;
	dynamicsWorld->addRigidBody(groundBody);
	

	
	btCollisionShape* collider = new btBoxShape(btVector3(btScalar(debugObject.getScaling().x), btScalar(debugObject.getScaling().y), btScalar(debugObject.getScaling().z)));
	collisionShapes.push_back(collider);

	btTransform startTransform;
	startTransform.setOrigin(btVector3(debugObject.getTranslation().x, debugObject.getTranslation().y, debugObject.getTranslation().z));

	btScalar mass(1.f);
	startTransform.setIdentity();
	
	bool isDynamic = (mass != 0.f);
	btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			collider->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, collider, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	debugObject.rigidBody = body;
	    

	dynamicsWorld->setGravity(btVector3(0, -4, 0));
	dynamicsWorld->addRigidBody(body);

	this->scene.push_back(groundObject);
	this->scene.push_back(debugObject);

	//--------------------------------------------------------------------------------------------------- 

	MSG msg;
	while (true)
	{
		currentTime = std::chrono::high_resolution_clock::now(); //Reset time - always needs to be at top
		primaryCamera.updateCamera();
		renderFirstPass(&scene);
		renderSecondPass();

		//upp upp och iv����g
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_DESTROY)
		{
			PostQuitMessage(0);
		}

		std::chrono::high_resolution_clock::time_point newTime = std::chrono::high_resolution_clock::now(); //Set new time
		std::chrono::duration<double> frameTime = std::chrono::duration_cast<std::chrono::duration<double>>(newTime - currentTime); //Get deltaTime for frame

		

		///-----stepsimulation_start-----
		for (int i = 0; i < 15; i++)
		{
			dynamicsWorld->stepSimulation(frameTime.count(), 10);

			//print positions of all objects
			for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
			{
				btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
				btRigidBody* body = btRigidBody::upcast(obj);

				btTransform trans;
				if (body && body->getMotionState())
				{
					body->getMotionState()->getWorldTransform(trans);
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
				primaryCamera.updateCamera();
			}
		}

		for (int i = 0; i < scene.size(); i++)
		{
			bool check = scene.at(i).rigidBody != nullptr;

			btTransform transform;
			if (scene.at(i).rigidBody && scene.at(i).rigidBody->getMotionState())
			{
				std::cout << "Passed" << std::endl;
				scene.at(i).setTranslation(DirectX::XMFLOAT3(scene.at(i).rigidBody->getWorldTransform().getOrigin().x(), scene.at(i).rigidBody->getWorldTransform().getOrigin().getY(), scene.at(i).rigidBody->getWorldTransform().getOrigin().z()));

			}
		}

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

void Engine::renderFirstPass(std::vector<Mesh>* scene)
{
	DxHandler::firstPassPixel->useThis(DxHandler::contextPtr);
	DxHandler::firstPassVertex->useThis(DxHandler::contextPtr);

	float background_color[4] = { 0.f, 0.f, 0.f, 1.f };
	//Clear RTVs again
	directXHandler->contextPtr->ClearRenderTargetView(deferredBufferHandler.buffers[GBufferType::Position].renderTargetView, background_color);
	directXHandler->contextPtr->ClearRenderTargetView(deferredBufferHandler.buffers[GBufferType::DiffuseColor].renderTargetView, background_color);
	directXHandler->contextPtr->ClearRenderTargetView(deferredBufferHandler.buffers[GBufferType::Normal].renderTargetView, background_color);
	directXHandler->contextPtr->ClearRenderTargetView(DxHandler::renderTargetPtr, background_color);
	DxHandler::contextPtr->ClearDepthStencilView(DxHandler::depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	ID3D11RenderTargetView* arr[3] =
	{
		deferredBufferHandler.buffers[GBufferType::Position].renderTargetView,
		deferredBufferHandler.buffers[GBufferType::DiffuseColor].renderTargetView,
		deferredBufferHandler.buffers[GBufferType::Normal].renderTargetView,
	};
	directXHandler->contextPtr->OMSetRenderTargets(3, arr, DxHandler::depthStencil); //DEPTH


	for (auto model : *scene) //Draw all meshes 
	{
		directXHandler->draw(&model, primaryCamera);
	}

	directXHandler->contextPtr->ClearRenderTargetView(DxHandler::renderTargetPtr, background_color);
}

void Engine::renderSecondPass()
{
	DxHandler::secondPassPixel->useThis(DxHandler::contextPtr);
	DxHandler::secondPassVertex->useThis(DxHandler::contextPtr);

	directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL);

	DxHandler::contextPtr->PSSetShaderResources(0, 1, &deferredBufferHandler.buffers[GBufferType::DiffuseColor].shaderResourceView); //Color
	DxHandler::contextPtr->PSSetShaderResources(1, 1, &deferredBufferHandler.buffers[GBufferType::Normal].shaderResourceView); //Normal
	DxHandler::contextPtr->PSSetShaderResources(2, 1, &deferredBufferHandler.buffers[GBufferType::Position].shaderResourceView); //Position

	directXHandler->drawFullscreenQuad();

	float background_color[4] = { 0.f, 0.f, 0.f, 1.f };
}
