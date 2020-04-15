#pragma once
#include <DirectXHelpers.h>
#include <SimpleMath.h>
#include <math.h>

class Camera
{
private:

public:
	DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(0, 0, -150, 0);
	DirectX::XMVECTOR cameraTarget = DirectX::XMVectorSet(0, 0, 1, 0);
	DirectX::XMVECTOR cameraUp = DirectX::XMVectorSet(0, 1, 0, 0);

	DirectX::XMMATRIX cameraView = DirectX::XMMatrixLookAtLH(Camera::cameraPosition, Camera::cameraTarget, Camera::cameraUp);
	DirectX::XMMATRIX cameraProjectionMatrix;


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

	Camera();
};