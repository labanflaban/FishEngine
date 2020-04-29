Texture2D colorMap : register(t0);
sampler mysampler;
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
}

struct PS_INPUT //Output from geometry shader
{
	float4 vPosition : SV_POSITION;
	float4 vColour : COLOR;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
	float4 positionInWorldSpace : POSITION;
	float4 vTangent : TANGENT;
};

float4 main(PS_INPUT input) : SV_Target
{
	return float4(1.f,0.f,0.f,0.0f); //Change last value to change opacity

}