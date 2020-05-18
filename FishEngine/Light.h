#pragma once
#include "Mesh.h"

class Light
{
private:
	ID3D11Device* device = nullptr;

public:
	DirectX::XMFLOAT3 pos;
	Mesh* lightVolume = nullptr;
	DirectX::XMVECTOR lightColor = DirectX::XMVectorSet(1, 0, 0, 0);
	Light(ID3D11Device* device);
	~Light();

	int vectorIndex = 0;

	void setPosition(DirectX::XMFLOAT3 pos);
};

