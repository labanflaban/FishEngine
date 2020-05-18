#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <DirectXMath.h>
<<<<<<< Updated upstream
=======
#include "Enemy.h"
#include "Mesh.h"
#include "DxHandler.h"
#include "SceneManager.h"
#include "CollisionStruct.h"

#define BT_NO_SIMD_OPERATOR_OVERLOADS
#include "btBulletDynamicsCommon.h"

>>>>>>> Stashed changes

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
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
};

struct Level
{
	vector<LevelMesh> levelMeshVector;
	void readFromeFile(vector<LevelMesh>& levelMeshVector);
	void printVector(vector<LevelMesh>& levelMeshVector);
<<<<<<< Updated upstream
=======
	DirectX::XMFLOAT3 multiplyFloat3XYZ(DirectX::XMFLOAT3 first, DirectX::XMFLOAT3 second);
	DirectX::XMFLOAT3 degreesToRadians(DirectX::XMFLOAT3 degrees);

	void createLevel(btDiscreteDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*> collisionShapes, SceneManager* sceneManager);

>>>>>>> Stashed changes

};
