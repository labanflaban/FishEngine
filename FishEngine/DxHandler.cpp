#include "DxHandler.h"

float DxHandler::WIDTH;
float DxHandler::HEIGHT;


//Init static memory variables

ID3D11Device* DxHandler::devicePtr = nullptr;
ID3D11DeviceContext* DxHandler::contextPtr = nullptr;
HINSTANCE DxHandler::hInstance = HINSTANCE();

VertexShader* DxHandler::firstPassVertex = new VertexShader();
PixelShader* DxHandler::firstPassPixel = new PixelShader();

VertexShader* DxHandler::secondPassVertex = new VertexShader();
PixelShader* DxHandler::secondPassPixel = new PixelShader();

//VertexShader* DxHandler::skyboxVertexShader = new VertexShader();
//PixelShader* DxHandler::skyboxPixelShader = new PixelShader();

ID3D11InputLayout* DxHandler::input_layout_ptr = nullptr;

IDXGISwapChain* DxHandler::swapChainPtr = nullptr;
ID3D11RenderTargetView* DxHandler::renderTargetPtr = nullptr;
DXGI_SWAP_CHAIN_DESC DxHandler::swapDesc = DXGI_SWAP_CHAIN_DESC{ 0 };

ID3D11DepthStencilView* DxHandler::depthStencil = nullptr;
ID3D11Texture2D* DxHandler::depthBuffer = nullptr;

HWND* DxHandler::hWnd = nullptr;

Mesh* DxHandler::fullscreenQuad = nullptr;

ID3D11Buffer* DxHandler::constantVertexBuffer = nullptr;
ID3D11Buffer* DxHandler::constantPixelBuffer = nullptr;

void DxHandler::initalizeDeviceContextAndSwapChain()
{
	//Create device and initial DX11 interface.
		//Swapchain has 2 frame buffers, while it displays one it is drawing to the other.
	HRESULT succ = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&DxHandler::swapDesc,
		&DxHandler::swapChainPtr,
		&DxHandler::devicePtr,
		nullptr,
		&DxHandler::contextPtr
	);

	assert(SUCCEEDED(succ));
	assert(devicePtr != nullptr);

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE; //No cull. Done in geometry shader.
	ID3D11RasterizerState* rasterizerState;

	devicePtr->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	contextPtr->RSSetState(rasterizerState);

	wrl::ComPtr<ID3D11Texture2D> backBufferPtr; //Create the original render target view for main output.
	(swapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBufferPtr));
	HRESULT rendertargetSucc = devicePtr->CreateRenderTargetView(backBufferPtr.Get(), nullptr, &renderTargetPtr);
	assert(SUCCEEDED(rendertargetSucc));

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(contextPtr);
	spriteFont = std::make_unique<DirectX::SpriteFont>(devicePtr, L"./Fonts/Arial.spritefont");
}

void DxHandler::setupInputLayout()
{

	//Input layout is a structure declaring how data in the vertex should be stored and interpreted on the GPU.
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, //Vector3
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, //Vector4
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}, //Vector2
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, //Vector3
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 } //Vector3
	};

	HRESULT inputLayoutSucc = DxHandler::devicePtr->CreateInputLayout
	(
		inputDesc,
		ARRAYSIZE(inputDesc),
		DxHandler::firstPassVertex->getBuffer()->GetBufferPointer(),
		DxHandler::firstPassVertex->getBuffer()->GetBufferSize(),
		&DxHandler::input_layout_ptr
	);
	assert(SUCCEEDED(inputLayoutSucc));
}

void DxHandler::setupDepthBuffer()
{
	D3D11_TEXTURE2D_DESC depthDesc{ 0 };
	depthDesc.Width = WIDTH;
	depthDesc.Height = HEIGHT;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	devicePtr->CreateTexture2D(&depthDesc, NULL, &depthBuffer);
	devicePtr->CreateDepthStencilView(DxHandler::depthBuffer, NULL, &depthStencil);
}

void DxHandler::generateFullScreenQuad()
{
	DxHandler::fullscreenQuad = new Mesh;	  //X  Y  Z   R	 G  B  A, U, V  nX nY nZ
	fullscreenQuad->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
	fullscreenQuad->vertices.push_back(Vertex{ 1, -1, 0.1f,    1, 1, 1, 1, 1, 1, 0, 0, -1 });
	fullscreenQuad->vertices.push_back(Vertex{ -1,  -1, 0.1f,  1, 1, 1, 1, 0, 1, 0, 0, -1 });

	fullscreenQuad->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
	fullscreenQuad->vertices.push_back(Vertex{ 1,  1, 0.1f,   1, 1, 1, 1, 1, 0, 0, 0, -1 });
	fullscreenQuad->vertices.push_back(Vertex{ 1,  -1, 0.1f, 1, 1, 1, 1, 1, 1, 0, 0, -1 });

	fullscreenQuad->createVertexBuffer();
}

void DxHandler::drawFullscreenQuad()
{
	UINT stride = (UINT)sizeof(float) * FLOATS_PER_VERTEX;
	UINT offset = 0u;

	DxHandler::contextPtr->IASetVertexBuffers(0, 1, &fullscreenQuad->vertexBuffer,
		&stride, &offset);

	DxHandler::contextPtr->Draw(fullscreenQuad->vertices.size(), 0);
}

ID3D11Buffer* DxHandler::createVSConstBuffer(VS_CONSTANT_MATRIX_BUFFER& matrix)
{
	D3D11_BUFFER_DESC constBDesc;
	constBDesc.ByteWidth = sizeof(matrix);
	constBDesc.Usage = D3D11_USAGE_DEFAULT;
	constBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBDesc.CPUAccessFlags = 0;
	constBDesc.MiscFlags = 0;
	constBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &matrix;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	ID3D11Buffer* constantBuffer = NULL;
	HRESULT buffSucc = devicePtr->CreateBuffer(&constBDesc, &InitData,
		&constantBuffer);
	assert(SUCCEEDED(buffSucc));

	//deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	DxHandler::contextPtr->VSSetConstantBuffers(PER_OBJECT_CBUFFER_SLOT, 1, &constantBuffer);

	//loadedVSBuffers.push_back(constantBuffer);
	this->constantVertexBuffer = constantBuffer;

	return constantVertexBuffer;
}

ID3D11Buffer* DxHandler::createPSConstBuffer(PS_CONSTANT_LIGHT_BUFFER& matrix)
{
	D3D11_BUFFER_DESC constPixelDesc;
	constPixelDesc.ByteWidth = sizeof(PS_CONSTANT_LIGHT_BUFFER);
	constPixelDesc.Usage = D3D11_USAGE_DEFAULT;
	constPixelDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constPixelDesc.CPUAccessFlags = 0;
	constPixelDesc.MiscFlags = 0;
	constPixelDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitDataPixel;
	InitDataPixel.pSysMem = &matrix;
	InitDataPixel.SysMemPitch = 0;
	InitDataPixel.SysMemSlicePitch = 0;
	ID3D11Buffer* constantPixelBuffer = NULL;
	HRESULT buffPSucc = devicePtr->CreateBuffer(&constPixelDesc, &InitDataPixel,
		&constantPixelBuffer);
	assert(SUCCEEDED(buffPSucc));

	contextPtr->UpdateSubresource(constantPixelBuffer, 0, NULL, &matrix, 0, 0);
	contextPtr->PSSetConstantBuffers(0, 1, &constantPixelBuffer);

	//loadedPSBuffers.push_back(constantPixelBuffer);
	this->constantPixelBuffer = constantPixelBuffer;

	return constantPixelBuffer;
}

void DxHandler::draw(Mesh* drawMesh, Camera drawFromCamera, bool isSky)
{
	UINT stride = (UINT)sizeof(float) * FLOATS_PER_VERTEX;
	UINT offset = 0u;

	VS_CONSTANT_MATRIX_BUFFER matrixBuff;
	matrixBuff.worldViewProjectionMatrix = drawMesh->getWorldMatrix() * drawFromCamera.cameraView * drawFromCamera.cameraProjectionMatrix;
	matrixBuff.worldMatrix = drawMesh->getWorldMatrix();
	matrixBuff.viewMatrix = drawFromCamera.cameraView;
	matrixBuff.projMatrix = drawFromCamera.cameraProjectionMatrix;
	matrixBuff.isSky = isSky;

	PS_CONSTANT_LIGHT_BUFFER lightBuff;
	lightBuff.isSky = isSky;
	lightBuff.camPos = drawFromCamera.cameraPosition;

	DxHandler::contextPtr->UpdateSubresource(constantPixelBuffer, 0, NULL, &lightBuff, 0, 0);
	DxHandler::contextPtr->UpdateSubresource(constantVertexBuffer, 0, NULL, &matrixBuff, 0, 0);

	//Set vertex buffer to the mesh you want to draw
	DxHandler::contextPtr->IASetVertexBuffers(0, 1, &drawMesh->vertexBuffer,
		&stride, &offset);
	//Draw it
	DxHandler::contextPtr->Draw(drawMesh->vertices.size(), 0);

}

void DxHandler::drawText()
{
	//this->spriteBatch->Begin();
	//spriteFont->DrawString(spriteBatch.get(), L"YEET", DirectX::XMFLOAT2(0, 0), DirectX::Colors::Red, 0.0f, DirectX::XMFLOAT2(0,0), DirectX::XMFLOAT2(1.0f, 1.0f));
	//this->spriteBatch->End();
}

DxHandler::DxHandler(HWND hWnd)
{
	DxHandler::hWnd = &hWnd;
	configureSwapChain(*DxHandler::hWnd);
	initalizeDeviceContextAndSwapChain();
}

void DxHandler::configureSwapChain(HWND& hWnd)
{
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE; //The hardware driver we'd like to use.
	D3D_FEATURE_LEVEL version[] = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0 }; //Which version we're running
	// CREATING OUR CHAIN DESC

	//BUFFERDESC
	DxHandler::swapDesc.BufferDesc.Width = 0; //Defaults to screenwidth
	DxHandler::swapDesc.BufferDesc.Height = 0;
	DxHandler::swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;// doesnt work with 32 for some reason
	DxHandler::swapDesc.BufferDesc.RefreshRate.Numerator = 60;//can be set to 0 for no max
	DxHandler::swapDesc.BufferDesc.RefreshRate.Denominator = 0;
	DxHandler::swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	DxHandler::swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	//SAMPLEDESC
	DxHandler::swapDesc.SampleDesc.Count = 1;
	DxHandler::swapDesc.SampleDesc.Quality = 0;
	//END SAMPLEDESC

	//GENERAL SWAPDESC
	DxHandler::swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	DxHandler::swapDesc.BufferCount = 2;
	DxHandler::swapDesc.OutputWindow = hWnd;
	DxHandler::swapDesc.Windowed = TRUE;
	DxHandler::swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	DxHandler::swapDesc.Flags = 0;
	//Checks for debug mode or not
	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
}
