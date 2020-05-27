#pragma once
#include "ObjParser.h"
#include "Vertex.h"
#include "FIDParser.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <assert.h>

using namespace std;

struct AssetReturnStruct
{
	ID3D11Buffer* buffer;
	unsigned long long int nrOfVerts = 0;
};

class AssetLoader
{
private:

public:
	static ID3D11Buffer* createBuffer(vector<Vertex>* vertices);
	static ID3D11Device* devicePtr;

	static unordered_map<std::string, AssetReturnStruct*>* meshDictionary;
	static AssetReturnStruct* loadAssetFID(std::string path);
	static AssetReturnStruct* loadAssetObj(std::string path);
};