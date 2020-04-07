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

#include "VertexShader.h"
#include "PixelShader.h"


#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment(lib, "d3dcompiler.lib")

const int FLOATS_PER_VERTEX = 15;
namespace wrl = Microsoft::WRL;

const int PER_OBJECT_CBUFFER_SLOT = 0;
const int CAMERA_CBUFFER_SLOT = 1;

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

	static float WIDTH;
	static float HEIGHT;

	static IDXGISwapChain* swapChainPtr;
	static ID3D11RenderTargetView* renderTargetPtr;
	static DXGI_SWAP_CHAIN_DESC swapDesc; 

	static ID3D11InputLayout* input_layout_ptr;

	DxHandler(HWND hWnd);
	void configureSwapChain(HWND& hWnd);
	void initalizeDeviceContextAndSwapChain();
	static void setupInputLayout();
	void setupDepthBuffer();

	

 
};

