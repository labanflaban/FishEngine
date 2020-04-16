#include "Skybox.h"
Mesh* Skybox::sphereModel = nullptr;
ID3D11Resource* Skybox::texture = nullptr;
ID3D11Texture2D* Skybox::pTexture = nullptr;
ID3D11ShaderResourceView* Skybox::srv = nullptr;

void Skybox::loadSkybox(ID3D11Device* devicePtr)
{
	std::wstring path = L"./Textures/skymap.dds";
	HRESULT succ = DirectX::CreateDDSTextureFromFile(devicePtr, path.data(), &texture, &srv, NULL, NULL);
	//assert(SUCCEEDED(succ));

	Skybox::sphereModel = new Mesh;
	sphereModel->readMeshFromFile("./Models/sphere.obj");
	sphereModel->setRotation(DirectX::XMFLOAT3(-3.14, 0, 0));
}
