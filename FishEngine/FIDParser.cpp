#include "FIDParser.h"

std::vector<Vertex> FIDParser::readFromFID(std::string fileName)
{
	std::vector<Vertex> vertexList;
	FID::Mesh* tempMesh = new FID::Mesh(fileName);
	Vertex tempVertex;

	for (int i = 0; i < tempMesh->getVertexCount(); i++)
	{
		tempVertex.x = tempMesh->getVertexPoints()[i].positon[0];
		tempVertex.y = tempMesh->getVertexPoints()[i].positon[1];
		tempVertex.z = tempMesh->getVertexPoints()[i].positon[2];

		tempVertex.u = tempMesh->getVertexPoints()[i].UV[0];
		tempVertex.v = 1-tempMesh->getVertexPoints()[i].UV[1];

		tempVertex.nx = tempMesh->getVertexPoints()[i].normal[0];
		tempVertex.ny = tempMesh->getVertexPoints()[i].normal[1];
		tempVertex.nz = tempMesh->getVertexPoints()[i].normal[2];

		tempVertex.tx = tempMesh->getVertexPoints()[i].tangents[0];
		tempVertex.ty = tempMesh->getVertexPoints()[i].tangents[1];
		tempVertex.tz = tempMesh->getVertexPoints()[i].tangents[2];

		vertexList.push_back(tempVertex);
	}

	return vertexList;
}
