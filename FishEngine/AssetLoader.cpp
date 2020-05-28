#include "AssetLoader.h"

unordered_map<std::string, AssetReturnStruct*>* AssetLoader::meshDictionary = new unordered_map<std::string, AssetReturnStruct*>;
ID3D11Device* AssetLoader::devicePtr = nullptr;

ID3D11Buffer* AssetLoader::createBuffer(vector<Vertex>* vertices)
{
	ID3D11Buffer* vertexBufferPtr = NULL;

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = sizeof(float) * FLOATS_PER_VERTEX * vertices->size(); //declares how many bytes it should contain

	D3D11_SUBRESOURCE_DATA subResData = D3D11_SUBRESOURCE_DATA{ 0 };
	subResData.pSysMem = vertices->data();
	subResData.SysMemPitch = FLOATS_PER_VERTEX * sizeof(float) * vertices->size();

	HRESULT succ2 = devicePtr->CreateBuffer(&bufferDesc, &subResData, &vertexBufferPtr);
	assert(SUCCEEDED(succ2));

	return vertexBufferPtr;
}

AssetReturnStruct* AssetLoader::loadAssetFID(std::string path)
{

	auto found = meshDictionary->find(path);
	if (found == meshDictionary->end()) //Not found
	{
		vector<Vertex>* vec = new vector<Vertex>(FIDParser::readFromFID(path));
		ID3D11Buffer* vertBuff = createBuffer(vec);

		AssetReturnStruct* returnVals = new AssetReturnStruct;
		returnVals->nrOfVerts = vec->size();
		returnVals->buffer = vertBuff;

		meshDictionary->insert(make_pair(path, returnVals));
		return returnVals;
	}
	else
		return found->second;
}

AssetReturnStruct* AssetLoader::loadAssetObj(std::string path)
{
	auto found = meshDictionary->find(path);
	if (found == meshDictionary->end()) //Not found
	{
		vector<Vertex>* vec = new vector<Vertex>(ObjParser::readFromObj(path));
		ID3D11Buffer* vertBuff = createBuffer(vec);

		AssetReturnStruct* returnVals = new AssetReturnStruct;
		returnVals->nrOfVerts = vec->size();
		returnVals->buffer = vertBuff;

		meshDictionary->insert(make_pair(path, returnVals));
		return returnVals;
	}
	else
		return found->second;
}
