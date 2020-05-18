#pragma once
#include "Vertex.h"
#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment(lib, "d3dcompiler.lib")
#include <iostream>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include<vector>

class Keyframes
{
private:
	
public:
	ID3D11Buffer* structuredBuffer = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	int nrOfPoses = 0;
	UINT nrOfVertices = 0;
	std::vector<Vertex> vertices;

<<<<<<< Updated upstream
	int targetPoseIndex = 0; //Neutral

	void createStructuredBuffer(ID3D11Device* device);
	void appendStructuredBuffer(std::vector<Vertex>* vertVectorsToMerge[], int nrOfVectorsToMerge);
=======
	bool decrementT = false;
	bool manualUpdate = false;

	int targetPoseIndex = 0; //Neutral
	double t = 0.0;
	double animationSpeed = 1;

	void createStructuredBuffer(ID3D11Device* device);
	void appendStructuredBuffer(std::vector<Vertex>* vertVectorsToMerge[], int nrOfVectorsToMerge);

	virtual void stepAnim(double deltaT = 0.0);
>>>>>>> Stashed changes
};