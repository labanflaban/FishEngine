#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <DirectXMath.h>

using namespace std;

struct LevelMesh
{
	string name;
	DirectX::XMFLOAT3 translation;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
	bool backGroundCheck;
	string tag;

	DirectX::XMFLOAT3 getTranslation();
	DirectX::XMFLOAT3 getRotation();
	DirectX::XMFLOAT3 getScale();

	friend ostream& operator << (ostream& outPut, LevelMesh& level);

	friend istream& operator >> (istream& input, LevelMesh& level);
};

struct Level
{
	vector<LevelMesh> levelMeshVector;
	void readFromeFile(vector<LevelMesh>& levelMeshVector);
	void printVector(vector<LevelMesh>& levelMeshVector);

};
