
#include "Mesh.h"

void Mesh::updateWorldMatrix()
{
	this->worldMatrix = rotationMatrix * translationMatrix * scalingMatrix;
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

	HRESULT succ2 = DxHandler::devicePtr->CreateBuffer(&bufferDesc, &subResData, &vertexBufferPtr);
	assert(SUCCEEDED(succ2));

	vertexBuffer = vertexBufferPtr;

	return vertexBufferPtr;
}

void Mesh::readMeshFromFile(std::string fileName)
{
	this->vertices = parser.readFromObj(fileName);
	createVertexBuffer();
}

void Mesh::readTextureFromFile(std::wstring textureName)
{
	HRESULT readTextureResult = DirectX::CreateWICTextureFromFile(DxHandler::devicePtr, textureName.data(), &texture, &textureView, 0);

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

	HRESULT createTextureResult = DxHandler::devicePtr->CreateTexture2D(&desc, NULL, &pTexture);
	assert(SUCCEEDED(createTextureResult));

	hasTexture = true;
}

Mesh::~Mesh()
{
}
