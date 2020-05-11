

cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 worldViewProjectionMatrix;
	row_major float4x4 worldMatrix;
	row_major float4x4 viewMatrix;
	row_major float4x4 projMatrix;
	bool isSky = false;
};

cbuffer VS_CONSTANT_ANIM_BUFFER : register(b1)
{
	int vertexOffsetPerModel;
	double time;
	double animSpeed;
	int currentTargetIndex;
};

struct VS_INPUT
{
	float3 vPosition;
	float4 vColour;
	float2 vUV;
	float3 vNormal;
	float3 vTangent;
};

StructuredBuffer<VS_INPUT> poses : register(t0);

struct VS_OUTPUT //Outputs to ordinary first pass pixel shader.
{
	float4 vPosition : SV_POSITION;
	float4 vColour : COLOR;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
	float4 positionInWorldSpace : POSITION;
	float4 vTangent : TANGENT;
};

VS_OUTPUT main(uint vertexID : SV_VertexID)
{
	VS_INPUT input = poses[vertexID];

	VS_OUTPUT Output; 

    float4 blendPos1 = float4(poses[vertexID].vPosition, 1); //Neutral
    float4 blendPos2 = float4(poses[vertexOffsetPerModel * currentTargetIndex + vertexID].vPosition, 1);
	
    float3 blendNormal1 = poses[vertexID].vNormal;
    float3 blendNormal2 = poses[vertexOffsetPerModel * currentTargetIndex + vertexID].vNormal;
	
    float4 blendedPos = lerp(blendPos1, blendPos2, time);
    float3 blendedNormal = normalize(lerp(blendNormal1, blendNormal2, time));
	
	Output.vNormal = normalize(mul(float4(normalize(blendedNormal), 0), worldMatrix));
	Output.vTangent = normalize(mul(float4(normalize(input.vTangent), 0), worldMatrix));
	Output.vPosition = mul(blendedPos, worldViewProjectionMatrix);

	Output.positionInWorldSpace = mul(blendedPos, worldMatrix).xyww;


    Output.vColour = input.vColour;
	Output.vUV = float4(input.vUV, 1, 1);

	return Output;
}