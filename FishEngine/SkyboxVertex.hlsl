
cbuffer VS_CONSTANT_BUFFER
{
    row_major float4x4 worldViewProjectionMatrix;
    row_major float4x4 worldMatrix;
    row_major float4x4 viewMatrix;
    row_major float4x4 projMatrix;
    bool isSky = false;
    bool enemyHit;
}

struct VS_INPUT
{
    float3 vPosition : POSITION;
    float4 vColour : COLOR;
    float2 vUV : UV;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 vPosition : SV_POSITION;
    float4 vColour : COLOR;
    float4 vUV : UV;
    float4 vNormal : NORMAL;
    float4 positionInWorldSpace : POSITION;
    float4 vTangent : TANGENT;
};

//Pure passthrough, world matrix is indentity matrix when doing this.
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    //output.positionInWorldSpace = float4(input.vPosition, 1); //mul(float4(input.vPosition, 1), worldMatrix);
    //output.vPosition = float4(input.vPosition, 1);
    //output.vPosition.z = 0.9999999f; //99999f;
	
    //output.vColour = input.vColour;
    //output.vColour.w = 2;
	
    //output.vUV = float4(input.vUV, 0, 1);
    //output.vNormal = float4(input.vNormal, 0); //mul(float4(input.vNormal, 0), worldMatrix);
    //output.vTangent = float4(input.vTangent, 0);
    
    //test --------------------------------------------------------------------------
    output.vNormal = normalize(mul(float4(normalize(input.vNormal), 0), worldMatrix));
    output.vTangent = normalize(mul(float4(normalize(input.vTangent), 0), worldMatrix));
    
    output.vPosition.z = 1.f;
    output.vPosition = mul(float4(input.vPosition, 1), worldViewProjectionMatrix);
    output.positionInWorldSpace = mul(float4(input.vPosition, 1), worldMatrix);
    output.vUV = float4(input.vUV, 0, 0);
    output.vColour = float4(1, 1, 1, 1);
    //output.vPosition.w = 0.9999f;
    //output.vPosition.z = 100.f;
   // output.positionInWorldSpace.z = 1.f;
    
    return output;
}