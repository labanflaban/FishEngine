#pragma once
#include "Mesh.h"

class Light
{
private:
	ID3D11Device* device = nullptr;

public:
	DirectX::XMFLOAT3 pos;
	Mesh* lightVolume = nullptr;
	Light(ID3D11Device* device);
	~Light();

	void setPosition(DirectX::XMFLOAT3 pos);
};