#include "Level.h"

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

void Level::createLevel(btDiscreteDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*> collisionShapes, SceneManager* sceneManager)
{
	Level* level = new Level();
	level->readFromeFile(level->levelMeshVector);
	DirectX::XMFLOAT3 multi(10, 10, 10);

	for (size_t i = 0; i < level->levelMeshVector.size(); i++)
	{

		if (level->levelMeshVector.at(i).tag == "platform")
		{
			AnimatedMesh* groundObject = new AnimatedMesh(DxHandler::devicePtr);
			//groundObject->readMeshFromFID("./Models/JellyFish.FID");
			std::vector<Vertex> jellyFish0 = ObjParser::readFromObj("./Models/JellyAnimPartOne.obj");
			std::vector<Vertex> jellyFish1 = ObjParser::readFromObj("./Models/JellyAnimPartTwo.obj");
			std::vector<Vertex> jellyFish2 = ObjParser::readFromObj("./Models/JellyAnimPartThree.obj");
			std::vector<Vertex> jellyFish3 = ObjParser::readFromObj("./Models/JellyAnimPartFour.obj");
			std::vector<Vertex>* fishArr[] = { &jellyFish0, &jellyFish1, &jellyFish2, &jellyFish3 };
			groundObject->appendStructuredBuffer(fishArr, 4);
			groundObject->createStructuredBuffer(DxHandler::devicePtr);
			groundObject->targetPoseIndex = 1;
			groundObject->animationSpeed = 0.3;

			groundObject->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
			groundObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));

			XMFLOAT3 jellyScale = level->levelMeshVector.at(i).getScale();
			groundObject->setScaling(level->multiplyFloat3XYZ(XMFLOAT3(jellyScale.x*0.75, jellyScale.y*0.5, jellyScale.z*0.75), multi));
			groundObject->initRigidbody(dynamicsWorld, &collisionShapes, 0);
			sceneManager->addAnimatedMesh(groundObject);

			Light* light = new Light(DxHandler::devicePtr);
			light->lightColor = XMVectorSet(1, 1, 0, 0);
			light->setPosition(XMFLOAT3(groundObject->getTranslation().x, groundObject->getTranslation().y+25, groundObject->getTranslation().z));
			sceneManager->addLight(light);
			//sceneManager->addMesh(groundObject);

		}
		else if (level->levelMeshVector.at(i).tag == "background")
		{
			Mesh* background = new Mesh(DxHandler::devicePtr);
			background->readMeshFromFID("./Models/berg.FID");
			background->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
			background->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
			background->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));

			sceneManager->addMesh(background);

		}
		else if (level->levelMeshVector.at(i).tag == "enemyTwo")
		{

			Enemy* enemy = new Enemy(DxHandler::devicePtr); //Instantiate enemy
			sceneManager->addEnemy(enemy);

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

			sceneManager->addAnimatedMesh(enemy->model);
			sceneManager->addLight(enemy->light);

			collisionStruct* enemy1CollStruct = new collisionStruct(enemy, collisionEnums::Enemy);
			enemy->model->initRigidbody(dynamicsWorld, &collisionShapes, 20, new btBoxShape(btVector3(btScalar(enemy->model->getScaling().x + 5), btScalar(enemy->model->getScaling().y + 5), btScalar(enemy->model->getScaling().z) + 5)));
			enemy->model->targetPoseIndex = 1;
			enemy->model->rigidBody->setUserPointer(enemy1CollStruct);
			enemy->model->rigidBody->setActivationState(ACTIVE_TAG);
			enemy->model->rigidBody->setGravity(btVector3(0, 0, 0));

		}
		else if (level->levelMeshVector.at(i).tag == "enemy")
		{
			Enemy* enemy = new Enemy(DxHandler::devicePtr); //Instantiate enemy
			sceneManager->addEnemy(enemy);


			enemy->model = new AnimatedMesh(DxHandler::devicePtr);
			std::vector<Vertex> vertVector = ObjParser::readFromObj("./Models/AnglerOne.obj");
			std::vector<Vertex> vertVector1 = ObjParser::readFromObj("./Models/AnglerTwo.obj");
			std::vector<Vertex> vertVector2 = ObjParser::readFromObj("./Models/AnglerThree.obj");
			std::vector<Vertex> vertVector3 = ObjParser::readFromObj("./Models/AnglerFour.obj");
			enemy->model->readTextureFromFile(L"./Models/ANGLAColor.png");

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

			collisionStruct* enemy1CollStruct = new collisionStruct(enemy, collisionEnums::Enemy);
			enemy->model->initRigidbody(dynamicsWorld, &collisionShapes, 20, new btBoxShape(btVector3(btScalar(enemy->model->getScaling().x + 5), btScalar(enemy->model->getScaling().y + 5), btScalar(enemy->model->getScaling().z) + 5)));
			enemy->model->targetPoseIndex = 1;
			enemy->model->rigidBody->setUserPointer(enemy1CollStruct);
			enemy->model->rigidBody->setActivationState(ACTIVE_TAG);
			enemy->model->rigidBody->setGravity(btVector3(0, 0, 0));
		}
		else if (level->levelMeshVector.at(i).tag == "GroundOne")
		{
			Mesh* groundObject = new Mesh(DxHandler::devicePtr);
			groundObject->readMeshFromFile("./Models/GroundOne.Obj");

			groundObject->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
			groundObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
			groundObject->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
			groundObject->readNormalMapFromFile(L"./Textures/StoneNormal.png");

			btBoxShape* box = new btBoxShape(btVector3(btScalar(groundObject->getScaling().x * 3.2f), btScalar(groundObject->getScaling().y * 0.1), btScalar(groundObject->getScaling().z) + 6));
			groundObject->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);

			sceneManager->addMesh(groundObject);
		}
		else if (level->levelMeshVector.at(i).tag == "GroundTwo")
		{
			Mesh* groundObject = new Mesh(DxHandler::devicePtr);
			groundObject->readMeshFromFile("./Models/GroundTwo.Obj");

			groundObject->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
			groundObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
			groundObject->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
			groundObject->readNormalMapFromFile(L"./Textures/StoneNormal.png");

			btBoxShape* box = new btBoxShape(btVector3(btScalar(groundObject->getScaling().x * 3.2f), btScalar(groundObject->getScaling().y * 0.1), btScalar(groundObject->getScaling().z) + 6));
			groundObject->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);

			sceneManager->addMesh(groundObject);
		}
		else if (level->levelMeshVector.at(i).tag == "GroundThree")
		{
			Mesh* groundObject = new Mesh(DxHandler::devicePtr);
			groundObject->readMeshFromFile("./Models/GroundThree.Obj");

			groundObject->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
			groundObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
			groundObject->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
			groundObject->readNormalMapFromFile(L"./Textures/StoneNormal.png");


			btBoxShape* box = new btBoxShape(btVector3(btScalar(groundObject->getScaling().x * 3.2f), btScalar(groundObject->getScaling().y * 0.1), btScalar(groundObject->getScaling().z) + 6));
			groundObject->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);
			sceneManager->addMesh(groundObject);
		}
		else if (level->levelMeshVector.at(i).tag == "Rock")
		{
			Mesh* rockObject = new Mesh(DxHandler::devicePtr);
			rockObject->readMeshFromFile("./Models/Rock.Obj");

			rockObject->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
			rockObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
			rockObject->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
			rockObject->readNormalMapFromFile(L"./Textures/StoneNormal.png");

			btBoxShape* box = new btBoxShape(btVector3(btScalar(rockObject->getScaling().x * 3.2f), btScalar(rockObject->getScaling().y * 0.1), btScalar(rockObject->getScaling().z) + 6));
			rockObject->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);
			sceneManager->addMesh(rockObject);
		}
		else if (level->levelMeshVector.at(i).tag == "light")
		{
			std::cout << "MAKING LIGHT" << std::endl;
			Light* lightObject = new Light(DxHandler::devicePtr);
			lightObject->setPosition(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
			lightObject->lightColor = XMVectorSet(5, 5, 5, 0);
			sceneManager->addLight(lightObject);
		}
		else if (level->levelMeshVector.at(i).tag == "respawn")
		{
			respawn = level->levelMeshVector.at(i).translation.y;
		}
		else if (level->levelMeshVector.at(i).tag == "goal")
		{
			goal = level->levelMeshVector.at(i).translation.x; 
		}
		else if (level->levelMeshVector.at(i).tag == "heart")
		{
			Heartdrop* drop = new Heartdrop;
			
			Mesh* model = new Mesh(DxHandler::devicePtr);
			model->vertexBuffer = masterHeart->vertexBuffer;
			model->nrOfVertices = masterHeart->nrOfVertices;
			model->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
			model->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
			model->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
			model->setScaling(XMFLOAT3(5, 5, 5));

			drop->model = model; 
			collisionStruct* dropCollStruct = new collisionStruct(drop, collisionEnums::Heart);
			btBoxShape* box = new btBoxShape(btVector3(btScalar(model->getScaling().x), btScalar(model->getScaling().y), btScalar(model->getScaling().z)));
			model->initRigidbody(dynamicsWorld, &collisionShapes, 0, box);
			model->rigidBody->setUserPointer(dropCollStruct);

			sceneManager->addMesh(model);
			sceneManager->addHeart(drop);
		}

	}
}

Level::Level()
{
	masterHeart = new Mesh(DxHandler::devicePtr);
	masterHeart->readMeshFromFile("./Models/actualCube.obj");
	
}

