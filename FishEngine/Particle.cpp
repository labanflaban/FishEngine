#include "Particle.h"

Particle::Particle(ID3D11Device* device)
	:Mesh(device)
{

}

void Particle::updateParticle()
{
	this->move(velocity);
	this->ticksLeft--;

	DirectX::XMFLOAT3 scaling = this->getScaling();
	this->setScaling(DirectX::XMFLOAT3(orgSize.x * ((float)ticksLeft / maxTicks), orgSize.y * ((float)ticksLeft / maxTicks), 0.1f));
}

void Particle::createMesh()
{
	this->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
	this->vertices.push_back(Vertex{ 1, -1, 0.1f,    1, 1, 1, 1, 1, 1, 0, 0, -1 });
	this->vertices.push_back(Vertex{ -1,  -1, 0.1f,  1, 1, 1, 1, 0, 1, 0, 0, -1 });

	this->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
	this->vertices.push_back(Vertex{ 1,  1, 0.1f,   1, 1, 1, 1, 1, 0, 0, 0, -1 });
	this->vertices.push_back(Vertex{ 1,  -1, 0.1f, 1, 1, 1, 1, 1, 1, 0, 0, -1 });

	this->createVertexBuffer();
}

Particle::~Particle()
{
	this->vertexBuffer = nullptr;
	this->textureView = nullptr;
}



