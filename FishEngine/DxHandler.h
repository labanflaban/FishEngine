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
	DirectX::XMMATRIX worldViewProjectionMatrix;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projMatrix;
};

struct PS_CONSTANT_LIGHT_BUFFER
{
	DirectX::XMVECTOR lightPos;
	DirectX::XMFLOAT4 ambientMeshColor;
	DirectX::XMFLOAT4 diffuseMeshColor;
	DirectX::XMFLOAT4 specularMeshColor;
	
	DirectX::XMVECTOR camPos;
	DirectX::XMMATRIX worldViewProjectionMatrix;

	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projMatrix;

	BOOL hasTexture = false;
};

struct Camera //Put into its own class
{
	DirectX::XMMATRIX cameraProjectionMatrix; //Contains far plane, aspect ratio etc
	DirectX::XMMATRIX cameraView;

	DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(0, 0, 0, 0);
	DirectX::XMVECTOR cameraTarget = DirectX::XMVectorSet(0, 0, 1, 0);
	DirectX::XMVECTOR cameraUp = DirectX::XMVectorSet(0, 1, 0, 0);

	Camera(int WIDTH, int HEIGHT)
	{
		this->cameraProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH( //Creates projection space
			0.35f * 3.14f,					//FovAngleY, height angle of perspective in radians
			(float)WIDTH / (float)HEIGHT,	//AspectRatio, width/height of window
			0.1f,							//NearZ, how close we render
			10000.f							//FarZ how far we render
		);
	}

	void updateCamera()
	{
		cameraView = DirectX::XMMatrixLookAtLH(Camera::cameraPosition, Camera::cameraTarget, Camera::cameraUp);

	}
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

