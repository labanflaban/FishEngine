#pragma once
#include <d3d11.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Vertex.h"
#include <assert.h>
#include <math.h>
#include <SimpleMath.h>
#include <DirectXHelpers.h>




struct Vertex;

class ObjParser
{
public:
	static std::vector<Vertex> readFromObj(std::string fileName);
};