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
		>> level.scale.x>> level.scale.y>> level.scale.z >> level.backGroundCheck >> level.tag;
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
