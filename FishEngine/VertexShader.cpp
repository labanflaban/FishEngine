#include "VertexShader.h"

void VertexShader::useThis(ID3D11DeviceContext* contextPtr)
{
	contextPtr->VSSetShader(shaderPtr, NULL, NULL);
}

void VertexShader::compileShader(std::wstring pathToShader, ID3D11Device* device)
{
	ID3DBlob* errorMessage;

	HRESULT	vertShaderSucc = D3DCompileFromFile(
		pathToShader.data(),
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		0,
		0,
		&shaderBuffer,
		&errorMessage
	);
	assert(SUCCEEDED(vertShaderSucc));

	HRESULT createVShaderSucc = device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &shaderPtr);
	assert(SUCCEEDED(createVShaderSucc));
}

ID3D11VertexShader* VertexShader::getShaderPtr()
{
	return shaderPtr;
}
