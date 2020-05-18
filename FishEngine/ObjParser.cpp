#include "ObjParser.h"
#include <algorithm>
#include <DirectXMath.h>
#include <DirectXHelpers.h>

using namespace DirectX;

struct float3
{
	float x, y, z = 0;
};

struct float2
{
	float x, y = 0;
};

std::vector<Vertex> ObjParser::readFromObj(std::string fileName)
{
	std::vector<float3> loadedVertsCoords;
	std::vector<float3> loadedVertNormals;
	std::vector<float2> loadedVertTextureCoords;

	std::ifstream objFile;
	objFile.open(fileName);
	assert(objFile.is_open());
	std::string line;

	float v1, v2, v3;

	std::vector<Vertex> vertices;

	while (!objFile.eof() && std::getline(objFile, line))
	{
		//std::cout << line << std::endl;
		if (line.size() > 0)
		{
			if (line.at(0) != '#')
			{
				//std::cout << line.at(0) << std::endl;
				if (line.find("v ") != -1) //If it's a vertex position.
				{
					line.erase(0, 2); //Removes v and spaces
					int tempLen = line.find(' ');
					std::string tempStr = line.substr(0, tempLen);
					v1 = (float)std::stof(tempStr);

					line.erase(0, tempLen + 1);
					tempLen = line.find(' ');
					tempStr = line.substr(0, tempLen);
					v2 = (float)std::stof(tempStr);

					line.erase(0, tempLen + 1);
					tempLen = line.find(' ');
					tempStr = line.substr(0, tempLen);
					v3 = (float)std::stof(tempStr);
					loadedVertsCoords.push_back(float3{ v1, v2, v3 });
					//std::cout << "VertexPos: " << v1 << "\t" << v2 << "\t" << v3 << std::endl;
				}
				else if (line.at(0) == 'g')
				{
					//It's a group, irrelevant to us
				}
				else if (line.compare(0, 2, "vn") == 0) //If it's a vertex normal.
				{
					line.erase(0, 3); //Removes vn and spaces
					int tempLen = line.find(' ');
					std::string tempStr = line.substr(0, tempLen);
					v1 = std::stof(tempStr);

					line.erase(0, tempLen + 1);
					tempLen = line.find(' ');
					tempStr = line.substr(0, tempLen);
					v2 = std::stof(tempStr);

					line.erase(0, tempLen + 1);
					tempLen = line.find(' ');
					tempStr = line.substr(0, tempLen);
					v3 = std::stof(tempStr);

					//its a normal
					//objFile >> ignoreChar >> ignoreChar2 >> v1 >> v2 >> v3; //Will read v, x, y, z ignore v
					loadedVertNormals.push_back(float3{ v1, v2, v3 });
				}
				else if (line.find("mtllib") != -1) //Parse mtl, we only support one material
				{
					line.erase(0, 7); //Deletes usemtl + spaces
					std::ifstream mtlFile;
					//mtlFile.open(("./TestModel/cube.mtl"));
					mtlFile.open(("./Models/" + line));
					std::string mtlLine;

					assert(mtlFile.is_open());

					while (!mtlFile.eof() && std::getline(mtlFile, mtlLine))
					{
						if (mtlLine.find("Ka") != -1) //Ambient light color
						{
							mtlLine.erase(0, mtlLine.find("Ka") + 3);
							int numToEmpty = mtlLine.find(' ');
							float r = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							numToEmpty = mtlLine.find(' ');
							float g = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							numToEmpty = mtlLine.find(' ');
							float b = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							////writeToMesh.ambientMeshColor = DirectX::XMFLOAT4(r, g, b, 1);
						}

						if (mtlLine.find("Kd ") != -1 && mtlLine.find("map") == -1) //Diffuse light color
						{
							mtlLine.erase(0, mtlLine.find("Kd") + 3);
							int numToEmpty = mtlLine.find(' ');
							float r = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							numToEmpty = mtlLine.find(' ');
							float g = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							numToEmpty = mtlLine.find(' ');
							float b = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							////writeToMesh.diffuseMeshColor = DirectX::XMFLOAT4(r, g, b, 1);
						}

						if (mtlLine.find("Ks") != -1) //Specular light color
						{
							mtlLine.erase(0, mtlLine.find("Ks") + 3);
							int numToEmpty = mtlLine.find(' ');
							float r = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							numToEmpty = mtlLine.find(' ');
							float g = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							numToEmpty = mtlLine.find(' ');
							float b = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							//writeToMesh.specularMeshColor = DirectX::XMFLOAT4(r, g, b, 1);
						}

						if (mtlLine.find("Ns") != -1) //Shininess of material
						{
							mtlLine.erase(0, mtlLine.find("Ns") + 3);
							float shininess = 0;

							shininess = stof(mtlLine);
							//writeToMesh.shininess = shininess;
						}

						if (mtlLine.find("map_Kd") != -1) //Texture
						{
							mtlLine.erase(0, mtlLine.find("map_Kd") + 7);

							////writeToMesh.textureName = mtlLine;
						}
					}

					mtlFile.close();
				}
				else if (line.compare(0, 2, "vt") == 0) // Vertex texture coordinates [UV]
				{
					line.erase(0, 3); //Removes vt and spaces
					int tempLen = line.find(' ');
					std::string tempStr = line.substr(0, tempLen);
					v1 = std::stof(tempStr);

					line.erase(0, tempLen + 1);
					tempLen = line.find(' ');
					tempStr = line.substr(0, tempLen);
					v2 = std::stof(tempStr);

					loadedVertTextureCoords.push_back(float2{ v1, v2 });
					//std::cout << v1 << " : " << v2 << std::endl;
					//std::cout << std::endl;
				}
				else if (line.at(0) == 'f') //If it's a face.
				{
					//assemble vertices

					line.erase(0, 2);//Removes f and two first spaces

					for (int k = 0; k < 3; k++) //Loops 3 times, because 3 v / vt / vn groups
					{
						Vertex tempVert; //Build new vert for each

						std::string tempString;
						for (int i = 0; i < 3; i++) //for each of our v / vt / vn
						{
							int tempLen = line.find('/'); // ger oss v/vn/vt
							if (line.find(' ') < tempLen)
							{
								tempLen = line.find(" ");

							}
							tempString = line.substr(0, tempLen); //Want to go up to the slash, not including it

							if (i == 0) //It's v index
							{
								if (tempLen != 0) //Make sure there is a number
								{
									tempVert.x = loadedVertsCoords.at((std::stoi(tempString)) - 1).x; //-1 to convert from index starting with 1 to 0
									tempVert.y = loadedVertsCoords.at((std::stoi(tempString)) - 1).y;
									tempVert.z = loadedVertsCoords.at((std::stoi(tempString)) - 1).z;
								}

							}
							if (i == 1) //It's vt index
							{

								if (tempString.length() != 0)//(tempLen != 0)
								{
									tempVert.u = loadedVertTextureCoords.at((std::stoi(tempString)) - 1).x;
									tempVert.v = 1 - loadedVertTextureCoords.at((std::stoi(tempString)) - 1).y;

								}
								else
								{
									tempVert.u = 0;
									tempVert.v = 0;
								}
							}
							if (i == 2) //It's vn index
							{
								if (tempString.length() != 0)//(tempLen != 0)
								{
									tempVert.nx = loadedVertNormals.at((std::stoi(tempString)) - 1).x;
									tempVert.ny = loadedVertNormals.at((std::stoi(tempString)) - 1).y;
									tempVert.nz = loadedVertNormals.at((std::stoi(tempString)) - 1).z;
								}
							}
							line.erase(0, tempLen + 1); //removes number and slash from string
							if (line.at(0) == ' ')
								line.erase(0, 1); //Remove the last space in between groups of v/vt/vn
						}
						tempVert.r = 0.5;
						tempVert.g = 1;
						tempVert.b = 1;

						vertices.push_back(tempVert);
					}
				}
			}
		}

	}

	for (size_t i = 0; i < vertices.size(); i += 3) //Reading one triangle at a time
	{
		Vertex* vertex1 = &vertices.at(i);
		Vertex* vertex2 = &vertices.at(i + 1);
		Vertex* vertex3 = &vertices.at(i + 2);

		//Is tangent?
		DirectX::XMVECTOR v1Tov2 = DirectX::XMVectorSet(vertex2->x - vertex1->x, vertex2->y - vertex1->y, vertex2->z - vertex1->z, 0);

		DirectX::XMVECTOR v1Tov3 = DirectX::XMVectorSet(vertex3->x - vertex1->x, vertex3->y - vertex1->y, vertex3->z - vertex1->z, 0);

		//Normal for entire face/triangle
		DirectX::XMVECTOR normal = DirectX::XMVectorSet(vertex1->nx, vertex1->ny, vertex1->nz, 0);
		DirectX::XMVECTOR textureSpaceVec1to2 = DirectX::XMVectorSet(vertex2->u - vertex1->u, vertex2->v - vertex1->v, 0, 0);
		DirectX::XMVECTOR textureSpaceVec1to3 = DirectX::XMVectorSet(vertex3->u - vertex1->u, vertex3->v - vertex1->v, 0, 0);

		float denominator = 1.f / (DirectX::XMVectorGetX(textureSpaceVec1to2) * DirectX::XMVectorGetY(textureSpaceVec1to3) -
			DirectX::XMVectorGetY(textureSpaceVec1to2) * DirectX::XMVectorGetX(textureSpaceVec1to3));

		DirectX::XMVECTOR tangent = (v1Tov2 * DirectX::XMVectorGetY(textureSpaceVec1to3) - (v1Tov3 * DirectX::XMVectorGetY(textureSpaceVec1to2))) * denominator;
		tangent = DirectX::XMVector3Normalize(tangent);

		//Tangent
		float tx = DirectX::XMVectorGetX(tangent);
		float ty = DirectX::XMVectorGetY(tangent);
		float tz = DirectX::XMVectorGetZ(tangent);

		vertex1->tx = tx;
		vertex1->ty = ty;
		vertex1->tz = tz;

		vertex2->tx = tx;
		vertex2->ty = ty;
		vertex2->tz = tz;

		vertex3->tx = tx;
		vertex3->ty = ty;
		vertex3->tz = tz;
	}

	objFile.close();

	return vertices;
}
