
#pragma once
#include "BaseShader.h"

class GeometryShader : public BaseShader
{
private:
	ID3D11GeometryShader* shaderPtr = nullptr;
public:


	// Inherited via BaseShader
	virtual void useThis(ID3D11DeviceContext* contextPtr) override;
	virtual void compileShader(std::wstring pathToShader, ID3D11Device* device) override;
	ID3D11GeometryShader* getShaderPtr();
};