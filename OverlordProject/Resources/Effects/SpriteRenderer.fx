float4x4 gTransform : WorldViewProjection;
Texture2D gSpriteTexture;
float2 gTextureSize;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

BlendState EnableBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

DepthStencilState NoDepth
{
    //Enable Depth Rendering
    DepthEnable = TRUE;
    //Disable Depth Writing
    DepthWriteMask = ZERO;
};

RasterizerState BackCulling
{
    CullMode = BACK;
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
    uint TextureId : TEXCOORD0;
    float4 TransformData : POSITION; //PosX, PosY, Depth (PosZ), Rotation
    float4 TransformData2 : POSITION1; //PivotX, PivotY, ScaleX, ScaleY
    float4 Color : COLOR;
};

struct GS_DATA
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD0;
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
    return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, float rotation, float2 rotCosSin, float2 offset, float2 pivotOffset)
{
    if (rotation != 0)
    {
		//Step 3.
		//Do rotation calculations
		//Transform to origin
		pos.xy -= (offset + pivotOffset);
		//Rotate
		float rotationX = (pos.x * rotCosSin.x) - pos.y * rotCosSin.y;
		float rotationY = (pos.y * rotCosSin.x) + pos.x * rotCosSin.y;
		float2 newPos = float2(rotationX, rotationY);
		pos.xy = newPos;
		//Retransform to initial position
		pos.xy += offset;
    }
    else
    {
		//Step 2.
		//No rotation calculations (no need to do the rotation calculations if there is no rotation applied > redundant operations)
		//Just apply the pivot offset
		pos.xy -= pivotOffset;
    }

	//Geometry Vertex Output
    GS_DATA geomData = (GS_DATA) 0;
    geomData.Position = mul(float4(pos, 1.0f), gTransform);
    geomData.Color = col;
    geomData.TexCoord = texCoord;
    triStream.Append(geomData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//Given Data (Vertex Data)
    float3 position = vertex[0].TransformData.xyz; //Extract the position data from the VS_DATA vertex struct
    float2 offset = vertex[0].TransformData.xy; //Extract the offset data from the VS_DATA vertex struct (initial X and Y position)
    float rotation = vertex[0].TransformData.w; //Extract the rotation data from the VS_DATA vertex struct
    float2 pivot = vertex[0].TransformData2.xy; //Extract the pivot data from the VS_DATA vertex struct
    float2 scale = vertex[0].TransformData2.zw; //Extract the scale data from the VS_DATA vertex struct
    float2 texCoord = float2(0, 0); //Initial Texture Coordinate
	
	//...

	// LT----------RT //TringleStrip (LT > RT > LB, LB > RB > RT)
	// |          / |
	// |       /    |
	// |    /       |
	// | /          |
	// LB----------RB

	float3 LTPosition = float3(position.x, position.y, position.z);
	float3 RTPosition = float3(position.x + (gTextureSize.x * scale.x), position.y, position.z);
	float3 LBPosition = float3(position.x, position.y + (gTextureSize.y * scale.y), position.z);
	float3 RBPosition = float3(position.x + (gTextureSize.x * scale.x), position.y + (gTextureSize.y * scale.y), position.z);
	
	float2 LTTexCoord = float2(0, 0);
	float2 RTTexCoord = float2(1, 0);
	float2 LBTexCoord = float2(0, 1);
	float2 RBTexCoord = float2(1, 1);
	
	float2 pivotOffset = gTextureSize * scale * pivot;
	
	float2 rotCosSin;
	if(rotation == 0)
		rotCosSin = float2(0,0);
	if(rotation != 0)
		rotCosSin = float2(cos(rotation), sin(rotation));
	
	
	//VERTEX 1 [LT]	
    CreateVertex(triStream, LTPosition, vertex[0].Color, LTTexCoord, rotation, rotCosSin, offset, pivotOffset); //Change the color data too!

	//VERTEX 2 [RT]
    CreateVertex(triStream, RTPosition, vertex[0].Color, RTTexCoord, rotation, rotCosSin, offset, pivotOffset); //Change the color data too!

	//VERTEX 3 [LB]
    CreateVertex(triStream, LBPosition, vertex[0].Color, LBTexCoord, rotation, rotCosSin, offset, pivotOffset); //Change the color data too!

	//VERTEX 4 [RB]
    CreateVertex(triStream, RBPosition, vertex[0].Color, RBTexCoord, rotation, rotCosSin, offset, pivotOffset); //Change the color data too!
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET
{

    return gSpriteTexture.Sample(samPoint, input.TexCoord) * input.Color;
}

// Default Technique
technique11 Default
{
    pass p0
    {
        SetRasterizerState(BackCulling);
        SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		//SetDepthStencilState(NoDepth,0);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(CompileShader(gs_4_0, MainGS()));
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}
