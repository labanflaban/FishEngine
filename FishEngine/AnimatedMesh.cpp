
#include "AnimatedMesh.h"

void AnimatedMesh::updateWorldMatrix()
{
	this->worldMatrix = scalingMatrix * rotationMatrix * translationMatrix;
}


void AnimatedMesh::initRigidbody(btDiscreteDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*>* collisionShapes, float mass)
{
	//rigidBody stuff
	btTransform rigidBodyTransform;

	rigidBodyTransform.setIdentity();
	rigidBodyTransform.setOrigin(btVector3(this->getTranslation().x, this->getTranslation().y, this->getTranslation().z));
	btCollisionShape* rigidBodyCollider = new btBoxShape(btVector3(btScalar(this->getScaling().x), btScalar(this->getScaling().y), btScalar(this->getScaling().z)));
	collisionShapes->push_back(rigidBodyCollider);

	bool isDynamic = (mass != 0.f);
	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		rigidBodyCollider->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(rigidBodyTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, rigidBodyCollider, localInertia);

	btDefaultMotionState* rigidBodyState = new btDefaultMotionState(rigidBodyTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo2(mass, rigidBodyState, rigidBodyCollider, btVector3(0, 0, 0)); //First param is 0, ie its static
	btRigidBody* rigidBodyBody = new btRigidBody(rbInfo2);
	this->rigidBody = rigidBodyBody;
	dynamicsWorld->addRigidBody(rigidBodyBody);
}

void AnimatedMesh::setRotation(DirectX::XMFLOAT3 rotation)
{
	this->rotation = rotation;
	this->rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	updateWorldMatrix();
}

void AnimatedMesh::setTranslation(DirectX::XMFLOAT3 translation)
{
	this->translation = translation;
	this->translationMatrix = DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);

	updateWorldMatrix();
}

void AnimatedMesh::setScaling(DirectX::XMFLOAT3 scaling)
{
	this->scaling = scaling;
	this->scalingMatrix = DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z);

	updateWorldMatrix();
}

void AnimatedMesh::move(DirectX::XMFLOAT3 deltaTranslation)
{
	DirectX::XMFLOAT3 currPos = this->getTranslation();

	this->setTranslation(DirectX::XMFLOAT3(currPos.x + deltaTranslation.x, currPos.y + deltaTranslation.y, currPos.z + deltaTranslation.z));
}

DirectX::XMFLOAT3 AnimatedMesh::getRotation()
{
	return this->rotation;
}

DirectX::XMFLOAT3 AnimatedMesh::getTranslation()
{
	return this->translation;
}

DirectX::XMFLOAT3 AnimatedMesh::getScaling()
{
	return this->scaling;
}

DirectX::XMMATRIX& AnimatedMesh::getWorldMatrix()
{
	return this->worldMatrix;
}

void AnimatedMesh::readMeshFromFID(std::string fileName)
{
	//this->vertices = fidParser.readFromFID(fileName);
	//createVertexBuffer();
}

void AnimatedMesh::readMeshFromFile(std::string fileName)
{
	//this->vertices = parser.readFromObj(fileName);
	//createVertexBuffer();
}

void AnimatedMesh::readTextureFromFile(std::wstring textureName)
{
	HRESULT readTextureResult = DirectX::CreateWICTextureFromFile(device, textureName.data(), &texture, &textureView, 0);

	//Check if file could be loaded
	assert(SUCCEEDED(readTextureResult));

	imageSampleDesc.MipLevels = imageSampleDesc.ArraySize = 1;
	imageSampleDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	imageSampleDesc.SampleDesc.Count = 1;
	imageSampleDesc.Usage = D3D11_USAGE_DEFAULT;
	imageSampleDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	imageSampleDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	imageSampleDesc.MiscFlags = 0;

	//Reading image size from the desc
	ID3D11Texture2D* pTextureInterface = 0;
	ID3D11Resource* res;
	textureView->GetResource(&res);
	res->QueryInterface<ID3D11Texture2D>(&pTextureInterface);

	D3D11_TEXTURE2D_DESC desc;
	pTextureInterface->GetDesc(&desc);
	imageSampleDesc.Width = static_cast<int>(desc.Width);
	imageSampleDesc.Height = static_cast<int>(desc.Height);

	res->Release();
	pTextureInterface->Release();

	HRESULT createTextureResult = device->CreateTexture2D(&desc, NULL, &pTexture);
	assert(SUCCEEDED(createTextureResult));

	hasTexture = true;
}

AnimatedMesh::AnimatedMesh(ID3D11Device* device)
{
	this->device = device;
}

AnimatedMesh::~AnimatedMesh()
{
	if (this->vertexBuffer)
	{
		this->vertexBuffer->Release();
	}
	//delete this->rigidBody;
	//delete this->collider;
}