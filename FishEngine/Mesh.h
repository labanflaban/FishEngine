
#pragma once
#include <vector>
#include <DirectXHelpers.h>
#include <SimpleMath.h>
#include <string>
#include <WICTextureLoader.h>
#include "DxHandler.h"
#include "ObjParser.h"
#include "Vertex.h"

#define BT_NO_SIMD_OPERATOR_OVERLOADS
#include "bullet\btBulletDynamicsCommon.h"

/*struct Vertex
{
	float x, y, z = 0;
	float r, g, b, a = 1; //Default to white for debug
	float u, v = 0;
	float nx, ny, nz = 0;
	float tx, ty, tz = 0;
};*/

class ObjParser;

class Mesh
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

	ID3D11Resource* texture;
	ID3D11Texture2D* pTexture = NULL;
	D3D11_SAMPLER_DESC textureSamplerDesc;
	D3D11_TEXTURE2D_DESC  imageSampleDesc = { 0 };

	void updateWorldMatrix();
public:
	DirectX::XMFLOAT4 ambientMeshColor = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 diffuseMeshColor = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 specularMeshColor = DirectX::XMFLOAT4(1, 1, 1, 1);
	float shininess = 1.f;
	float specularExponent = 10.f;

	btRigidBody* rigidBody = nullptr;
	btCollisionShape* collider = nullptr;
	void initRigidbody(btDiscreteDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*>* collisionShapes, float mass);

	ID3D11ShaderResourceView* textureView = nullptr;

	ID3D11Buffer* vertexBuffer = NULL;
	std::vector<Vertex> vertices;

	void setRotation(DirectX::XMFLOAT3 rotation);
	void setTranslation(DirectX::XMFLOAT3 translation);
	void setScaling(DirectX::XMFLOAT3 scaling);

	DirectX::XMFLOAT3 getRotation();
	DirectX::XMFLOAT3 getTranslation();
	DirectX::XMFLOAT3 getScaling();
	DirectX::XMMATRIX& getWorldMatrix();

	bool hasTexture = false;

	ID3D11Buffer* createVertexBuffer();

	void readMeshFromFile(std::string fileName);
	void readTextureFromFile(std::wstring textureName); //No need for each instance to hold this function

	bool isSky = false;
	~Mesh();
};