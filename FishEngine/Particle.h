#pragma once
#include "Mesh.h"

class Particle : public Mesh
{
private:

public:
	DirectX::XMFLOAT3 velocity = DirectX::XMFLOAT3(0, 0, 0);
	Particle(ID3D11Device* device);
	void updateParticle();
};