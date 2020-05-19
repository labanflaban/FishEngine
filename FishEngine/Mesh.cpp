
#include "Mesh.h"

void Mesh::updateWorldMatrix()
{
	this->worldMatrix = scalingMatrix * rotationMatrix * translationMatrix;
}


void Mesh::initRigidbody(btDiscreteDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*>* collisionShapes, float mass, btCollisionShape* collShape)
{
	//rigidBody stuff
	btTransform rigidBodyTransform;

	rigidBodyTransform.setIdentity();
	rigidBodyTransform.setOrigin(btVector3(this->getTranslation().x, this->getTranslation().y, this->getTranslation().z));

	btCollisionShape* rigidBodyCollider = nullptr;
	if (collShape == nullptr)
		rigidBodyCollider = new btBoxShape(btVector3(btScalar(this->getScaling().x), btScalar(this->getScaling().y), btScalar(this->getScaling().z)));
	else
		rigidBodyCollider = collShape;
	collisionShapes->push_back(rigidBodyCollider);

	bool isDynamic = (mass != 0.f);
	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		rigidBodyCollider->calculateLocalInertia(mass, localInertia);

	btQuaternion xRot = btQuaternion(btVector3(1, 0, 0), getRotation().x);
	btQuaternion yRot = btQuaternion(btVector3(0, 1, 0), getRotation().y);
	btQuaternion zRot = btQuaternion(btVector3(0, 0, 1), getRotation().z);

	rigidBodyTransform.setRotation(xRot * yRot * zRot);
	btDefaultMotionState* myMotionState = new btDefaultMotionState(rigidBodyTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, rigidBodyCollider, localInertia);

	btDefaultMotionState* rigidBodyState = new btDefaultMotionState(rigidBodyTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo2(mass, rigidBodyState, rigidBodyCollider, btVector3(0, 0, 0)); //First param is 0, ie its static
	btRigidBody* rigidBodyBody = new btRigidBody(rbInfo2);
	this->rigidBody = rigidBodyBody;


	//this->rigidBody->setUserPointer(collStruct);

	dynamicsWorld->addRigidBody(rigidBodyBody);
}

void Mesh::setRotation(DirectX::XMFLOAT3 rotation)
{
	this->rotation = rotation;
	this->rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	updateWorldMatrix();
}

void Mesh::setTranslation(DirectX::XMFLOAT3 translation)
{
	this->translation = translation;
	this->translationMatrix = DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);

	updateWorldMatrix();
}

void Mesh::setScaling(DirectX::XMFLOAT3 scaling)
{
	this->scaling = scaling;
	this->scalingMatrix = DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z);

	updateWorldMatrix();
}

void Mesh::move(DirectX::XMFLOAT3 deltaTranslation)
{
	DirectX::XMFLOAT3 currPos = this->getTranslation();

	this->setTranslation(DirectX::XMFLOAT3(currPos.x + deltaTranslation.x, currPos.y + deltaTranslation.y, currPos.z + deltaTranslation.z));
}

DirectX::XMFLOAT3 Mesh::getRotation()
{
	return this->rotation;
}

DirectX::XMFLOAT3 Mesh::getTranslation()
{
	return this->translation;
}

DirectX::XMFLOAT3 Mesh::getScaling()
{
	return this->scaling;
}

DirectX::XMMATRIX& Mesh::getWorldMatrix()
{
	return this->worldMatrix;
}

ID3D11Buffer* Mesh::createVertexBuffer()
{
	ID3D11Buffer* vertexBufferPtr = NULL;

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = sizeof(float) * FLOATS_PER_VERTEX * this->vertices.size(); //declares how many bytes it should contain

	D3D11_SUBRESOURCE_DATA subResData = D3D11_SUBRESOURCE_DATA{ 0 };
	subResData.pSysMem = vertices.data();
	subResData.SysMemPitch = FLOATS_PER_VERTEX * sizeof(float) * vertices.size();

	HRESULT succ2 = device->CreateBuffer(&bufferDesc, &subResData, &vertexBufferPtr);
	assert(SUCCEEDED(succ2));

	vertexBuffer = vertexBufferPtr;

	this->nrOfVertices = vertices.size();
	return vertexBufferPtr;
}

void Mesh::readMeshFromFID(std::string fileName)
{
	this->vertices = fidParser.readFromFID(fileName);
	createVertexBuffer();
}

void Mesh::readMeshFromFile(std::string fileName)
{
	this->vertices = parser.readFromObj(fileName);
	createVertexBuffer();
}

void Mesh::readTextureFromFile(std::wstring textureName)
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

void Mesh::readNormalMapFromFile(std::wstring NormalMapName)
{
	HRESULT readNormalMapResult = DirectX::CreateWICTextureFromFile(device, NormalMapName.data(), &NormalMap, &NormalView, 0);

	assert(SUCCEEDED(readNormalMapResult));

	imageSampleDesc.MipLevels = imageSampleDesc.ArraySize = 1;
	imageSampleDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	imageSampleDesc.SampleDesc.Count = 1;
	imageSampleDesc.Usage = D3D11_USAGE_DEFAULT;
	imageSampleDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	imageSampleDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	imageSampleDesc.MiscFlags = 0;

	ID3D11Texture2D* NormalInterface = 0;
	ID3D11Resource* Resource;

	NormalView->GetResource(&Resource);
	Resource->QueryInterface<ID3D11Texture2D>(&NormalInterface);

	D3D11_TEXTURE2D_DESC desc;
	NormalInterface->GetDesc(&desc);
	imageSampleDesc.Width = static_cast<int>(desc.Width);
	imageSampleDesc.Height = static_cast<int>(desc.Height);

	Resource->Release();
	NormalInterface->Release();

	HRESULT createNormalResult = device->CreateTexture2D(&desc, NULL, &pNormalMap);
	assert(SUCCEEDED(createNormalResult));

	hasNormalMap = true;
}

Mesh::Mesh(ID3D11Device* device)
{
	this->device = device;
}

Mesh::~Mesh()
{
	if (this->vertexBuffer)
	{
		this->vertexBuffer->Release();
	}
	if (this->textureView)
	{
		this->textureView->Release();
	}

	//delete this->rigidBody;
	//delete this->collider;
}
