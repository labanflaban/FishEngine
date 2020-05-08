#pragma once
#include <FIDImporter.h>
#include "Vertex.h"
#include <vector>
#include <iostream>
class FIDParser
{
public:
	static std::vector<Vertex> readFromFID(std::string fileName);
};