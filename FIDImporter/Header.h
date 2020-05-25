#pragma once

#define MESH_NAME_MAX_LENGTH 64
#define MATERIAL_NAME_MAX_LENGTH 32
#define SHADING_MODEL_NAME_MAX_LENGTH 32
#define TEXTURE_NAME_MAX_LENGTH 24
#define FILE_PATH_NAME_MAX_LENGTH 128
#define SURFACE_TYPE_NAME_MAX_LENGTH 64
#define TYPE_NAME 32
#define MAX_TEXTURE_CONNECTIONS 4

/*
* FORMATET KAN HETA '.FID', Filip, Isak, David... XD
*/
namespace FIDHeader
{
	enum  SectionType
	{
		UNKNOWN,
		FILE_INFO,
		MESH,
		MATERIAL,
		TEXTURE,
		MATERIAL_TEXTURE_CONNECTION,
		LIGHT,
		ANIMATION,
		CAMERA
	};

	struct SectionHeader
	{
		SectionType type;

		SectionHeader(SectionType _type)
		{
			type = _type;
		}
		SectionHeader()
		{
			type = UNKNOWN;
		}
	};
	//---------------------------------------------------------------------------------------

	struct MainHeader
	{
		unsigned int meshCount;
		unsigned int lightCount;
		unsigned int cameraCount;

		/*MeshHeader *mesh = new MeshHeader[meshCount];
		LightHeader *light = new LightHeader[lightCount];
		CameraHeader *camera = new CameraHeader[cameraCount];*/


	};

	struct Vertex
	{
		float positon[3];
		float UV[2];
		float normal[3];
		float tangents[3];
		float biTangents[3];
	};

	struct BlendShapeVertex
	{
		float position[3];
		float normal[3];
	};

	struct BlendShape
	{
		int BlendShapevertexCount;
	};

	struct MeshHeader : SectionHeader
	{
		int vertexCount;
		//unsigned int materialID;
		//char name[MESH_NAME_MAX_LENGTH];

		//add material

		MeshHeader() : SectionHeader(MESH) {}
	};

	struct MaterialHeader : SectionHeader
	{
		char name[MATERIAL_NAME_MAX_LENGTH];
		char shadingModel[SHADING_MODEL_NAME_MAX_LENGTH];

		float ambient[3];
		float diffuse[3];
		float specular[3];
		float emissive[3];
		float opacity;
		float shininess;
		unsigned int ID;

		MaterialHeader() : SectionHeader(MATERIAL) {}
	};

	struct Texture
	{
		char textureTyp[TYPE_NAME];
		char name[TEXTURE_NAME_MAX_LENGTH];
		char filePath[FILE_PATH_NAME_MAX_LENGTH];
	};

	struct TextureHeader : SectionHeader
	{
		unsigned int textureCount;

		TextureHeader() : SectionHeader(TEXTURE) {}
	};

	struct AnimationHeader : SectionHeader
	{
		int BlendShapeCount;
		AnimationHeader() : SectionHeader(ANIMATION) {}
	};

	struct LightHeader : SectionHeader
	{
		char lightType[TYPE_NAME];
		float position[3];
		float color[3];
		float intensity;
		float direction[3];

		LightHeader() : SectionHeader(LIGHT) {}
	};

	struct CameraHeader : SectionHeader
	{
		float position[3];
		float target[3];
		float upVector[3];

		float aspectRatio;
		float nearPlane;
		float farPlane;
		float fieldOfView;

		CameraHeader() : SectionHeader(CAMERA) {}
	};
}