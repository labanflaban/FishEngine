#pragma once
#include "DDSTextureLoader.h"
#include <string>
#include "ObjParser.h"
#include "Mesh.h"

class Skybox
{
private:
	static ID3D11Texture2D* pTexture;

public:
	static ID3D11ShaderResourceView* textureView;
	static ID3D11Resource* texture;
	static ID3D11ShaderResourceView* srv;

	static Mesh* sphereModel;
	static void loadSkybox(ID3D11Device* devicePtr);
};