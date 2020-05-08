#pragma once
#include <vector>
#include <DirectXHelpers.h>
#include <SimpleMath.h>
#include <string>
#include <WICTextureLoader.h>
#include "ObjParser.h"
#include "Vertex.h"
#include "FIDParser.h"
#define BT_NO_SIMD_OPERATOR_OVERLOADS
#include "btBulletDynamicsCommon.h"
#include "Keyframes.h"

//const int FLOATS_PER_VERTEX = 15;
/*struct Vertex
{
	float x, y, z = 0;
	float r, g, b, a = 1; //Default to white for debug
	float u, v = 0;
	float nx, ny, nz = 0;
	float tx, ty, tz = 0;
};*/

//class ObjParser;

class AnimatedMesh : public Keyframes
{
private:
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX scalingMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 translation;
	DirectX::XMFLOAT3 scaling;

	ObjParser parser;
	FIDParser fidParser;

	ID3D11Resource* texture;
	ID3D11Texture2D* pTexture = NULL;
	D3D11_SAMPLER_DESC textureSamplerDesc;
	D3D11_TEXTURE2D_DESC  imageSampleDesc = { 0 };

	void updateWorldMatrix();
public:
	ID3D11Device* device = nullptr;

	DirectX::XMFLOAT4 ambientMeshColor = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 diffuseMeshColor = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 specularMeshColor = DirectX::XMFLOAT4(1, 1, 1, 1);

	btRigidBody* rigidBody = nullptr;
	btCollisionShape* collider = nullptr;
	ID3D11ShaderResourceView* textureView = nullptr;
	ID3D11Buffer* vertexBuffer = NULL;
	//std::vector<Vertex> vertices;
	bool hasTexture = false;

	DirectX::XMFLOAT3 getRotation();
	DirectX::XMFLOAT3 getTranslation();
	DirectX::XMFLOAT3 getScaling();
	DirectX::XMMATRIX& getWorldMatrix();

	void setRotation(DirectX::XMFLOAT3 rotation);
	void setTranslation(DirectX::XMFLOAT3 translation);
	void setScaling(DirectX::XMFLOAT3 scaling);
	void move(DirectX::XMFLOAT3 deltaTranslation);
	void initRigidbody(btDiscreteDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*>* collisionShapes, float mass);
	void readMeshFromFID(std::string fileName);
	void readMeshFromFile(std::string fileName);
	void readTextureFromFile(std::wstring textureName); //No need for each instance to hold this function

	AnimatedMesh(ID3D11Device* device);
	~AnimatedMesh();
};