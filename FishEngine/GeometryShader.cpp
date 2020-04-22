#include "GeometryShader.h"

void GeometryShader::useThis(ID3D11DeviceContext* contextPtr)
{
	contextPtr->GSSetShader(shaderPtr, NULL, NULL);
}

void GeometryShader::compileShader(std::wstring pathToShader, ID3D11Device* device)
{
	ID3DBlob* errorMessage;

	HRESULT	geoShaderSucc = D3DCompileFromFile(
		pathToShader.data(),
		nullptr,
		nullptr,
		"main",
		"gs_5_0",
		0,
		0,
		&shaderBuffer,
		&errorMessage
	);
	assert(SUCCEEDED(geoShaderSucc));

	HRESULT createVShaderSucc = device->CreateGeometryShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &shaderPtr);
	assert(SUCCEEDED(createVShaderSucc));
}

ID3D11GeometryShader* GeometryShader::getShaderPtr()
{
	return shaderPtr;
}
