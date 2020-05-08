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
	string fileName = "./Levels/Level.txt";
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
	std::cout << degrees.x << "," << degrees.y << "," << degrees.z << std::endl;
	return degrees;
}

void Level::createLevel(btDiscreteDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*> collisionShapes, vector<Mesh*>& scene, vector<Enemy*>& enemies, vector<Light*>& lights)
{
	Level* level = new Level();
	level->readFromeFile(level->levelMeshVector);
	DirectX::XMFLOAT3 multi(10, 10, 10);

	//skapa en funktion som förenklar skapande av objekt senare

	for (size_t i = 0; i < level->levelMeshVector.size(); i++)
	{

		if (level->levelMeshVector.at(i).tag == "platform")
		{
			Mesh* groundObject = new Mesh(DxHandler::devicePtr);
			groundObject->readMeshFromFID("./Models/JellyFish.FID");
			groundObject->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
			groundObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
			groundObject->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));

			groundObject->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));

			groundObject->initRigidbody(dynamicsWorld, &collisionShapes, 0);
			scene.push_back(groundObject);
			DirectX::XMFLOAT3 test = level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi);
			std::cout << test.x << " " << test.y << " " << test.z << std::endl;
		}
		else if (level->levelMeshVector.at(i).tag == "background")
		{
			Mesh* background = new Mesh(DxHandler::devicePtr);
			background->readMeshFromFID("./Models/berg.FID");
			background->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
			background->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
			background->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));

			scene.push_back(background);
			cout << endl;

		}
		else if (level->levelMeshVector.at(i).tag == "enemy")
		{
			Enemy* enemy = new Enemy(DxHandler::devicePtr);
			enemy->model->setTranslation(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getTranslation(), multi));
			enemy->model->setRotation(level->degreesToRadians(level->levelMeshVector.at(i).getRotation()));
			enemy->model->setScaling(level->multiplyFloat3XYZ(level->levelMeshVector.at(i).getScale(), multi));
			enemies.push_back(enemy);
			scene.push_back(enemy->model);
			lights.push_back(enemy->light);

		}
		else if (level->levelMeshVector.at(i).tag == "character")
		{

		}
	}
}

