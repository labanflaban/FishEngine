#include "Level.h"
Mesh* Level::masterHeart;// = new Mesh(DxHandler::devicePtr);
Mesh* Level::masterPoint;// = new Mesh(DxHandler::devicePtr);
float Level::goal = 0;
float Level::respawn = 0;
std::mutex Level::vectorLock;

DirectX::XMFLOAT3 LevelMesh::getTranslation()
{
	return translation;
}

DirectX::XMFLOAT3 LevelMesh::getRotation()
{
	return rotation;
}



DirectX::XMFLOAT3 LevelMesh::getScale()
{
	return scale;
}

ostream& operator<<(ostream& outPut, LevelMesh& level)
{
	outPut << level.name << ": " << level.translation.x << "," << level.translation.y << "," << level.translation.z << ","
		<< level.rotation.x << "," << level.rotation.y << "," << level.rotation.z << ","
		<< level.scale.x << "," << level.scale.y << "," << level.scale.z << "," << level.backGroundCheck << "," << level.tag;
	return outPut;
}

istream& operator>>(istream& input, LevelMesh& level)
{
	input >> level.name >> level.translation.x >> level.translation.y >> level.translation.z
		>> level.rotation.x >> level.rotation.y >> level.rotation.z
		>> level.scale.x >> level.scale.y >> level.scale.z >> level.backGroundCheck >> level.tag;
	return input;
}

void Level::readFromeFile(vector<LevelMesh>& levelMeshVector)
{
	string fileName = "./Levels/Level.level";
	ifstream inFile;
	LevelMesh* levelMesh = new LevelMesh();

	inFile.open(fileName, ios::in);

	if (inFile.is_open() == false)
	{
		cout << "file is not open" << endl;
	}
	else
	{
		while (inFile >> *levelMesh)
		{
			levelMeshVector.push_back(*levelMesh);
			//cout << *levelMesh << endl;
		}
	}

	inFile.close();
	delete levelMesh;
}

void Level::printVector(vector<LevelMesh>& levelMeshVector)
{
	for (size_t i = 0; i < levelMeshVector.size(); i++)
	{
		cout << levelMeshVector.at(i) << endl;
	}
}

DirectX::XMFLOAT3 Level::multiplyFloat3XYZ(DirectX::XMFLOAT3 first, DirectX::XMFLOAT3 second)
{
	DirectX::XMFLOAT3 mul(1, 1, 1);
	mul.x = first.x * second.x;
	mul.y = first.y * second.y;
	mul.z = first.z * second.z;
	return mul;
}

DirectX::XMFLOAT3 Level::degreesToRadians(DirectX::XMFLOAT3 degrees)
{
	degrees.x = (degrees.x / 180) * DirectX::XM_PI;
	degrees.y = (degrees.y / 180) * DirectX::XM_PI;
	degrees.z = (degrees.z / 180) * DirectX::XM_PI;
	//std::cout << degrees.x << "," << degrees.y << "," << degrees.z << std::endl;
	return degrees;
}

DirectX::XMFLOAT3 multi(10, 10, 10);
void Level::loadTag(string tag, btDiscreteDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*> collisionShapes, SceneManager* sceneManager, Level* level, int i)
{
	if (tag == "platform")
	{
		AnimatedMesh* groundObject = new AnimatedMesh(DxHandler::devicePtr);
		//groundObject->readMeshFromFID("./Models/JellyFish.FID");
		std::vector<Vertex> jellyFish0 = ObjParser::readFromObj("./Models/JellyAnimPartOne.obj");
		//std::vector<Vertex> jellyFish1 = ObjParser::readFromObj("./Models/JellyAnimPartTwo.obj");
		//std::vector<Vertex> jellyFish2 = ObjParser::readFromObj("./Models/JellyAnimPartThree.obj");
		std::vector<Vertex> jellyFish3 = ObjParser::readFromObj("./Models/JellyAnimPartFour.obj");
		std::vector<Vertex>* fishArr[] = { &jellyFish0, &jellyFish3 };//, &jellyFish2, &jellyFish3 };
		groundObject->appendStructuredBuffer(fishArr, 2);
		groundObject->createStructuredBuffer(DxHandler::devicePtr);
		groundObject->targetPoseIndex = 1;
		groundObject->animationSpeed = 0.3;

		groundObject->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		groundObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));

		XMFLOAT3 jellyScale = level->levelMeshVector.at(i).getScale();
		groundObject->setScaling(level->multiplyFloat3XYZ(XMFLOAT3(jellyScale.x * 0.75, jellyScale.y * 0.5, jellyScale.z * 0.75), multi));

		std::lock_guard<std::mutex> lock(vectorLock);
		sceneManager->addAnimatedMesh(groundObject);

		Light* light = new Light(DxHandler::devicePtr);
		light->lightColor = XMVectorSet(1, 1, 0, 0);
		light->setPosition(XMFLOAT3(groundObject->getTranslation().x, groundObject->getTranslation().y + 25, groundObject->getTranslation().z));

		sceneManager->addLight(light);
		//sceneManager->addMesh(groundObject);
		collisionStruct* jellyCollStruct = new collisionStruct(groundObject, collisionEnums::BounceObject);
		groundObject->initRigidbody(dynamicsWorld, &collisionShapes, 0);
		groundObject->rigidBody->setUserPointer(jellyCollStruct);

	}
	else if (tag == "background")
	{
		Mesh* background = new Mesh(DxHandler::devicePtr);
		background->readMeshFromFID("./Models/berg.FID");
		background->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		background->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
		background->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));

		std::lock_guard<std::mutex> lock(vectorLock);
		sceneManager->addMesh(background);

	}
	else if (tag == "enemyTwo")
	{

		Enemy* enemy = new Enemy(DxHandler::devicePtr); //Instantiate enemy
		

		std::vector<Vertex> target1 = ObjParser::readFromObj("./Models/FishLeft.Obj");
		std::vector<Vertex> target2 = ObjParser::readFromObj("./Models/FishRight.Obj");

		enemy->model = new AnimatedMesh(DxHandler::devicePtr);
		enemy->model->readTextureFromFile(L"./Models/FISHCOLOR.png");
		std::vector<Vertex>* fishArr[] = { &target1, &target2 };
		enemy->model->appendStructuredBuffer(fishArr, 2);
		enemy->model->createStructuredBuffer(DxHandler::devicePtr);
		enemy->model->targetPoseIndex = 1;
		enemy->model->animationSpeed = 0.3;
		enemy->model->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		enemy->model->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
		enemy->model->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
		enemy->model->setScaling(DirectX::XMFLOAT3(5, 5, 5));
		enemy->startPos = level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi);

		std::lock_guard<std::mutex> lock(vectorLock);
		sceneManager->addAnimatedMesh(enemy->model);
		sceneManager->addLight(enemy->light);
		sceneManager->addEnemy(enemy);

		collisionStruct* enemy1CollStruct = new collisionStruct(enemy, collisionEnums::Enemy);
		enemy->model->initRigidbody(dynamicsWorld, &collisionShapes, 20, new btBoxShape(btVector3(btScalar(enemy->model->getScaling().x + 5), btScalar(enemy->model->getScaling().y + 5), btScalar(enemy->model->getScaling().z) + 5)));
		enemy->model->targetPoseIndex = 1;
		enemy->model->rigidBody->setUserPointer(enemy1CollStruct);
		enemy->model->rigidBody->setActivationState(ACTIVE_TAG);
		enemy->model->rigidBody->setGravity(btVector3(0, 0, 0));

	}
	else if (tag == "enemy")
	{
		Enemy* enemy = new Enemy(DxHandler::devicePtr); //Instantiate enemy


		enemy->model = new AnimatedMesh(DxHandler::devicePtr);
		std::vector<Vertex> vertVector = ObjParser::readFromObj("./Models/AnglerOne.obj");
		std::vector<Vertex> vertVector1 = ObjParser::readFromObj("./Models/AnglerTwo.obj");
		std::vector<Vertex> vertVector2 = ObjParser::readFromObj("./Models/AnglerThree.obj");
		std::vector<Vertex> vertVector3 = ObjParser::readFromObj("./Models/AnglerFour.obj");
		enemy->model->readTextureFromFile(L"./Models/ANGLAColor.png");
		enemy->startPos = level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi);

		std::lock_guard<std::mutex> lock(vectorLock);
		std::vector<Vertex>* fishArr[] = { &vertVector, &vertVector1, &vertVector2, &vertVector3 };

		enemy->model->appendStructuredBuffer(fishArr, 4);
		enemy->model->createStructuredBuffer(DxHandler::devicePtr);
		enemy->model->targetPoseIndex = 1;
		enemy->model->animationSpeed = 2;
		enemy->model->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		enemy->model->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
		enemy->model->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
		enemy->model->setScaling(DirectX::XMFLOAT3(5, 5, 5));

		sceneManager->addAnimatedMesh(enemy->model);
		sceneManager->addLight(enemy->light);
		sceneManager->addEnemy(enemy);

		collisionStruct* enemy1CollStruct = new collisionStruct(enemy, collisionEnums::Enemy);
		enemy->model->initRigidbody(dynamicsWorld, &collisionShapes, 20, new btBoxShape(btVector3(btScalar(enemy->model->getScaling().x + 15), btScalar(enemy->model->getScaling().y + 15), btScalar(enemy->model->getScaling().z) + 25)));
		enemy->model->targetPoseIndex = 1;
		enemy->model->rigidBody->setUserPointer(enemy1CollStruct);
		enemy->model->rigidBody->setActivationState(ACTIVE_TAG);
		enemy->model->rigidBody->setGravity(btVector3(0, 0, 0));
	}
	else if (tag == "GroundOne")
	{
		Mesh* groundObject = new Mesh(DxHandler::devicePtr);
		groundObject->enemyCollIgnore = true;
		groundObject->readMeshFromFile("./Models/GroundOne.Obj");


		groundObject->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		groundObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
		groundObject->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
		groundObject->readNormalMapFromFile(L"./Textures/StoneNormal.png");

		std::lock_guard<std::mutex> lock(vectorLock);
		btBoxShape* box = new btBoxShape(btVector3(btScalar(groundObject->getScaling().x * 3.2f), btScalar(groundObject->getScaling().y * 0.1), btScalar(groundObject->getScaling().z) + 6));
		groundObject->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);

		
		sceneManager->addMesh(groundObject);
	}
	else if (tag == "GroundTwo")
	{
		Mesh* groundObject = new Mesh(DxHandler::devicePtr);
		groundObject->enemyCollIgnore = true;
		groundObject->readMeshFromFile("./Models/GroundTwo.Obj");

		groundObject->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		groundObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
		groundObject->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
		groundObject->readNormalMapFromFile(L"./Textures/StoneNormal.png");

		std::lock_guard<std::mutex> lock(vectorLock);
		btBoxShape* box = new btBoxShape(btVector3(btScalar(groundObject->getScaling().x * 3.2f), btScalar(groundObject->getScaling().y * 0.1), btScalar(groundObject->getScaling().z) + 6));
		groundObject->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);

		sceneManager->addMesh(groundObject);
	}
	else if (tag == "GroundThree")
	{
		Mesh* groundObject = new Mesh(DxHandler::devicePtr);
		groundObject->enemyCollIgnore = true;
		groundObject->readMeshFromFile("./Models/GroundThree.Obj");

		groundObject->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		groundObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
		groundObject->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
		groundObject->readNormalMapFromFile(L"./Textures/StoneNormal.png");


		std::lock_guard<std::mutex> lock(vectorLock);
		btBoxShape* box = new btBoxShape(btVector3(btScalar(groundObject->getScaling().x * 3.2f), btScalar(groundObject->getScaling().y * 0.1), btScalar(groundObject->getScaling().z) + 6));
		groundObject->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);

		
		sceneManager->addMesh(groundObject);
	}
	else if (tag == "Rock")
	{
		Mesh* rockObject = new Mesh(DxHandler::devicePtr);
		rockObject->enemyCollIgnore = true;
		rockObject->readMeshFromFile("./Models/Rock.Obj");

		rockObject->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		rockObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
		rockObject->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
		rockObject->readNormalMapFromFile(L"./Textures/StoneNormal.png");

		std::lock_guard<std::mutex> lock(vectorLock);
		btBoxShape* box = new btBoxShape(btVector3(btScalar(rockObject->getScaling().x * 3.2f), btScalar(rockObject->getScaling().y * 0.1), btScalar(rockObject->getScaling().z) + 6));
		rockObject->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);

		
		sceneManager->addMesh(rockObject);
	}
	else if (tag == "light")
	{
		std::cout << "MAKING LIGHT" << std::endl;
		Light* lightObject = new Light(DxHandler::devicePtr);
		lightObject->setPosition(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		lightObject->lightColor = XMVectorSet(4, 0, 2, 0);

		std::lock_guard<std::mutex> lock(vectorLock);
		sceneManager->addLight(lightObject);
	}
	else if (tag == "respawn")
	{
		respawn = level->levelMeshVector.at(i).translation.y * multi.y;
	}
	else if (tag == "goal")
	{
		Mesh* winObject = new Mesh(DxHandler::devicePtr);
		winObject->enemyCollIgnore = true;
		winObject->readMeshFromFile("./Models/Medallion.Obj");
		winObject->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		winObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
		winObject->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));

		std::lock_guard<std::mutex> lock(vectorLock);
		sceneManager->addMesh(winObject);

		goal = level->levelMeshVector.at(i).translation.x * multi.x;
	}
	else if (tag == "heart")
	{
		Heartdrop* drop = new Heartdrop;

		Mesh* model = new Mesh(DxHandler::devicePtr);
		model->enemyCollIgnore = true;
		model->vertexBuffer = masterHeart->vertexBuffer;
		model->nrOfVertices = masterHeart->nrOfVertices;
		model->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		model->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
		model->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
		model->setScaling(XMFLOAT3(2, 2, 2));
		drop->startPos = level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi);

		drop->model = model;
		collisionStruct* dropCollStruct = new collisionStruct(drop, collisionEnums::Heart);

		std::lock_guard<std::mutex> lock(vectorLock);
		btBoxShape* box = new btBoxShape(btVector3(btScalar(model->getScaling().x), btScalar(model->getScaling().y), btScalar(model->getScaling().z)));
		model->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);
		model->rigidBody->setUserPointer(dropCollStruct);

		
		sceneManager->addTransparentObject(model);
		sceneManager->addHeart(drop);
	}
	else if (tag == "point")
	{
		Pointdrop* drop = new Pointdrop;

		Mesh* model = new Mesh(DxHandler::devicePtr);
		model->enemyCollIgnore = true;
		model->vertexBuffer = masterPoint->vertexBuffer;
		model->nrOfVertices = masterPoint->nrOfVertices;
		model->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		//model->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
		model->setRotation(XMFLOAT3(0, 3.14 / 2, 0));
		model->setScaling(XMFLOAT3(4, 4, 4));
		drop->startPos = level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi);

		drop->model = model;
		collisionStruct* dropCollStruct = new collisionStruct(drop, collisionEnums::Pointdrop);

		std::lock_guard<std::mutex> lock(vectorLock);
		btBoxShape* box = new btBoxShape(btVector3(btScalar(model->getScaling().x), btScalar(model->getScaling().y), btScalar(model->getScaling().z)));
		model->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);
		model->rigidBody->setUserPointer(dropCollStruct);
		sceneManager->addTransparentObject(model);
		sceneManager->addPoint(drop);
	}
	else if (tag == "spike")
	{
		Spike* spike = new Spike();
		Mesh* spikeObject = new Mesh(DxHandler::devicePtr);
		spikeObject->enemyCollIgnore = true;
		spikeObject->readMeshFromFile("./Models/spike.obj");
		spikeObject->readTextureFromFile(L"./Textures/red.png");

		spikeObject->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		spikeObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
		spikeObject->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));

		spike->model = spikeObject;

		std::lock_guard<std::mutex> lock(vectorLock);
		btBoxShape* box = new btBoxShape(btVector3(btScalar(spikeObject->getScaling().x), btScalar(spikeObject->getScaling().y), btScalar(spikeObject->getScaling().z)));
		collisionStruct* spikeColl = new collisionStruct(spike, collisionEnums::Spike);
		spikeObject->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);
		spikeObject->rigidBody->setUserPointer(spikeColl);

		
		sceneManager->addMesh(spikeObject);
		sceneManager->addSpike(spike);
	}
	else if (tag == "movingPlatform")
	{
		MovingPlatform* platform = new MovingPlatform;
		Mesh* platformMesh = new Mesh(DxHandler::devicePtr);
		platformMesh->readMeshFromFile("./Models/GroundThree.Obj");
		platformMesh->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
		platformMesh->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
		
		std::lock_guard<std::mutex> lock(vectorLock);
		btBoxShape* box = new btBoxShape(btVector3(btScalar(platformMesh->getScaling().x * 3.2f), btScalar(platformMesh->getScaling().y * 0.1), btScalar(platformMesh->getScaling().z) + 6));
		platformMesh->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);
		platformMesh->readNormalMapFromFile(L"./Textures/StoneNormal.png");
		platform->startPos = platformMesh->getTranslation();
		platform->platform = platformMesh;
		platform->range = 250;//platformMesh->getScaling().x * 3.2f * 2;

		sceneManager->addMesh(platformMesh);
		sceneManager->addPlatform(platform);
	}

	for (int i = 0; i < sceneManager->enemies.size(); i++)
	{
		for (int j = 0; j < sceneManager->sceneMeshes.size(); j++)
		{
			if (sceneManager->sceneMeshes.at(j)->enemyCollIgnore)
				sceneManager->enemies.at(i)->model->rigidBody->setIgnoreCollisionCheck(sceneManager->sceneMeshes.at(j)->rigidBody, true);
		}
	}
}

void Level::createLevel(btDiscreteDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*> collisionShapes, SceneManager* sceneManager)
{
	Level* level = new Level();
	level->readFromeFile(level->levelMeshVector);

	for (size_t i = 0; i < level->levelMeshVector.size(); i++)
	{
		//futures.push_back(std::async(std::launch::async, loadTag, level->levelMeshVector.at(i).tag, dynamicsWorld, collisionShapes, sceneManager, level, i));
		loadTag(level->levelMeshVector.at(i).tag, dynamicsWorld, collisionShapes, sceneManager, level, i);

	}
	//std::cout << " f " << std::endl;

	//auto val = futures.at(futures.size() - 1).wait_for(std::chrono::seconds(17));
	//futures.at(futures.size() - 1).wait_for(std::chrono::seconds(1));
	/*while ((!futures.at(futures.size() - 1)._Is_ready()))
	{
		Sleep(0.01);
	}*/

}

Level::Level()
{
	masterHeart = new Mesh(DxHandler::devicePtr);
	masterHeart->readMeshFromFile("./Models/heart.obj");
	
	masterPoint = new Mesh(DxHandler::devicePtr);
	masterPoint->readMeshFromFile("./Models/coin.obj");
}

