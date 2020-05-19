#pragma once
#include "Vertex.h"
#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment(lib, "d3dcompiler.lib")
#include <iostream>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include<vector>
#include "DirectXHelpers.h"
#include "DirectXMath.h"

struct KeyframeVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
};

class Keyframes
{
private:

public:
	ID3D11Buffer* structuredBuffer = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	ID3D11Buffer* vertexStateBuffer = nullptr;
	ID3D11UnorderedAccessView* vertexStateUAV = nullptr;

	int nrOfPoses = 0;
	UINT nrOfVertices = 1;
	std::vector<Vertex> vertices;

	bool decrementT = false;
	bool manualUpdate = false;

	double last = 0.0;
	int fromIndex = 0;
	int targetPoseIndex = 0; //Neutral
	double t = 0.0;
	double animationSpeed = 1;
	double remaining = 0.0;

	float w1 = 0;
	float w2 = 0;
	float w3 = 0;
	DirectX::XMFLOAT2 v1 = DirectX::XMFLOAT2(0.5, 1);
	DirectX::XMFLOAT2 v2 = DirectX::XMFLOAT2(0, 0);
	DirectX::XMFLOAT2 v3 = DirectX::XMFLOAT2(1, 0);

	DirectX::XMFLOAT2 point = DirectX::XMFLOAT2(1, 0);
	void computeWeights();

	void tweenTowardV1(float amount);

	void createStructuredBuffer(ID3D11Device* device);
	void appendStructuredBuffer(std::vector<Vertex>* vertVectorsToMerge[], int nrOfVectorsToMerge);

	virtual void stepAnim(double deltaT = 0.0);
};