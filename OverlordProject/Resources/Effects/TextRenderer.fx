float4x4 gTransform : WORLDVIEWPROJECTION;
float4x4 gViewInverse : ViewInverse;
float4x4 gView : View;
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
	DepthEnable = FALSE;
};

RasterizerState BackCulling 
{ 
	CullMode = BACK; 
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
	int Channel : TEXCOORD2; //Texture Channel
	float3 Position : POSITION; //Left-Top Character Quad Starting Position
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Left-Top Character Texture Coordinate on Texture
	float2 CharSize: TEXCOORD1; //Size of the character (in screenspace)
};

struct GS_DATA
{
	float4 Position : SV_POSITION; //HOMOGENEOUS clipping space position
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Texcoord of the vertex
	int Channel: TEXCOORD1; //Channel of the vertex
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, int channel)
{
	//Create a new GS_DATA object
	GS_DATA obj = (GS_DATA)0;
	//Fill in all the fields
	obj.Position = mul(float4(pos, 1), gTransform);
	obj.Color = col;
	obj.Channel = channel;
	obj.TexCoord = texCoord;
	//Append it to the TriangleStream
	triStream.Append(obj);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//Create a Quad using the character information of the given vertex
	//Note that the Vertex.CharSize is in screenspace, TextureCoordinates aren't ;) [Range 0 > 1]
	//Thank you Miel for helping with this
	float3 posUL = vertex[0].Position;
	float3 posUR = vertex[0].Position;
	float3 posBL = vertex[0].Position;
	float3 posBR = vertex[0].Position;

	posBR.xy += vertex[0].CharSize;
	posUR.x += vertex[0].CharSize.x;
	posBL.y += vertex[0].CharSize.y;

	float2 charSize = vertex[0].CharSize / gTextureSize;
	float2 texCoordUL = vertex[0].TexCoord;
	float2 texCoordUR = vertex[0].TexCoord;
	float2 texCoordBL = vertex[0].TexCoord;
	float2 texCoordBR = vertex[0].TexCoord;

	texCoordBR += charSize;
	texCoordUR.x += charSize.x;
	texCoordBL.y += charSize.y;

	//1. Vertex Left-Top
	CreateVertex(triStream, posUL, vertex[0].Color, texCoordUL, vertex[0].Channel);
	//2. Vertex Right-Top
	CreateVertex(triStream, posUR, vertex[0].Color, texCoordUR, vertex[0].Channel);
	//3. Vertex Left-Bottom
	CreateVertex(triStream, posBL, vertex[0].Color, texCoordBL, vertex[0].Channel);
	//4. Vertex Right-Bottom
	CreateVertex(triStream, posBR, vertex[0].Color, texCoordBR, vertex[0].Channel);
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET{

	//Sample the texture and return the correct channel [Vertex.Channel]
	float4 tempColor = gSpriteTexture.Sample(samPoint, input.TexCoord);
	//You can iterate a float4 just like an array, using the index operator
	float channel = tempColor[input.Channel];
	//Also, don't forget to colorize ;) [Vertex.Color]
	float4 color = mul(input.Color, channel);
	return color;
}

// Default Technique
technique10 Default {

	pass p0 {
		SetRasterizerState(BackCulling);
		SetBlendState(EnableBlending,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetDepthStencilState(NoDepth, 0);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}
