#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include <tchar.h>
#include <string>
#include <d3dcompiler.h>
#include <assert.h>

class BaseShader
{
private:

public:
	//BaseShader();
	virtual void useThis(ID3D11DeviceContext* contextPtr) = 0;
	virtual void compileShader(std::wstring pathToShader, ID3D11Device* device) = 0;
	ID3DBlob* getBuffer();

protected:
	ID3DBlob* shaderBuffer;
};