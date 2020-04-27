
Texture2D colorMap : register(t0);
TextureCube sky : register(t1);
SamplerState mysampler;

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

struct PS_OUTPUT //Output to gbuffers
{
	float4 vPosition : SV_TARGET0;
	float4 vColour : SV_TARGET1;
	float4 vNormal : SV_TARGET2;
};


PS_OUTPUT main(PS_INPUT input) : SV_Target
{

	PS_OUTPUT output;
	output.vPosition = input.positionInWorldSpace;

	if (hasTexture == true)
	{
		output.vColour = colorMap.Sample(mysampler, input.vUV.xy);
	}
	else
	{
		output.vColour = float4(0.8, 0.8, 0.8, 1);
	}

	if (isSky)
	{
		float3 camToPixelVec = (input.positionInWorldSpace.xyz - camPos.xyz);
		float3 camToPixelReflected = normalize(reflect(camToPixelVec, input.vNormal.xyz));

		output.vColour.w = 2;
		//input.vUV = float4(input.vUV.x - 1, input.vUV.y, input.vUV.z, 0);
		//output.vColour = sky.Sample(mysampler, input.vUV);
		output.vColour = sky.Sample(mysampler, normalize(camToPixelVec));
		//output.vColour = sky.Sample(mysampler, input.vPosition);
		output.vColour.w = 2;
	}

	/*if (hasNormalMap)
	{
		float3 loadedNormal = NormalMapTexture.Sample(mysampler, input.vUV);
		float3 tangent = normalize(input.vTangent - dot(input.vTangent, input.vNormal) * input.vNormal);
		float3 bitangent = normalize(cross(loadedNormal, tangent));

		float3x3 tbn = float3x3(tangent, bitangent, loadedNormal); //This will move things into 'texture space' or 'tangent space' in order to support rotations of the object without distortion.

		input.vNormal = normalize(float4(mul(loadedNormal, tbn), 0));
	}*/

	output.vNormal = input.vNormal;
	return output;

}