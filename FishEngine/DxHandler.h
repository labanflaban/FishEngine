#pragma once
#define WIN32_LEAN_AND_MEAN
#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <assert.h>
#include <cassert>
#include <iostream>
#include <Windows.h>
#include <dxgi1_6.h>
#include <tchar.h>
#include <string>
#include <wrl.h>
#include <vector>
#include <stdint.h>
#include <chrono>
#include <DirectXHelpers.h>
#include <SimpleMath.h>
#include <WICTextureLoader.h>
#include <math.h>
#include "Mesh.h"


#include "VertexShader.h"
#include "PixelShader.h"

#include "Camera.h"

#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment(lib, "d3dcompiler.lib")

const int FLOATS_PER_VERTEX = 15;
namespace wrl = Microsoft::WRL;

const int PER_OBJECT_CBUFFER_SLOT = 0;
const int CAMERA_CBUFFER_SLOT = 1;

class Mesh;

struct VS_CONSTANT_MATRIX_BUFFER
{
	DirectX::XMMATRIX worldViewProjectionMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX projMatrix = DirectX::XMMatrixIdentity();
};

struct PS_CONSTANT_LIGHT_BUFFER
{
	DirectX::XMVECTOR lightPos = DirectX::XMVectorSet(0, 0, 0, 0);
	DirectX::XMFLOAT4 ambientMeshColor = DirectX::XMFLOAT4(0, 0, 0, 0);
	DirectX::XMFLOAT4 diffuseMeshColor = DirectX::XMFLOAT4(0, 0, 0, 0);
	DirectX::XMFLOAT4 specularMeshColor = DirectX::XMFLOAT4(0, 0, 0, 0);
	
	DirectX::XMVECTOR camPos = DirectX::XMVectorSet(0, 0, 0, 0);
	DirectX::XMMATRIX worldViewProjectionMatrix = DirectX::XMMatrixIdentity();

	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX projMatrix = DirectX::XMMatrixIdentity();

	BOOL hasTexture = false;
};

class DxHandler
{
private:

public:
	static ID3D11DepthStencilView* depthStencil;
	static ID3D11Texture2D* depthBuffer;

	static VertexShader* firstPassVertex; //Contains geometry data, first pass in deferred
	static PixelShader* firstPassPixel; //Outputs to a bunch of images 'SRVs'

	static VertexShader* secondPassVertex; //Reads a full screen quad
	static PixelShader* secondPassPixel; //Fills full screen quad with data from textures in previous in first pass.

	static  ID3D11Device* devicePtr;
	static  ID3D11DeviceContext* contextPtr;
	static  HINSTANCE hInstance;
	static	HWND* hWnd;

	static Mesh* fullscreenQuad;

	static float WIDTH;
	static float HEIGHT;

	static IDXGISwapChain* swapChainPtr;
	static ID3D11RenderTargetView* renderTargetPtr;
	static DXGI_SWAP_CHAIN_DESC swapDesc; 

	static ID3D11InputLayout* input_layout_ptr;

	static ID3D11Buffer* constantVertexBuffer;
	static ID3D11Buffer* constantPixelBuffer;

	DxHandler(HWND hWnd);
	void configureSwapChain(HWND& hWnd);
	void initalizeDeviceContextAndSwapChain();
	static void setupInputLayout();
	void setupDepthBuffer();

	void generateFullScreenQuad();
	void drawFullscreenQuad();

	ID3D11Buffer* createVSConstBuffer(VS_CONSTANT_MATRIX_BUFFER& matrix);
	ID3D11Buffer* createPSConstBuffer(PS_CONSTANT_LIGHT_BUFFER& matrix);

	void draw(Mesh* drawMesh, Camera drawFromCamera);

 
};

