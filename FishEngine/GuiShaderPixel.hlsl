Texture2D colorMap : register(t0);
sampler mysampler;
//From CPU to GPU. Sends whatever you need.
cbuffer PS_CONSTANT_BUFFER : register(b0)
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

cbuffer PS_GUI_BUFFER : register(b1)
{
    bool selected;
    bool isButton;
};

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
    float2 bruh;
    if (isButton)
    {
        if (selected)
        {
            bruh = (input.vUV.x, (input.vUV.y + 1) / 2);
        }
        else
        {
            bruh = (input.vUV.x, input.vUV.y / 2);
        }
    }
    else
    {
        bruh = input.vUV;
    }
    
    return colorMap.Sample(mysampler, bruh); //Change last value to change opacity
    //return float4(input.vUV.x, input.vUV.y, 0 ,0);
}