#pragma once
#include "Mesh.h"

class Particle : public Mesh
{
private:

public:
	DirectX::XMFLOAT3 velocity = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 orgSize = DirectX::XMFLOAT3(0, 0, 0);

	Particle(ID3D11Device* device);
	void updateParticle();
	void createMesh();

	int vectorIndex = 0;
	int ticksLeft = 200;
	int maxTicks = ticksLeft;

	~Particle();
};