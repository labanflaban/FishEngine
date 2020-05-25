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

	void Animation::PrintBlendShapes()
	{
		int count = 0;
		for (int x = 0; x < animHeader.BlendShapeCount * currentBlendShape.BlendShapevertexCount; x += 3)
		{

			if (count == currentBlendShape.BlendShapevertexCount)
			{
				count = 0;
			}
			count += 3;
			cout << "POLYGON " << count / 3 << endl;
			for (int j = 0; j < 3; j++)
			{
				int faceindex = x + j;
				cout << "Coordinate " << blendVertex[faceindex].position[0] << " " << blendVertex[faceindex].position[1] << " " << blendVertex[faceindex].position[2] << " " << endl;
				cout << "Normal" << blendVertex[faceindex].normal[0] << " " << blendVertex[faceindex].normal[1] << " " << blendVertex[faceindex].normal[2] << " " << endl << endl;
			}
		}
	}



	Animation::Animation(std::string filePath)
	{
		ifstream infile(filePath, ios::out | ios::binary);
		if (!infile)
		{
			cout << "Cannot open Anim File" << endl;
		}
		else
		{

			infile.read((char*)&animHeader, sizeof(FIDHeader::AnimationHeader));

			infile.read((char*)&currentBlendShape, sizeof(FIDHeader::BlendShape));

			int allpoints = currentBlendShape.BlendShapevertexCount * animHeader.BlendShapeCount;

			this->blendVertex = new FIDHeader::BlendShapeVertex[allpoints];

			for (int x = 0; x < allpoints; x++)
			{
				infile.read((char*)&blendVertex[x], sizeof(FIDHeader::BlendShapeVertex));
			}

			//this->timeAndValue = new BlendTimeAndValue[currentBlendShape.KeyframeCount];

			//for (int x = 0; x < 4/* currentBlendShape.KeyframeCount*/; x++)
			//{
			//	infile.read((char*)&timeAndValue[x], sizeof(BlendTimeAndValue));
			//}

			infile.close();
		}
	}

	FIDHeader::BlendShapeVertex* Animation::getBlendShapeVertexPoints()
	{
		return blendVertex;
	}

	const int Animation::getBlendVertexCount()
	{
		return currentBlendShape.BlendShapevertexCount;
	}
	const int Animation::getBlendShapeCount()
	{
		return animHeader.BlendShapeCount;
	}
}
