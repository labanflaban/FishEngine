TextureCube colorMap : register(t3);
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
    bool hasNormalMap;
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
    float3 camToPixelVec = (input.positionInWorldSpace.xyz - camPos.xyz);
    float3 camToPixelReflected = normalize(reflect(camToPixelVec, input.vNormal.xyz));

	//	output.vColour = sky.Sample(mysampler, normalize(camToPixelVec));
	//	output.vColour.w = 2;
    
    return colorMap.Sample(mysampler, normalize(camToPixelVec));

}