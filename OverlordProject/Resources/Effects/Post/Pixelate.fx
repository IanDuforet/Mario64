
//TECHNIQUE
//---------//=============================================================================
//// Shader uses position and texture
//=============================================================================


//SOURCE FOR PIXELATION SHADER
//https://www.geeks3d.com/20101029/shader-library-pixelation-post-processing-effect-glsl/


SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState EnableDepthWriting
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};
/// Create Rasterizer State (Backface culling) 
RasterizerState BackCulling
{
    CullMode = BACK;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    // Set the Position
    output.Position = float4(input.Position, 1);
    // Set the TexCoord
    output.TexCoord = input.TexCoord;
    return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input) : SV_Target
{
    //Variables
    float pixelWidth = 4.f;
    float pixelHeight = 4.f;

    //Get screen (renderTexture) sizes
    uint screenWidth;
    uint screenHeight;
    gTexture.GetDimensions(screenWidth, screenHeight);

    //Save UV's
    float2 uv = input.TexCoord.xy;

    //Calculate new uv/pixelsize
    float newX = pixelWidth * (1.f/screenWidth);
    float newY = pixelHeight * (1.f / screenHeight);
    
    //Calculat new UV by rounding the UV down depending on pixelsize
    float2 newUV = float2(newX * floor(uv.x/newX), newY * floor(uv.y / newY));

    //Sample color with new UV coords
    float3 finalColor = gTexture.Sample(samPoint, newUV);
    return float4(finalColor, 1.0f);
}


//TECHNIQUE
//---------
technique11 Blur
{
    pass P0
    {
        // Set states...
        SetRasterizerState(BackCulling);
        SetDepthStencilState(EnableDepthWriting, 0);
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}
technique11 Grayscale
{
    pass P0
    {
        // Set states...
        SetRasterizerState(BackCulling);
        SetDepthStencilState(EnableDepthWriting, 0);
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}

