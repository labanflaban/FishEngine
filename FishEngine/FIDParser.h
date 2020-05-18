#pragma once
#include <FIDImporter.h>
#include "Vertex.h"
#include <vector>
#include <iostream>
<<<<<<< Updated upstream
=======
#include <assert.h>

>>>>>>> Stashed changes
class FIDParser
{
public:
	static std::vector<Vertex> readFromFID(std::string fileName);
};