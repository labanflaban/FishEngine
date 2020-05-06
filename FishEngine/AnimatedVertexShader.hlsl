

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
	float3 vPosition : POSITION;
	float4 vColour : COLOR;
	float2 vUV : UV;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
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

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT Output; 

	Output.vNormal = normalize(mul(float4(normalize(input.vNormal), 0), worldMatrix));
	Output.vTangent = normalize(mul(float4(normalize(input.vTangent), 0), worldMatrix));
	Output.vPosition = mul(float4(input.vPosition, 1), worldViewProjectionMatrix);

	if (!isSky)
		Output.positionInWorldSpace = mul(float4(input.vPosition, 1), worldMatrix);
	else
	{
		Output.positionInWorldSpace = mul(float4(input.vPosition, 1), worldMatrix).xyww;
		Output.vUV = float4(input.vPosition, 1);
	}


	Output.vColour = input.vColour;
	Output.vUV = float4(input.vUV, 1, 1);

	return Output;
}