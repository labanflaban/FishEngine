#include "Keyframes.h"
#include <assert.h>

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

	std::cout << "Test" << std::endl;
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
