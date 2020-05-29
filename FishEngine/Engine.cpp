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

	YSE::System().close();
}

void Engine::initialSetup()
{

	//DxHandler::WIDTH = WIDTH;
	//DxHandler::HEIGHT = HEIGHT;

	YSE::System().init();
	GameSoundtrack.create("./Sounds/SoundTrack.wav");
	GameSoundtrack.setVolume(0.02f);
	GameSoundtrack.play();
	//GameSoundtrack.isLooping();
	GameSoundtrack.setLooping(true);
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

	DxHandler::GuiShaderVertex->compileShader(L"./GuiShaderVertex.hlsl", DxHandler::devicePtr);
	DxHandler::GuiShaderPixel->compileShader(L"./GuiShaderPixel.hlsl", DxHandler::devicePtr);

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

	createGUIHandler();

	directXHandler->generateFullScreenQuad();

	PS_CONSTANT_LIGHT_BUFFER ps_buff;
	VS_CONSTANT_MATRIX_BUFFER vs_buff;

	VS_CONSTANT_ANIM_BUFFER vs_anim_buff;

	PS_CONSTANT_GUI_BUFFER ps_gui_buff;

	directXHandler->createPSConstBuffer(ps_buff);
	directXHandler->createVSConstBuffer(vs_buff);
	directXHandler->createVSAnimBuffer(vs_anim_buff);
	directXHandler->createGSConstBuffer();
	directXHandler->createPSGuiBuffer(ps_gui_buff);
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

	particleMesh = new Mesh(DxHandler::devicePtr);
	particleMesh->readTextureFromFile(L"./Textures/bubble.png");

	particleMesh->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
	particleMesh->vertices.push_back(Vertex{ 1, -1, 0.1f,    1, 1, 1, 1, 1, 1, 0, 0, -1 });
	particleMesh->vertices.push_back(Vertex{ -1,  -1, 0.1f,  1, 1, 1, 1, 0, 1, 0, 0, -1 });

	particleMesh->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
	particleMesh->vertices.push_back(Vertex{ 1,  1, 0.1f,   1, 1, 1, 1, 1, 0, 0, 0, -1 });
	particleMesh->vertices.push_back(Vertex{ 1,  -1, 0.1f, 1, 1, 1, 1, 1, 1, 0, 0, -1 });

	particleMesh->createVertexBuffer();

	AssetLoader::devicePtr = DxHandler::devicePtr;
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
	if (inputHandler.isKeyDown(VK_ESCAPE))
	{
		guiHandler->showMainMenu();
		scoreHandle.readFromFile("score.txt");
		pause = true;
	}

	if (inputHandler.isKeyDown(VK_SPACE) && player->boostReserve >= 5.f)
	{

		//std::cout << "Space" << std::endl;
		movementVector += btVector3(0, 40.f, 0);

		player->boostReserve -= 10.f;
		player->jumpSound.play();

		for (int i = 0; i < 100; i++)
		{
			Particle* particlePtr = new Particle(DxHandler::devicePtr);
			//particlePtr->readTextureFromFile(L"./Textures/bubble.png");
			particlePtr->vertexBuffer = particleMesh->vertexBuffer;
			particlePtr->textureView = particleMesh->textureView;
			particlePtr->hasTexture = particleMesh->hasTexture;
			particlePtr->nrOfVertices = particleMesh->nrOfVertices;

			float randomNumber = (float)((rand() % 30) + 5) / 10;
			particlePtr->setScaling(DirectX::XMFLOAT3(randomNumber, randomNumber, randomNumber));
			particlePtr->orgSize = particlePtr->getScaling();
			particlePtr->setTranslation(DirectX::XMFLOAT3(-30, 25, 0));

			std::random_device randomSeed;
			std::mt19937 numberGenerator(randomSeed());
			std::uniform_real_distribution<> randomNum(-0.5f, 0.5f); //Between -1 - 1
			std::uniform_real_distribution<> randomNumPlus(0.2f, 1.f); //Between 0.8 - 1
			std::uniform_real_distribution<> randomNumAngle(-3.14/2, 3.14f/2); //Between 0.8 - 1
			float angle = randomNumAngle(numberGenerator);
			float xVel = cos(angle+3.14/2);
			float yVel = sin(angle+3.14 / 2);


			particlePtr->setTranslation(player->model->getTranslation());
			//particlePtr->velocity = DirectX::XMFLOAT3(rNum * 0.5 + randomNum(numberGenerator) + player->model->rigidBody->getLinearVelocity().x() * 0.1, 0.5 * rNumY + 1 * rNumY + player->model->rigidBody->getLinearVelocity().y() * 0.1, 0);
			particlePtr->velocity = XMFLOAT3(xVel*randomNumPlus(numberGenerator) + player->model->rigidBody->getLinearVelocity().x() * 0.001, yVel * randomNumPlus(numberGenerator), randomNum(numberGenerator));
			sceneManager.addParticle(particlePtr);
		}
	}
	if (GetAsyncKeyState(0x41)) //A-key
	{

		//std::cout << GetAsyncKeyState(0x41) << std::endl;
		//std::cout << "A" << std::endl;
		movementVector += btVector3(-130/5 * deltaTime * 40, 0, 0);
	}
	if(GetAsyncKeyState(0x44)) // D-key
	{
		movementVector += btVector3(130/5 * deltaTime * 40, 0, 0);

	}

	btVector3 orgVel = player->model->rigidBody->getLinearVelocity();
	player->model->rigidBody->setLinearVelocity(btVector3(0, orgVel.y(), 0) + movementVector);
	if (!movementVector.isZero() || player->attacking)
	{

	}
	else
	{
		player->model->fromIndex = 0;
		player->model->targetPoseIndex = 0;
		player->model->remaining = 0;
		player->model->t = 0;
	}


	player->stepAnim(deltaTime);
	player->updatePlayer(fishingRod, hook, rope, movementVector.x());

	if (movementVector.x() > 0)
		this->player->model->setRotation(XMFLOAT3(0, 3.14/2, 0));
	else
		if ((movementVector.x() < 0))
		this->player->model->setRotation(XMFLOAT3(0, -3.14/2, 0));

	XMFLOAT3 playerPos = player->model->getTranslation();
	primaryCamera.cameraPosition = XMVectorSet(playerPos.x, playerPos.y+30, playerPos.z - 150, 0);
	primaryCamera.cameraTarget = XMVectorSet(playerPos.x, playerPos.y, playerPos.z, 0);

	Skybox::sphereModel->setTranslation(DirectX::XMFLOAT3(DirectX::XMVectorGetX(primaryCamera.cameraPosition), DirectX::XMVectorGetY(primaryCamera.cameraPosition), DirectX::XMVectorGetZ(primaryCamera.cameraPosition)));
	player->playerLight->setPosition(XMFLOAT3(playerPos.x, playerPos.y + 25, playerPos.z));
	hook->hookLight->setPosition(DirectX::XMFLOAT3(hook->model->getTranslation().x, hook->model->getTranslation().y, hook->model->getTranslation().z));
}





void Engine::updateParticles()
{
	for (int i = 0; i < sceneManager.particles.size(); i++)
	{
		Particle* p = sceneManager.particles.at(i);
		p->updateParticle();

		if (p->ticksLeft <= 0)
		{
			sceneManager.removeParticle(p);
		}
	}
}

void Engine::updateGUI()
{
	if(guiHandler->checkButtons() == 1)
	{
		gameOver = false;
		guiHandler->hideMainMenu();
		cout << "STARTING GAME!" << endl;
		pause = false;
	}

	else if(guiHandler->checkButtons() == 2)
	{
		cout << "QUITING APP!" << endl;
		this->shutdown = true;
		//exit(1);
	}

	if (inputHandler.windowClosed)
		this->shutdown = true;

	if(!pause)
	{
		guiHandler->updateHUD(player->health);
	}
}

void Engine::resetEnemies()
{
	for (int i = 0; i < sceneManager.enemies.size(); i++)
	{
		sceneManager.enemies.at(i)->resetEnemy();
	}
}

void Engine::resetDrops()
{
	for (int i = 0; i < sceneManager.hearts.size(); i++)
	{
		sceneManager.hearts.at(i)->resetDrop();
	}

	for (int i = 0; i < sceneManager.points.size(); i++)
	{
		sceneManager.points.at(i)->resetDrop();
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

		collisionStruct* collision = nullptr;
		collisionStruct* collision1 = nullptr;

		if (isValidStruct(colliderA->getUserPointer()) && isValidStruct(colliderB->getUserPointer())) //If both have an enum that is not ignore, ie is a rod, hook or enemy
		{
			collision = getCollStruct(manif->getBody0()->getUserPointer());
			collision1 = getCollStruct(manif->getBody1()->getUserPointer());

			//collision->type = collisionEnums::Hook;

			if (((collision->type == collisionEnums::Hook) && (collision1->type == collisionEnums::Enemy)) || ((collision1->type == collisionEnums::Hook) && (collision->type == collisionEnums::Enemy)))
			{
				//H�mta hooken h�r, kolla om den �r active, kolla enum p� b�da collision och se vilken som har enum hook och sedan h�mta d�r
				Tool* hook = nullptr;
				if (collision->type == collisionEnums::Hook)
				{
					hook = collision->hook;
				}
				else
				{
					hook = collision1->hook;
				}

				Enemy* myEnemy = nullptr; //S�tt denna i if-checken
				if (collision->type == collisionEnums::Enemy)
				{
					myEnemy = collision->enemy;
				}
				else
				{
					myEnemy = collision1->enemy;
				}


				if (myEnemy->damageDebounce >= myEnemy->maxDebounce && hook->isActive)
				{
					myEnemy->health -= 100;
					myEnemy->damageDebounce = 0;
					myEnemy->model->rigidBody->setLinearVelocity(btVector3(10, 0, 0));
					//std::cout << "Enemy hit" << std::endl;
					myEnemy->justHit = true;

				}
			}

			if (((collision->type == collisionEnums::Rod) && (collision1->type == collisionEnums::Enemy)) || ((collision1->type == collisionEnums::Rod) && (collision->type == collisionEnums::Enemy)))
			{
				Tool* rod = nullptr;
				if (collision->type == collisionEnums::Rod)
				{
					rod = collision->rod;
				}
				else
				{
					rod = collision1->rod;
				}

				Enemy* myEnemy = nullptr;
				if (collision->type == collisionEnums::Enemy)
				{
					myEnemy = collision->enemy;
				}
				else
				{
					myEnemy = collision1->enemy;
				}

				//std::cout << myEnemy->damageDebounce << std::endl;
				//std::cout << "Health: " << myEnemy->health << std::endl;
				//std::cout << "Debounce " << myEnemy->damageDebounce << std::endl;
				if (myEnemy->damageDebounce >= myEnemy->maxDebounce && rod->isActive)
				{
					myEnemy->health -= 100;
					myEnemy->damageDebounce = 0;
					myEnemy->model->rigidBody->setLinearVelocity(btVector3(10, 0, 0));
					//std::cout << "Enemy hit" << std::endl;
					myEnemy->justHit = true;
				}

				//if (myEnemy->health < 0)
					//std::cout << "Should be dead" << std::endl;
			}

			if (((collision->type == collisionEnums::Player) && (collision1->type == collisionEnums::Enemy)) || ((collision1->type == collisionEnums::Player) && (collision->type == collisionEnums::Enemy)))
			{
				Player* myPlayer = nullptr;
				if (collision->type == collisionEnums::Player)
				{
					myPlayer = collision->player;
				}
				else
				{
					myPlayer = collision1->player;
				}

				Enemy* myEnemy = nullptr;
				if (collision->type == collisionEnums::Enemy)
				{
					myEnemy = collision->enemy;
				}
				else
				{
					myEnemy = collision1->enemy;
				}

				if (myEnemy->damageDebounce >= myEnemy->maxDebounce)
				{
					//if (myPlayer->model->getTranslation().y > myEnemy->model->getTranslation().y)
						//myEnemy->health = 0;
					//else
					//{
						myPlayer->health--;
						myEnemy->damageDebounce = 0;

						//std::cout << "Health: " << myPlayer->health << std::endl;
					//}
						
					//std::cout << "Enemy hit" << std::endl;
				}

				//if (myPlayer->health < 0)
					//std::cout << "Player should be dead " << myPlayer->health << std::endl;

				//std::cout << "Enemy Health: " << myEnemy->health << "\nDebounce: " << myEnemy->damageDebounce << std::endl;
			}

			if (((collision->type == collisionEnums::Player) && (collision1->type == collisionEnums::BounceObject)) || ((collision1->type == collisionEnums::Player) && (collision->type == collisionEnums::BounceObject)))
			{
				Player* myPlayer = nullptr;
				if (collision->type == collisionEnums::Player)
				{
					myPlayer = collision->player;
				}
				else
				{
					myPlayer = collision1->player;
				}

				AnimatedMesh* bounceObject = nullptr;
				if (collision->type == collisionEnums::BounceObject)
				{
					bounceObject = collision->bounceObject;
				}
				else
				{
					bounceObject = collision1->bounceObject;
				}

				btVector3 currVel = myPlayer->model->rigidBody->getLinearVelocity();

				btVector3 newVel = btVector3(currVel.x(), 45.f, currVel.z());
				myPlayer->model->rigidBody->setLinearVelocity(newVel);
			}

			if (((collision->type == collisionEnums::Player) && (collision1->type == collisionEnums::Heart)) || ((collision1->type == collisionEnums::Player) && (collision->type == collisionEnums::Heart)))
			{
				Player* myPlayer = nullptr;
				if (collision->type == collisionEnums::Player)
				{
					myPlayer = collision->player;
				}
				else
				{
					myPlayer = collision1->player;
				}

				Heartdrop* heartDrop = nullptr;
				if (collision->type == collisionEnums::Heart)
				{
					//myEnemy = collision->enemy;
					heartDrop = collision->heartDrop;
				}
				else
				{
					heartDrop = collision1->heartDrop;
				}

				if (!heartDrop->usedUp)
				{
					if (myPlayer->health < myPlayer->maxHealth)
						myPlayer->health++;
					heartDrop->usedUp = true;
				}
			}

			if (((collision->type == collisionEnums::Player) && (collision1->type == collisionEnums::Pointdrop)) || ((collision1->type == collisionEnums::Player) && (collision->type == collisionEnums::Pointdrop)))
			{
				Player* myPlayer = nullptr;
				if (collision->type == collisionEnums::Player)
				{
					myPlayer = collision->player;
				}
				else
				{
					myPlayer = collision1->player;
				}

				Pointdrop* pointDrop = nullptr;
				if (collision->type == collisionEnums::Pointdrop)
				{
					//myEnemy = collision->enemy;
					pointDrop = collision->pointDrop;
				}
				else
				{
					pointDrop = collision1->pointDrop;
				}

				if (!pointDrop->usedUp)
				{
					myPlayer->points += 100;
					pointDrop->usedUp = true;
				}
			}

			if (((collision->type == collisionEnums::Player) && (collision1->type == collisionEnums::Spike)) || ((collision1->type == collisionEnums::Player) && (collision->type == collisionEnums::Spike)))
			{
				Player* myPlayer = nullptr;
				if (collision->type == collisionEnums::Player)
				{
					myPlayer = collision->player;
				}
				else
				{
					myPlayer = collision1->player;
				}

				Spike* spike = nullptr;
				if (collision->type == collisionEnums::Spike)
				{
					//myEnemy = collision->enemy;
					spike = collision->spike;
				}
				else
				{
					spike = collision1->spike;
				}

				//std::cout << spike->debounce << std::endl;
				if (spike->debounce >= spike->debounceLimit)
				{
					spike->debounce = 0;
					myPlayer->health = 0;
				}


			}
		}
	}
}

void Engine::fixedUpdate(double deltaTime, btDiscreteDynamicsWorld* dynamicWorld) //time in seconds since last frame
{
	//game logic here thanks

	if (bubbleDebounce < bubbleDebounceLimit)
		bubbleDebounce += 0.1;

	if (bubbleDebounce >= bubbleDebounceLimit)
	{
		generateRandomBubbles();
		bubbleDebounce = 0;
	}

	for (int i = 0; i < backgroundFishVec.size(); i++)
	{
		backgroundFishVec.at(i).model->move(backgroundFishVec.at(i).velocity);

		if (abs(backgroundFishVec.at(i).model->getTranslation().x - player->model->getTranslation().x) > 2500)
			backgroundFishVec.at(i).model->setTranslation(XMFLOAT3(player->model->getTranslation().x - 2500, backgroundFishVec.at(i).model->getTranslation().y, backgroundFishVec.at(i).model->getTranslation().z));
	}


	updatePlayerMovement(deltaTime);
	player->updatePlayerTools(fishingRod, hook, rope, deltaTime);
	if (fishingRod->pullback < 10.f)
		fishingRod->pullback += 0.1f;

	if (hook->ableToThrowHook < 10.f)
		hook->ableToThrowHook += 0.1f;

	for (int i = 0; i < sceneManager.enemies.size(); i++)
	{
		if (sceneManager.enemies.at(i)->damageDebounce < sceneManager.enemies.at(i)->maxDebounce) //L�gg 3 i enemy i en variabel
			sceneManager.enemies.at(i)->damageDebounce += 0.1;
	}

	for (int i = 0; i < sceneManager.enemies.size(); i++)
	{
		if (sceneManager.enemies.at(i)->damageDebounce < sceneManager.enemies.at(i)->maxDebounce) //L�gg 3 i enemy i en variabel
			sceneManager.enemies.at(i)->damageDebounce += 0.1;
	}

	for (int i = 0; i < sceneManager.spikes.size(); i++)
	{
		if (sceneManager.spikes.at(i)->debounce <= sceneManager.spikes.at(i)->debounceLimit)
			sceneManager.spikes.at(i)->debounce += 0.1;
	}

	if (hook->ropeZipBack < 10.f)
		hook->ropeZipBack += 0.1f;

	if (player->boostReserve < 10.f)
		player->boostReserve += 0.1f;

	if (player->boostReserve <= 0.2f)
	{

	}

	for (int i = 0; i < sceneManager.enemies.size(); i++)
	{
		if (sceneManager.enemies.at(i)->active)
		{
			Enemy* enemy = sceneManager.enemies.at(i);
			if (enemy)
			{
				enemy->update(player);
				//std::cout << "Health: " << enemy->health << std::endl;
				if (enemy->health < 0)
				{


					//collisionStruct* collStruct = getCollStruct(enemy->model->rigidBody->getUserPointer());
					//delete collStruct;

					//dynamicWorld->removeRigidBody(enemy->model->rigidBody);

					enemy->active = false;
					enemy->moveAway();
					this->player->points += 100;
				}
			}
		}
	}

	if (player->health <= 0)
	{
		resetDrops();
		guiHandler->showMainMenu();
		player->points = 0;

		player->health = player->maxHealth;
		guiHandler->resetHealth(player->health);
		player->resetPlayer();
		resetEnemies();
		pause = true;
	}

	if (level != nullptr && player->model->getTranslation().y < level->respawn)
	{
		resetDrops();
		player->resetPlayer();
		player->points = 0;
		resetEnemies();
	}


	if (level != nullptr && player->model->getTranslation().x > level->goal)
	{
		guiHandler->showMainMenu();
		scoreHandle.readFromFile("score.txt");
		scoreHandle.writeToFile("score.txt", (int)this->player->points, player->gameTime);
		gameOver = true;
		pause = true;
		player->resetPlayer();
		player->points = 0;
		resetEnemies();
		resetDrops();
	}

	for (int i = 0; i < sceneManager.spikes.size(); i++)
	{
		XMFLOAT3 rot = sceneManager.spikes.at(i)->model->getRotation();
		sceneManager.spikes.at(i)->model->setRotation(XMFLOAT3(rot.x + 0.7 * deltaTime, rot.y + 0.7 * deltaTime, rot.z + 0.7 * deltaTime));
	}

	for (int i = 0; i < sceneManager.points.size(); i++)
	{
		XMFLOAT3 rot = sceneManager.points.at(i)->model->getRotation();
		sceneManager.points.at(i)->model->setRotation(XMFLOAT3(0, rot.y + 0.7 * deltaTime, 0));
	}

	for (int i = 0; i < sceneManager.hearts.size(); i++)
	{
		XMFLOAT3 rot = sceneManager.hearts.at(i)->model->getRotation();
		sceneManager.hearts.at(i)->model->setRotation(XMFLOAT3(0, rot.y + 0.7 * deltaTime, 0));
	}

	guiHandler->currentHealth = player->health;
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
	dynamicsWorld->setGravity(btVector3(0, -9.82f, 0));
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
	std::vector<Vertex> vertVectorPlr = ObjParser::readFromObj("./Models/characterStart.obj");//readFromFile("./Models/characterStart.FID");
	std::vector<Vertex> vertVectorPlr1 = ObjParser::readFromObj("./Models/characterMiddle1.obj");
	std::vector<Vertex> vertVectorPlr2 = ObjParser::readFromObj("./Models/characterMiddle2.obj");
	std::vector<Vertex> vertVectorPlr3 = ObjParser::readFromObj("./Models/characterMiddle3.obj");

	std::vector<Vertex> vertVectorPlr4 = ObjParser::readFromObj("./Models/characterFishingStart.obj");
	std::vector<Vertex> vertVectorPlr5 = ObjParser::readFromObj("./Models/characterFishingMiddle.obj");
	std::vector<Vertex> vertVectorPlr6 = ObjParser::readFromObj("./Models/characterFishingEnd.obj");
	std::vector<Vertex> vertVectorPlr7 = ObjParser::readFromObj("./Models/characterFishingMiddle.obj");
	std::vector<Vertex> vertVectorPlr8 = ObjParser::readFromObj("./Models/characterFishingStart.obj");
	//std::vector<Vertex> vertVectorPlr4 = ObjParser::readFromObj("./Models/characterEnd.obj");

	//std::vector<Vertex> vertVectorPlr3 = FIDParser::readFromFID("./Models/characterEmd.FID");

	std::vector<Vertex>* plrArr[] = { &vertVectorPlr, &vertVectorPlr1, &vertVectorPlr2, &vertVectorPlr3, &vertVectorPlr4, &vertVectorPlr5, &vertVectorPlr6, &vertVectorPlr7, &vertVectorPlr8};
	debugObject->appendStructuredBuffer(plrArr, 9);
	debugObject->createStructuredBuffer(DxHandler::devicePtr);
	debugObject->setTranslation(DirectX::XMFLOAT3(200, 0, 4));
	debugObject->setScaling(DirectX::XMFLOAT3(2, 2, 2));
	debugObject->setRotation(DirectX::XMFLOAT3(0, 3.14 / 2, 0));
	debugObject->manualUpdate = true;
	this->player = new Player(&inputHandler);
	this->player->model = debugObject;
	player->model->animationSpeed = 5;
	debugObject->initRigidbody(dynamicsWorld, &collisionShapes, 10, nullptr, btVector3(0,25,0));
	collisionStruct* plrCollStruct = new collisionStruct(player, collisionEnums::Player);
	player->model->rigidBody->setUserPointer(plrCollStruct);

	this->sceneManager.addAnimatedMesh(debugObject);
	debugObject->targetPoseIndex = 1;

	Mesh* hookObject = new Mesh(DxHandler::devicePtr); //Hook
	hookObject->readMeshFromFile("./Models/hook.obj");
	hookObject->setScaling(DirectX::XMFLOAT3(2, 2, 2));
	hookObject->setRotation(DirectX::XMFLOAT3(0, 3.14/2, 0));
	this->hook = new Tool(&inputHandler);
	this->hook->model = hookObject;
	collisionStruct* hookCollStruct = new collisionStruct(hook, collisionEnums::Hook);
	hookCollStruct->hook = hook;
	hookObject->initRigidbody(dynamicsWorld, &collisionShapes, 5);
	hookObject->rigidBody->setUserPointer(hookCollStruct);
	this->sceneManager.addMesh(hookObject);

	//MovingPlatform platform;
	//Mesh* platformMesh = new Mesh(DxHandler::devicePtr);
	//platformMesh->readMeshFromFile("./Models/GroundThree.Obj");
	//platformMesh->setTranslation(XMFLOAT3(-25, 0, 0));
	//platformMesh->setScaling(XMFLOAT3(20, 10, 10));
	//btBoxShape* box = new btBoxShape(btVector3(btScalar(platformMesh->getScaling().x * 3.2f), btScalar(platformMesh->getScaling().y * 0.1), btScalar(platformMesh->getScaling().z) + 6));
	//platformMesh->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);
	////btCollisionObject* collObj = new btCollisionObject();
	////btTransform colliderTrans;
	////colliderTrans.setIdentity();
	////colliderTrans.setOrigin(btVector3(platformMesh->getTranslation().x, platformMesh->getTranslation().y, platformMesh->getTranslation().z));
	////collObj->setWorldTransform(colliderTrans);
	////collObj->setCollisionShape(box);
	////dynamicsWorld->addCollisionObject(collObj);
	//platformMesh->readNormalMapFromFile(L"./Textures/StoneNormal.png");
	//platform.startPos = platformMesh->getTranslation();
	//platform.platform = platformMesh;
	//sceneManager.addMesh(platformMesh);

	player->playerLight = new Light(DxHandler::devicePtr);
	player->playerLight->lightColor = XMVectorSet(1.f, 2, 0.5, 0.f);
	//lights.push_back(player->playerLight);
	this->hook->hookLight = new Light(DxHandler::devicePtr);
	this->hook->hookLight->lightColor = XMVectorSet(1.f, 249 / 255.f, 10 / 255.f, 0.f);
	sceneManager.addLight(hook->hookLight);
	sceneManager.addLight(player->playerLight);

	Mesh* rope = new Mesh(DxHandler::devicePtr); //Rope
	rope->readMeshFromFile("./Models/actualCube.obj");
	rope->setRotation(DirectX::XMFLOAT3(0, 0, 8));
	this->rope = new Tool(&inputHandler);
	this->rope->model = rope;
	rope->initRigidbody(dynamicsWorld, &collisionShapes, 5);
	this->sceneManager.addMesh(rope);

	Mesh* fishingRodObject = new Mesh(DxHandler::devicePtr); // fishing rod
	fishingRodObject->readMeshFromFile("./Models/rod.obj");
	//fishingRodObject->readMeshFromFile("./Models/actualCube.obj");
	fishingRodObject->setScaling(DirectX::XMFLOAT3(1, 1, 1));
	this->fishingRod = new Tool(&inputHandler);
	this->fishingRod->model = fishingRodObject;
	collisionStruct* fishingRodCollStruct = new collisionStruct(fishingRod, collisionEnums::Rod);
	fishingRodObject->initRigidbody(dynamicsWorld, &collisionShapes, 1, new btBoxShape(btVector3(4, 8, 4)));
	fishingRodObject->rigidBody->setUserPointer(fishingRodCollStruct);
	this->fishingRod->model->rigidBody->setGravity(btVector3(0, 0, 0));
	this->sceneManager.addMesh(fishingRodObject);


	//Mesh* block = new Mesh(DxHandler::devicePtr); // fishing rod
	////fishingRodObject->readMeshFromFile("./Models/rod.obj");
	//block->readMeshFromFile("./Models/actualCube.obj");
	//block->setScaling(DirectX::XMFLOAT3(5, 5, 5));
	//sceneManager.addMesh(block);

	AnimatedMesh* groundObject5= new AnimatedMesh(DxHandler::devicePtr); //Ground
	//groundObject5->readNormalMapFromFile(L"./Models/TegelNormMap.png");
	groundObject5->setTranslation(DirectX::XMFLOAT3(20, 15, 75));
	groundObject5->setScaling(DirectX::XMFLOAT3(30, 30, 30));
	groundObject5->setRotation(DirectX::XMFLOAT3(0, 3.14/2, 0));
	std::vector<Vertex> jellyFish0 = ObjParser::readFromObj("./Models/JellyAnimPartOne.obj");
	std::vector<Vertex> jellyFish1 = ObjParser::readFromObj("./Models/JellyAnimPartTwo.obj");
	std::vector<Vertex> jellyFish2 = ObjParser::readFromObj("./Models/JellyAnimPartThree.obj");
	std::vector<Vertex> jellyFish3 = ObjParser::readFromObj("./Models/JellyAnimPartFour.obj");
	std::vector<Vertex>* fishArr[] = { &jellyFish0, &jellyFish1, &jellyFish2, &jellyFish3 };
	groundObject5->appendStructuredBuffer(fishArr, 4);
	groundObject5->createStructuredBuffer(DxHandler::devicePtr);
	groundObject5->targetPoseIndex = 1;
	groundObject5->animationSpeed = 0.3;
	sceneManager.addAnimatedMesh(groundObject5);

	Skybox::loadSkybox(DxHandler::devicePtr);
	Skybox::sphereModel->setTranslation(XMFLOAT3(1, 50, 4));
	Skybox::sphereModel->setScaling(XMFLOAT3(3000, 3000, 3000));
	Skybox::sphereModel->isSky = true;

	Light* light = new Light(DxHandler::devicePtr);
	light->setPosition(XMFLOAT3(0, 0, -10));
	sceneManager.addLight(light);

	level = new Level();
	level->createLevel(dynamicsWorld, collisionShapes, &sceneManager);

	guiHandler->initMainMenu();
	guiHandler->initHUD();

	//---------------------------------------------------------------------------------------------------
	std::chrono::high_resolution_clock::time_point newTime = std::chrono::high_resolution_clock::now(); //Set new time
	std::chrono::duration<double> frameTime = std::chrono::duration_cast<std::chrono::duration<double>>(newTime - currentTime); //Get deltaTime for frame

	MSG msg;
	std::vector<Vertex> vertVector = ObjParser::readFromObj("./Models/AnglerOne.obj");
	std::vector<Vertex> vertVector1 = ObjParser::readFromObj("./Models/AnglerTwo.obj");
	std::vector<Vertex> vertVector2 = ObjParser::readFromObj("./Models/AnglerThree.obj");
	std::vector<Vertex> vertVector3 = ObjParser::readFromObj("./Models/AnglerFour.obj");
	AnimatedMesh* animMesh = new AnimatedMesh(DxHandler::devicePtr);
	animMesh->readTextureFromFile(L"./Models/ANGLAColor.png");

	std::vector<Vertex>* arr[] = { &vertVector, &vertVector1, &vertVector2, &vertVector3 };
	animMesh->appendStructuredBuffer(arr, 4);
	animMesh->createStructuredBuffer(DxHandler::devicePtr);
	animMesh->setTranslation(XMFLOAT3(-25, -20, 10));
	animMesh->setScaling(XMFLOAT3(10, 10, 10));
	animMesh->setRotation(XMFLOAT3(0, -3.14/2, 0));
	animMesh->targetPoseIndex = 1;
	sceneManager.addAnimatedMesh(animMesh);

	std::uniform_real_distribution<> randomNumPlacement(-1.f, 1.f);
	std::uniform_real_distribution<> randomNumVelocity(0.1f, 2.f);
	std::random_device randomSeed;
	std::mt19937 numberGenerator(randomSeed());

	for (int i = 0; i < 10; i++)
	{
		Mesh* backgroundFish1 = new Mesh(DxHandler::devicePtr);
		backgroundFish1->readMeshFromFile("./Models/FishRight.obj");
		backgroundFish1->setScaling(XMFLOAT3(16, 16, 16));
		backgroundFish1->setTranslation(XMFLOAT3(randomNumPlacement(numberGenerator)*1000, backgroundFishVec.size() * 50, 1200* randomNumVelocity(numberGenerator)));
		backgroundFish1->setRotation(XMFLOAT3(0, 3.14 / 2.f, 0));

		backgroundItems backgroundFishStruct;
		backgroundFishStruct.model = backgroundFish1;
		backgroundFishStruct.velocity = XMFLOAT3(randomNumVelocity(numberGenerator), 0, 0);
		
		backgroundFishVec.push_back(backgroundFishStruct);
		sceneManager.addMesh(backgroundFish1);
	}

	scoreHandle.readFromFile("score.txt");

	startedGameTimer = std::chrono::high_resolution_clock::now();
	while (!shutdown)
	{
		//platform.updatePlatform();
		for (int i = 0; i < sceneManager.movingPlatforms.size(); i++)
			sceneManager.movingPlatforms.at(i)->updatePlatform();

		directXHandler->contextPtr->RSSetViewports(1, &port);
		inputHandler.handleInput();
		currentTime = std::chrono::high_resolution_clock::now(); //Reset time - always needs to be at top / limitFstd::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastTime) / limitFPS;
		primaryCamera.updateCamera();

		renderFirstPass(&sceneManager.sceneMeshes);


		renderSecondPass();
		renderLightVolumes();
		renderParticles();
		updateGUI();
		renderGUI();

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

		///-----stepsimulation_start-----
		if (!pause)
		{
			for (int i = 0; i < 5; i++)
			{
				dynamicsWorld->stepSimulation(1 / 60.f);
				for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
				{
					//std::cout << "Looping" << j << std::endl;
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
				}
			}
		}


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

		for (int i = 0; i < sceneManager.hearts.size(); i++)
		{
			if (sceneManager.hearts.at(i)->usedUp)
			{
				guiHandler->resetHealth(player->health);
				//dynamicsWorld->removeRigidBody(sceneManager.hearts.at(i)->model->rigidBody);
				//sceneManager.removeHeart(sceneManager.hearts.at(i));
				sceneManager.hearts.at(i)->moveAway();
			}
		}

		for (int i = 0; i < sceneManager.points.size(); i++)
		{
			if (sceneManager.points.at(i)->usedUp)
			{
				//dynamicsWorld->removeRigidBody(sceneManager.points.at(i)->model->rigidBody);
				//sceneManager.removePoint(sceneManager.points.at(i));
				sceneManager.points.at(i)->moveAway();
			}
		}

		if(pause)
		{
			primaryCamera.cameraPosition = DirectX::XMVectorSet(0, -500, 0, 0);
			primaryCamera.cameraTarget = DirectX::XMVectorSet(0, -500, 10, 0);
			guiHandler->hideHUD();
		}

		directXHandler->spriteBatch->Begin();

		if (gameOver)
		{

			std::wstring string1 = L"Highscore:\n ";
			directXHandler->spriteFont->DrawString(directXHandler->spriteBatch.get(), string1.data(), DirectX::XMFLOAT2(0, 300), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(0.5f, 0.5f));
			int amountOfLinesToRead = 10;
			int space = 0;
			int placement = 1;
			for (int i = scoreHandle.scores.size() - 1; i > 0; i--)
			{
				space += 50.0f;
				std::wstring placeScore = std::to_wstring(placement++) + L": " + std::to_wstring(scoreHandle.scores.at(i)) + L"\n";
				directXHandler->spriteFont->DrawString(directXHandler->spriteBatch.get(), placeScore.data(), DirectX::XMFLOAT2(0, 300 + space), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(0.5f, 0.5f));
			}

		}

		if (!pause)
		{
			player->gameTime += frameTime.count();

			int seconds = (int)fmod(player->gameTime, 60);
			int minutes = (int)(player->gameTime / 60);

			std::wstring timerText;
			if (seconds > 9)
				timerText = std::to_wstring(minutes) + L":" + std::to_wstring(seconds) + L"\n";// + std::to_wstring((int)this->player->points) + L" points";
			else
				timerText = std::to_wstring(minutes) + L":0" + std::to_wstring(seconds) + L"\n";// + std::to_wstring((int)this->player->points) + L" points";

			directXHandler->spriteFont->DrawString(directXHandler->spriteBatch.get(), timerText.data(), DirectX::XMFLOAT2((WIDTH/2)-50, 5), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));
		
			std::wstring pointCounter;
			pointCounter = std::to_wstring((int)scoreHandle.scoreMultiplier(this->player->points, player->gameTime)) + L" points";
			directXHandler->spriteFont->DrawString(directXHandler->spriteBatch.get(), pointCounter.data(), DirectX::XMFLOAT2(5, 150), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(0.5f, 0.5f));


		}

		directXHandler->spriteBatch->End();

		DxHandler::swapChainPtr->Present(1, 0);
		DxHandler::contextPtr->ClearState();

		directXHandler->setDefaultState();

		//block->setTranslation(XMFLOAT3(platformMesh->rigidBody->getWorldTransform().getOrigin().x(), platformMesh->rigidBody->getWorldTransform().getOrigin().y(), platformMesh->rigidBody->getWorldTransform().getOrigin().z()));
		//block->setTranslation(XMFLOAT3(platformMesh->rigidBody->getWorldTransform().getOrigin().x(), platformMesh->rigidBody->getWorldTransform().getOrigin().y(), platformMesh->rigidBody->getWorldTransform().getOrigin().z()));


		updateParticles();
		newTime = std::chrono::high_resolution_clock::now(); //Set new time
		frameTime = std::chrono::duration_cast<std::chrono::duration<double>>(newTime - currentTime); //Get deltaTime for frame
		if (!pause)
		{
			fixedUpdate(frameTime.count(), dynamicsWorld);
		}
	}
	//Begin cleanup
	YSE::System().close();

	//Clean assetmanager memory
	auto it = AssetLoader::meshDictionary->begin();
	for (std::pair<std::string, AssetReturnStruct*> element : *AssetLoader::meshDictionary)
	{
		AssetReturnStruct* returnVal = element.second;
		returnVal->buffer->Release();
	}
	AssetLoader::meshDictionary->clear();
	//

	//CLEAN UP PHYSICS
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
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

	for (int i = 0; i < collisionShapes.size(); i++)
	{
		btCollisionShape* shape = collisionShapes[i];
		collisionShapes[i] = 0;
		delete shape;
	}

	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;
	// physics clean up complete
}

void Engine::generateRandomBubbles()
{
	std::random_device randomSeed;
	std::mt19937 numberGenerator(randomSeed());
	std::uniform_real_distribution<> randomNum(-0.5f, 0.5f); //Between -1 - 1
	std::uniform_real_distribution<> randomNumPlus(0.2f, 1.f); //Between 0.8 - 1
	std::uniform_real_distribution<> randomNumPlacement(-100.f, 100.f); //Between 0.8 - 1

	//int gameTime = (int)(player->gameTime);
	//if ((gameTime % 5) == 0)
	//{
		//for (int i = 0; i < 5; i++)
		//{
			Particle* particlePtr = new Particle(DxHandler::devicePtr);
			//particlePtr->readTextureFromFile(L"./Textures/bubble.png");
			particlePtr->vertexBuffer = particleMesh->vertexBuffer;
			particlePtr->textureView = particleMesh->textureView;
			particlePtr->hasTexture = particleMesh->hasTexture;
			particlePtr->nrOfVertices = particleMesh->nrOfVertices;
			particlePtr->ticksLeft = 400.f;


			float randomNumber = (float)((rand() % 30) + 5) / 10;
			particlePtr->setScaling(DirectX::XMFLOAT3(randomNumber, randomNumber, randomNumber));
			particlePtr->orgSize = particlePtr->getScaling();
			particlePtr->setTranslation(DirectX::XMFLOAT3(player->model->getTranslation().x + randomNumPlacement(numberGenerator), player->model->getTranslation().y - 120, (randomNum(numberGenerator)* randomNum(numberGenerator))*10.f));

			particlePtr->velocity = XMFLOAT3(0, randomNumPlus(numberGenerator) * 0.7, 0);
			sceneManager.addParticle(particlePtr);
		//}
	//}
}

void Engine::createGUIHandler()
{
	this->guiHandler = new GUIhandler(this->directXHandler, &this->inputHandler);
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
		//if (abs(model->getTranslation().x - player->model->getTranslation().x) < 100)
			directXHandler->draw(model, primaryCamera);
	}

	DxHandler::contextPtr->GSSetShader(NULL, NULL, NULL);
	//animatedMeshes.at(0)->setRotation(rot);
	directXHandler->animVertex->useThis(DxHandler::contextPtr); //Animation vertex shader
	//for (auto animMesh : sceneManager.animatedMeshes) //Draw all meshes
	for (int i = 0; i < sceneManager.animatedMeshes.size(); i++)
	{
		AnimatedMesh* animMesh = sceneManager.animatedMeshes.at(i);

		if (abs(animMesh->getTranslation().x - player->model->getTranslation().x) < 300)
		{

			ID3D11UnorderedAccessView* views[] = { animMesh->vertexStateUAV };
			DxHandler::contextPtr->OMSetRenderTargetsAndUnorderedAccessViews(3, arr, DxHandler::depthStencil, 4, ARRAYSIZE(views), views, nullptr);

			if (!pause)
			{
				if (!animMesh->manualUpdate && abs(animMesh->getTranslation().x - player->model->getTranslation().x) < 300)
				{
					animMesh->stepAnim(1 / 60.00);
					//std::cout << "Anim " << animMesh->t << std::endl;
				}
			}

			directXHandler->draw(animMesh, primaryCamera);
		}
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

void Engine::renderGUI()
{
	float blendingFactor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };

	DxHandler::contextPtr->OMSetBlendState(DxHandler::alphaBlendState, blendingFactor, 0xFFFFFFFF);
	DxHandler::GuiShaderPixel->useThis(DxHandler::contextPtr);
	DxHandler::GuiShaderVertex->useThis(DxHandler::contextPtr);
	directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL);//, DxHandler::depthStencil); //Application screen
	DxHandler::contextPtr->PSSetShaderResources(0, 1, &guiHandler->guiElements.at(0)->mesh->textureView);
	guiHandler->drawGuiElements(this->primaryCamera);
	DxHandler::contextPtr->OMSetBlendState(NULL, NULL, NULL);
}

void Engine::renderLightVolumes()
{
	DxHandler::firstPassVertex->useThis(DxHandler::contextPtr);
	DxHandler::lightPixel->useThis(DxHandler::contextPtr);

	float blendingFactor[4] = { 0.5f, 0.5f, 0.5f, 0.0f };
	//DxHandler::contextPtr->OMSetBlendState(states->Additive(), blendingFactor, 0xFFFFFFFF);
	DxHandler::contextPtr->OMSetBlendState(DxHandler::additiveBlendState, blendingFactor, 0xFFFFFFFF);
	for (int i = 0; i < sceneManager.lights.size(); i++) //Draw all lights
	{
		Light* model = sceneManager.lights.at(i);
		if (model != nullptr)
			directXHandler->draw(model->lightVolume, primaryCamera, false, model);
		else
			std::cout << "Light was nullptr" << std::endl;
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
	directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, DxHandler::depthStencil);//, DxHandler::depthStencil); //Application screen
	//DxHandler::contextPtr->GSSetShader(NULL, NULL, NULL);
	for (Particle* model : sceneManager.particles) //Draw transparent stuff
	{
		DxHandler::contextPtr->PSSetShaderResources(0, 1, &model->textureView);
		directXHandler->draw(model, primaryCamera, false);
	}
	//DxHandler::backfaceCullShader->useThis(DxHandler::contextPtr);
	DxHandler::contextPtr->OMSetBlendState(NULL, NULL, NULL);

}
