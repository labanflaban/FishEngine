// FIDImporter.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "FIDImporter.h"

#include <iostream>

#include <cstdlib>
#include <fstream>
using namespace std;

namespace FID
{
	void printMessage()
	{
		cout << "HELLO WORLD" << endl;
	}

	void Mesh::printMesh()
	{
		for (int i = 0; i < this->meshHeader.vertexCount; i++)
		{
			cout << "Vertex : " << i << endl << endl;
			cout << vertices[i].positon[0] << ", " << vertices[i].positon[1] << ", " << vertices[i].positon[2] << endl;
			cout << vertices[i].UV[0] << ", " << vertices[i].UV[1] << endl;
			cout << vertices[i].normal[0] << ", " << vertices[i].normal[1] << ", " << vertices[i].normal[2] << endl;
			cout << vertices[i].tangents[0] << ", " << vertices[i].tangents[1] << ", " << vertices[i].tangents[2] << endl;
			cout << vertices[i].biTangents[0] << ", " << vertices[i].biTangents[1] << ", " << vertices[i].biTangents[2] << endl << endl;
		}
	}

	FIDHeader::Vertex* Mesh::getVertexPoints()
	{
		return this->vertices;
	}

	const int Mesh::getVertexCount()
	{
		return this->meshHeader.vertexCount;
	}

	Mesh::Mesh(std::string filePath)
	{
		ifstream infile(filePath, ios::out | ios::binary);

		if (!infile)
		{
			cout << "Cannot open file!" << endl;
		}
		else
		{
			infile.read((char*)&meshHeader, sizeof(FIDHeader::MeshHeader));

			this->vertices = new FIDHeader::Vertex[meshHeader.vertexCount];

			infile.read((char*)&this->vertices->positon, sizeof(FIDHeader::Vertex) * meshHeader.vertexCount);

			infile.close();
		}
	}

	Mesh::~Mesh()
	{
		delete[] this->vertices;
	}

	std::string Texture::getTextureName(int index)
	{
		return this->textures[index].name;
	}

	std::string Texture::getTextureFilePath(int index)
	{
		return this->textures[index].filePath;
	}

	Texture::Texture(std::string filePath)
	{
		ifstream infile(filePath, ios::out | ios::binary);

		if (!infile)
		{
			cout << "Cannot open file!" << endl;
		}
		else
		{
			infile.read((char*)&textureHeader, sizeof(FIDHeader::TextureHeader));
			this->textures = new FIDHeader::Texture[2];
			infile.read((char*)&this->textures->filePath, sizeof(FIDHeader::Texture) * 2);
		}
	}
	Texture::~Texture()
	{
		delete[] this->textures;
	}
}
