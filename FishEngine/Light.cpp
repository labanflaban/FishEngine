#include "Light.h"

Light::Light(ID3D11Device* device)
{
	this->device = device;
	this->lightVolume = new Mesh(device);
	lightVolume->readMeshFromFile("./Models/sphere.obj");
}

Light::~Light()
{
	if (this->lightVolume)
	{
		//this->lightVolume->vertexBuffer->Release();
		delete this->lightVolume;
	}
	//
}

void Light::setPosition(DirectX::XMFLOAT3 pos)
{
	this->lightVolume->setTranslation(pos);
	this->pos = pos;
	lightVolume->setScaling(DirectX::XMFLOAT3(120, 120, 120));
}
