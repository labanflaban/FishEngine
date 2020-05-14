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

PixelShader* DxHandler::lightPixel = new PixelShader();
PixelShader* DxHandler::particlePixel = new PixelShader();

PixelShader* DxHandler::transparencyPixel = new PixelShader();
VertexShader* DxHandler::transparencyVertex = new VertexShader();
PixelShader* DxHandler::skyboxPixel = new PixelShader();
VertexShader* DxHandler::skyboxVertex = new VertexShader();

VertexShader* DxHandler::animVertex = new VertexShader; //´handles morph anims

//VertexShader* DxHandler::skyboxVertexShader = new VertexShader();
//PixelShader* DxHandler::skyboxPixelShader = new PixelShader();

ID3D11InputLayout* DxHandler::input_layout_ptr = nullptr;

ID3D11DepthStencilState* DxHandler::depthStencilState = nullptr;

IDXGISwapChain* DxHandler::swapChainPtr = nullptr;
ID3D11RenderTargetView* DxHandler::renderTargetPtr = nullptr;
DXGI_SWAP_CHAIN_DESC DxHandler::swapDesc = DXGI_SWAP_CHAIN_DESC{ 0 };

ID3D11DepthStencilView* DxHandler::depthStencil = nullptr;
ID3D11Texture2D* DxHandler::depthBuffer = nullptr;

HWND* DxHandler::hWnd = nullptr;

Mesh* DxHandler::fullscreenQuad = nullptr;
GeometryShader* DxHandler::backfaceCullShader = new GeometryShader;

ID3D11RasterizerState* DxHandler::rasterizerState = nullptr;

ID3D11Buffer* DxHandler::constantVertexBuffer = nullptr;
ID3D11Buffer* DxHandler::constantPixelBuffer = nullptr;
ID3D11Buffer* DxHandler::constantAnimBuffer = nullptr;

ID3D11BlendState* DxHandler::additiveBlendState = nullptr;
ID3D11BlendState* DxHandler::alphaBlendState = nullptr;
ID3D11SamplerState* DxHandler::standardSampler = nullptr;

ID3D11Buffer* DxHandler::GSConstBuff = nullptr;

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
	rasterizerDesc.CullMode = D3D11_CULL_BACK; //No cull. Done in geometry shader.

	devicePtr->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	contextPtr->RSSetState(rasterizerState);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //RGBA 4 lyf
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; //Means it'll be handled as a 2D array
	rtvDesc.Texture2D.MipSlice = 0;

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

	D3D11_DEPTH_STENCIL_DESC depthStateDesc = { 0 };
	depthStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	devicePtr->CreateDepthStencilState(&depthStateDesc, &depthStencilState);
	contextPtr->OMSetDepthStencilState(depthStencilState, 0);
}

void DxHandler::generateFullScreenQuad()
{
	DxHandler::fullscreenQuad = new Mesh(devicePtr);	  //X  Y  Z   R	 G  B  A, U, V  nX nY nZ
	fullscreenQuad->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
	fullscreenQuad->vertices.push_back(Vertex{ 1, -1, 0.1f,    1, 1, 1, 1, 1, 1, 0, 0, -1 });
	fullscreenQuad->vertices.push_back(Vertex{ -1,  -1, 0.1f,  1, 1, 1, 1, 0, 1, 0, 0, -1 });

	fullscreenQuad->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
	fullscreenQuad->vertices.push_back(Vertex{ 1,  1, 0.1f,   1, 1, 1, 1, 1, 0, 0, 0, -1 });
	fullscreenQuad->vertices.push_back(Vertex{ 1,  -1, 0.1f, 1, 1, 1, 1, 1, 1, 0, 0, -1 });

	fullscreenQuad->createVertexBuffer();
}

void DxHandler::drawFullscreenQuad(Camera& drawFromCamera)
{
	UINT stride = (UINT)sizeof(float) * FLOATS_PER_VERTEX;
	UINT offset = 0u;

	//PS_CONSTANT_LIGHT_BUFFER lightBuff;
	//lightBuff.camPos = drawFromCamera.cameraPosition;

	//DxHandler::contextPtr->UpdateSubresource(constantPixelBuffer, 0, NULL, &lightBuff, 0, 0);

	DxHandler::contextPtr->IASetVertexBuffers(0, 1, &fullscreenQuad->vertexBuffer,
		&stride, &offset);

	DxHandler::contextPtr->Draw(fullscreenQuad->vertices.size(), 0);
}

void DxHandler::setDefaultState()
{
	DxHandler::contextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DxHandler::contextPtr->IASetInputLayout((ID3D11InputLayout*)DxHandler::input_layout_ptr);
	//contextPtr->OMSetBlendState(NULL, NULL, NULL);
	contextPtr->RSSetState(rasterizerState);
	
	VS_CONSTANT_MATRIX_BUFFER buff;
	PS_CONSTANT_LIGHT_BUFFER lBuff;
	DxHandler::contextPtr->VSSetConstantBuffers(PER_OBJECT_CBUFFER_SLOT, 1, &constantVertexBuffer);
	contextPtr->VSSetConstantBuffers(1, 1, &constantAnimBuffer);
	DxHandler::contextPtr->ClearDepthStencilView(DxHandler::depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	contextPtr->PSSetConstantBuffers(0, 1, &constantPixelBuffer);
	
	contextPtr->GSSetConstantBuffers(0, 1, &GSConstBuff);

	
	DxHandler::contextPtr->PSSetSamplers(0, 1, &standardSampler);
}

void DxHandler::initAdditiveBlendState()
{
	//states = std::make_unique<CommonStates>(device);
	D3D11_BLEND_DESC additiveBlendDesc{ 0 };
	//additiveBlendDesc.RenderTarget
	additiveBlendDesc.IndependentBlendEnable = true;
	additiveBlendDesc.RenderTarget[0].BlendEnable = true;
	additiveBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;//D3D11_BLEND_SRC_COLOR;//D3D11_BLEND_ONE;//D3D11_BLEND_DEST_COLOR;
	additiveBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;//D3D11_BLEND_SRC_COLOR;
	additiveBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX; //Pick the max of the color values
	additiveBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	additiveBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	additiveBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	additiveBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	

	HRESULT succ = devicePtr->CreateBlendState(&additiveBlendDesc, &additiveBlendState);
	assert(SUCCEEDED(succ));
	//DxHandler::additiveBlendDesc = additiveBlendDesc;
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

ID3D11Buffer* DxHandler::createVSAnimBuffer(VS_CONSTANT_ANIM_BUFFER& matrix)
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
	DxHandler::contextPtr->VSSetConstantBuffers(1, 1, &constantBuffer); //Bind to slot 1

	//loadedVSBuffers.push_back(constantBuffer);
	this->constantAnimBuffer = constantBuffer;

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

ID3D11Buffer*& DxHandler::createGSConstBuffer()
{
	D3D11_BUFFER_DESC constGeometryDesc;
	constGeometryDesc.ByteWidth = sizeof(GS_CONSTANT_MATRIX_BUFFER);
	constGeometryDesc.Usage = D3D11_USAGE_DEFAULT;

	constGeometryDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constGeometryDesc.CPUAccessFlags = 0;
	constGeometryDesc.MiscFlags = 0;
	constGeometryDesc.StructureByteStride = 0;

	ID3D11Buffer* constantGeometryBuffer = NULL;
	HRESULT buffPSucc = devicePtr->CreateBuffer(&constGeometryDesc, NULL,
		&constantGeometryBuffer);
	assert(SUCCEEDED(buffPSucc));

	contextPtr->GSSetConstantBuffers(0, 1, &constantGeometryBuffer);

	GSConstBuff = constantGeometryBuffer;

	return constantGeometryBuffer;
}

void DxHandler::draw(Mesh* drawMesh, Camera drawFromCamera, bool isSky, Light* light)
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
	lightBuff.hasTexture = drawMesh->hasTexture;

	if (lightBuff.hasTexture)
	{
		contextPtr->PSSetShaderResources(0, 1, &drawMesh->textureView);
	}

	lightBuff.hasNormalMap = drawMesh->hasNormalMap;
	if (lightBuff.hasNormalMap)
	{
		contextPtr->PSSetShaderResources(3, 1, &drawMesh->NormalView);
	}

	if (light != nullptr)
	{
		lightBuff.lightPos = DirectX::XMVectorSet(light->pos.x, light->pos.y, light->pos.z, 0);
		lightBuff.lightColor = light->lightColor;
	}
		

	GS_CONSTANT_MATRIX_BUFFER gBuff;
	gBuff.camPos = drawFromCamera.cameraPosition;  
	//std::cout << DirectX::XMVectorGetX(gBuff.camPos) << " " << DirectX::XMVectorGetY(gBuff.camPos) << " " << DirectX::XMVectorGetZ(gBuff.camPos) << std::endl;

	DxHandler::contextPtr->UpdateSubresource(constantPixelBuffer, 0, NULL, &lightBuff, 0, 0);
	DxHandler::contextPtr->UpdateSubresource(constantVertexBuffer, 0, NULL, &matrixBuff, 0, 0);
	DxHandler::contextPtr->UpdateSubresource(GSConstBuff, 0, NULL, &gBuff, 0, 0);

	//Set vertex buffer to the mesh you want to draw
	DxHandler::contextPtr->IASetVertexBuffers(0, 1, &drawMesh->vertexBuffer,
		&stride, &offset);
	//Draw it
	DxHandler::contextPtr->Draw(drawMesh->vertices.size(), 0);

}

void DxHandler::draw(AnimatedMesh* drawMesh, Camera drawFromCamera, Light* light)
{
	UINT stride = (UINT)sizeof(float) * FLOATS_PER_VERTEX;
	UINT offset = 0u;

	VS_CONSTANT_MATRIX_BUFFER matrixBuff;
	matrixBuff.worldViewProjectionMatrix = drawMesh->getWorldMatrix() * drawFromCamera.cameraView * drawFromCamera.cameraProjectionMatrix;
	matrixBuff.worldMatrix = drawMesh->getWorldMatrix();
	matrixBuff.viewMatrix = drawFromCamera.cameraView;
	matrixBuff.projMatrix = drawFromCamera.cameraProjectionMatrix;

	PS_CONSTANT_LIGHT_BUFFER lightBuff;
	lightBuff.camPos = drawFromCamera.cameraPosition;
	lightBuff.hasTexture = drawMesh->hasTexture;
	lightBuff.hasNormalMap = drawMesh->hasNormalMap;

	lightBuff.hasNormalMap = drawMesh->hasNormalMap;
	if (lightBuff.hasNormalMap)
	{
		contextPtr->PSSetShaderResources(3, 1, &drawMesh->NormalView);
	}

	if (lightBuff.hasTexture)
	{
		contextPtr->PSSetShaderResources(0, 1, &drawMesh->textureView);
	}

	if (light != nullptr)
	{
		lightBuff.lightPos = DirectX::XMVectorSet(light->pos.x, light->pos.y, light->pos.z, 0);
		lightBuff.lightColor = light->lightColor;
	}



	VS_CONSTANT_ANIM_BUFFER animBuff;
	animBuff.time = drawMesh->t;
	animBuff.currentTargetIndex = drawMesh->targetPoseIndex;
	animBuff.vertexOffsetPerModel = drawMesh->nrOfVertices;

	contextPtr->VSSetShaderResources(0, 1, &drawMesh->srv);


	GS_CONSTANT_MATRIX_BUFFER gBuff;
	gBuff.camPos = drawFromCamera.cameraPosition;
	//std::cout << DirectX::XMVectorGetX(gBuff.camPos) << " " << DirectX::XMVectorGetY(gBuff.camPos) << " " << DirectX::XMVectorGetZ(gBuff.camPos) << std::endl;

	DxHandler::contextPtr->UpdateSubresource(constantPixelBuffer, 0, NULL, &lightBuff, 0, 0);
	DxHandler::contextPtr->UpdateSubresource(constantVertexBuffer, 0, NULL, &matrixBuff, 0, 0);
	DxHandler::contextPtr->UpdateSubresource(constantAnimBuffer, 0, NULL, &animBuff, 0, 0);
	DxHandler::contextPtr->UpdateSubresource(GSConstBuff, 0, NULL, &gBuff, 0, 0);

	//Set vertex buffer to the mesh you want to draw
	DxHandler::contextPtr->IASetVertexBuffers(0, 0, nullptr, //BIND NOTHING FOR VERTEX PULLING
		&stride, &offset);
	//Draw it
	DxHandler::contextPtr->Draw(drawMesh->nrOfVertices, 0);
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
	DxHandler::swapDesc.BufferDesc.RefreshRate.Denominator = 1;
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
