#include "Keyframes.h"
#include <assert.h>

void Keyframes::computeWeights()
{
	w1 = ((v2.y - v3.y) * (point.x - v3.x) + (v3.x - v2.x) * (point.y - v3.y)) / ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));
	w2 = ((v3.y - v1.y) * (point.x - v3.x) + (v1.x - v3.x) * (point.y - v3.y)) / ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));
	w3 = 1 - w1 - w2;
}

void Keyframes::tweenTowardV1(float amount)
{
	using namespace DirectX;
	XMVECTOR edge1;
	XMVECTOR p;
	XMVECTOR dir;

	edge1 = XMLoadFloat2(&v1);
	p = XMLoadFloat2(&point);
	dir = DirectX::XMVector2Normalize(edge1 - p);

	point = XMFLOAT2(DirectX::XMVectorGetX(dir), DirectX::XMVectorGetY(dir));

	computeWeights();
}

void Keyframes::createStructuredBuffer(ID3D11Device* device)
{
	if (structuredBuffer != nullptr)
	{
		structuredBuffer->Release();
		std::cout << "Structured buffer for animatedMesh already exists, releasing to create a new one." << std::endl;
		//delete structuredBuffer;
	}

	D3D11_BUFFER_DESC buffDesc = { 0 };
	buffDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	buffDesc.ByteWidth = sizeof(Vertex)*vertices.size();
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //Try to change to 0.
	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	buffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	buffDesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertices.data();

	HRESULT succ = device->CreateBuffer(&buffDesc, &initData, &structuredBuffer);
	assert(SUCCEEDED(succ));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.ElementWidth = vertices.size();
	srvDesc.Buffer.FirstElement = 0;

	HRESULT shaderResourceSucc = device->CreateShaderResourceView(structuredBuffer, &srvDesc, &srv);
	assert(SUCCEEDED(shaderResourceSucc));

	//Create UAV stuff
	std::vector<KeyframeVertex> animStateVector;
	for (int i = 0; i < nrOfVertices; i++)
	{
		animStateVector.push_back(KeyframeVertex());
		animStateVector.at(i).position = DirectX::XMFLOAT3(vertices.at(i).x, vertices.at(i).y, vertices.at(i).z);
		animStateVector.at(i).normal = DirectX::XMFLOAT3(vertices.at(i).nx, vertices.at(i).ny, vertices.at(i).nz);
	}

	D3D11_SUBRESOURCE_DATA initDataUAV;
	initDataUAV.pSysMem = animStateVector.data(); 

	D3D11_BUFFER_DESC buffDesc1 = {};
	buffDesc1.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	buffDesc1.CPUAccessFlags = NULL;
	buffDesc1.Usage = D3D11_USAGE_DEFAULT;
	buffDesc1.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	buffDesc1.StructureByteStride = sizeof(KeyframeVertex);
	buffDesc1.ByteWidth = sizeof(KeyframeVertex) * nrOfVertices;
	
	HRESULT buffSucc1 = device->CreateBuffer(&buffDesc1, &initDataUAV, &vertexStateBuffer);
	assert(SUCCEEDED(buffSucc1));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = nrOfVertices;
	HRESULT uavSucc = device->CreateUnorderedAccessView(this->vertexStateBuffer, &uavDesc, &vertexStateUAV);
	assert(SUCCEEDED(uavSucc));
}

void Keyframes::appendStructuredBuffer(std::vector<Vertex>* vertVectorsToMerge[], int nrOfVectorsToMerge)
{

	for (int i = 0; i < nrOfVectorsToMerge; i++) //Append the meshes after each other in verts
	{
		vertices.insert(vertices.end(), vertVectorsToMerge[i]->begin(), vertVectorsToMerge[i]->end());
	}

	this->nrOfPoses += nrOfVectorsToMerge;
	this->nrOfVertices = vertices.size() / nrOfPoses;
}

void Keyframes::stepAnim(double deltaT)
{
	//if (t >= 1)
	//{
	//	//t = 0;
	//	this->decrementT = true;
	//}
	//	
	//	//decrementT = true;

	//if (t <= 0)
	//	decrementT = false;

	//if (!decrementT)
	//{
	//	this->t += deltaT * animationSpeed;
	//}
	//else
	//	this->t -= deltaT * animationSpeed;

	this->t = this->t + deltaT * this->animationSpeed;

	if (this->t > 0.99f)
	{
		this->t = 0.f;
		this->remaining = 0.f;

		this->targetPoseIndex = ((++this->targetPoseIndex) % this->nrOfPoses);
		std::cout << "Index: " << this->targetPoseIndex << std::endl;
		//this->decrementT = true;
	}
	else
		this->remaining = this->animationSpeed * deltaT / (1 - this->t);
}
