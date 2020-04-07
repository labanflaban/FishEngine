#pragma once
#include "BaseShader.h"

class PixelShader : public BaseShader
{
private:
	ID3D11PixelShader* shaderPtr = nullptr;
public:


	// Inherited via BaseShader
	virtual void useThis(ID3D11DeviceContext* contextPtr) override;
	virtual void compileShader(std::wstring pathToShader, ID3D11Device* device) override;
	ID3D11PixelShader* getShaderPtr();
};