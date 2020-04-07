#include "PixelShader.h"

void PixelShader::useThis(ID3D11DeviceContext* contextPtr)
{
	contextPtr->PSSetShader(shaderPtr, NULL, NULL);
}

void PixelShader::compileShader(std::wstring pathToShader, ID3D11Device* device)
{
	ID3DBlob* errorMessage;

	HRESULT	vertShaderSucc = D3DCompileFromFile(
		pathToShader.data(),
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		0,
		0,
		&shaderBuffer,
		&errorMessage
	);
	assert(SUCCEEDED(vertShaderSucc));

	HRESULT createVShaderSucc = device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &shaderPtr);
	assert(SUCCEEDED(createVShaderSucc));
}

ID3D11PixelShader* PixelShader::getShaderPtr()
{
	return shaderPtr;
}
