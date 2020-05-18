
//These are the gbuffers which hold pixel by pixel values to be read from.
Texture2D ColorTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D PositionTexture : register(t2);

SamplerState mySampler;

//From CPU to GPU. Sends whatever you need.
cbuffer PS_CONSTANT_BUFFER
{
	float4 lightPos;
	float4 lightColor;
	float4 globalLightPos;
	float4 ambientMeshColor;
	float4 diffuseMeshColor;
	float4 specularMeshColor;

	float4 camPos;
	row_major float4x4 worldViewProjectionMatrix;

	row_major float4x4 worldMatrix;
	row_major float4x4 viewMatrix;
	row_major float4x4 projMatrix;

	bool hasTexture;
	bool isSky;
	bool hasNormalMap;
}

struct VS_OUTPUT //comes from deferredVShader
{
	float4 vPosition : SV_POSITION;
	float4 vColour : COLOR;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
	float4 positionInWorldSpace : POSITION;
	float4 vTangent : TANGENT;
};

float4 main(VS_OUTPUT input) : SV_Target0
{

	//Load in values from gbuffer textures/SRVs.
	float4 albedo = ColorTexture.Load(float3(input.vPosition.xy, 0), 0);
	float4 normal = NormalTexture.Load(float3(input.vPosition.xy, 0), 0);
	float4 position = PositionTexture.Load(float3(input.vPosition.xy, 0), 0);


	//Calculate light
	float3 surfaceToLightV = normalize(globalLightPos - position);
	float diffuseStrength = clamp(dot(normal, surfaceToLightV), 0, 1); //Cannot go over 1, and never below 0.
	float ambientStrength = 0.1f; //Constant 'global' light.

	float4 lookVector = normalize(position - camPos); //Specular, camera to position
	float4 reflectionVec = normalize(reflect(float4(surfaceToLightV, 0), normal)); //Specular
	float specStrength = pow(clamp(dot(reflectionVec, lookVector), 0, 1), 32); // 32 being spec exponent
	//

	if (albedo.w != 2)
        return (diffuseStrength + ambientStrength + specStrength)*0.1* albedo; //((diffuseStrength + ambientStrength + specStrength) * albedo); //Final light output
	//else
	//if (albedo.w != 2)
        //return albedo * ambientStrength;
	else
        return albedo;
}
//* float4(0.117, 0.47, 0.83, 0) -- underwater color