#pragma once
#include "Header.h"
#include <fstream>
namespace FID
{
	struct Texture
	{
		FIDHeader::TextureHeader textureHeader;
		FIDHeader::Texture* textures;
		
		std::string getTextureName(int index);
		std::string getTextureFilePath(int index);

		Texture(std::string filePath);
		~Texture();
	};

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

	struct Animation
	{
		FIDHeader::AnimationHeader animHeader;
		FIDHeader::BlendShape currentBlendShape;
		FIDHeader::BlendShapeVertex* blendVertex;
		/*BlendTimeAndValue* timeAndValue;*/

		void PrintBlendShapes();

		Animation(std::string filePath);

		FIDHeader::BlendShapeVertex* getBlendShapeVertexPoints();
		/*BlendTimeAndValue* getTimeAndValue();*/

		const int getBlendVertexCount();
		const int getBlendShapeCount();
		/*const int getKeyFrameCount();*/
	};

	struct Camera
	{
		FIDHeader::CameraHeader CameraHead;

		Camera(std::string filepath);
		void PrintCamera();
	};

	void printMessage();
}