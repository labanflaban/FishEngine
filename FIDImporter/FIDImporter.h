#pragma once
#include "Header.h"
#include <fstream>
namespace FID
{
	struct Mesh
	{
		FIDHeader::MeshHeader meshHeader;
		FIDHeader::Vertex* vertices;

		// Prints to the console all the properties of all the vertex points in the mesh
		void printMesh();

		// Returns a pointer to the vertex array in the mesh
		FIDHeader::Vertex* getVertexPoints();


		// Returns the number of vertex points in the mesh
		const int getVertexCount();

		Mesh(std::string filePath);
		~Mesh();
	};

	void printMessage();
}