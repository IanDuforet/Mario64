#include "stdafx.h"
#include "SpriteFontLoader.h"
#include "BinaryReader.h"
#include "ContentManager.h"
#include "TextureData.h"

SpriteFont* SpriteFontLoader::LoadContent(const std::wstring& assetFile)
{
	auto pBinReader = new BinaryReader();
	pBinReader->Open(assetFile);

	if (!pBinReader->Exists())
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Failed to read the assetFile!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	char idByte1 = pBinReader->Read<char>();
	char idByte2 = pBinReader->Read<char>();
	char idByte3 = pBinReader->Read<char>();

	//If Identification bytes doesn't match B|M|F,
	if (idByte1 != 'B' && idByte2 != 'M' && idByte3 != 'F')
	{
		//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
		//return nullptr 
		return nullptr;
	}

	//Parse the version (version 3 required)
	char version =  pBinReader->Read<char>();

	//If version is < 3,
	if (version < 3)
	{
		//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
		//return nullptr
		return nullptr;
	}

	//Valid .fnt file
	auto pSpriteFont = new SpriteFont();
	//SpriteFontLoader is a friend class of SpriteFont
	//That means you have access to its privates (pSpriteFont->m_FontName = ... is valid)

	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	int blockId0 = pBinReader->Read<char>();
	int blockSize0 = pBinReader->Read<int>();
	UNREFERENCED_PARAMETER(blockId0);
	UNREFERENCED_PARAMETER(blockSize0);
	//Retrieve the FontSize (will be -25, BMF bug) [SpriteFont::m_FontSize]
	int fontsizeBlock = pBinReader->Read<uint16_t>();
	UNREFERENCED_PARAMETER(fontsizeBlock);
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	pBinReader->MoveBufferPosition(12);
	//Retrieve the FontName [SpriteFont::m_FontName]
	pSpriteFont->m_FontName = pBinReader->ReadNullString();

	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	int blockId1 = pBinReader->Read<char>();
	UNREFERENCED_PARAMETER(blockId1);
	int blockSize1 = pBinReader->Read<int>();
	UNREFERENCED_PARAMETER(blockSize1);
	//Retrieve Texture Width & Height [SpriteFont::m_TextureWidth/m_TextureHeight]
	pBinReader->MoveBufferPosition(4);
	pSpriteFont->m_TextureWidth = pBinReader->Read<uint16_t>();
	pSpriteFont->m_TextureHeight = pBinReader->Read<uint16_t>();
	//Retrieve PageCount
	int pageCount =  pBinReader->Read<uint16_t>();
	//> if pagecount > 1
	if (pageCount > 1)
	{
		//> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed)
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed");
		return nullptr;
	}
	//Advance to Block2 (Move Reader)
	pBinReader->MoveBufferPosition(5);

	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	int blockId2 = pBinReader->Read<char>();
	UNREFERENCED_PARAMETER(blockId2);
	int blockSize2 = pBinReader->Read<int>();
	UNREFERENCED_PARAMETER(blockSize2);

	//Retrieve the PageName (store Local)
	std::wstring pageName = pBinReader->ReadNullString();
	//	> If PageName is empty
	if (pageName.empty())
	{
		//	> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty])
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty]");
		return nullptr;
	}
	//>Retrieve texture filepath from the assetFile path
	//> (ex. c:/Example/somefont.fnt => c:/Example/) [Have a look at: wstring::rfind()]
	std::wstring filePath = assetFile.substr(0, assetFile.rfind('/') + 1);

	//>Use path and PageName to load the texture using the ContentManager [SpriteFont::m_pTexture]
	//> (ex. c:/Example/ + 'PageName' => c:/Example/somefont_0.png)
	pSpriteFont->m_pTexture = ContentManager::Load<TextureData>(filePath + pageName);


	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	int blockId3 = pBinReader->Read<char>();
	UNREFERENCED_PARAMETER(blockId3);
	int blockSize3 = pBinReader->Read<int>();
	UNREFERENCED_PARAMETER(blockSize3);

	//Retrieve Character Count (see documentation)
	const int characterCount{ blockSize3 / 20 };
	//Retrieve Every Character, For every Character:
	for (int i = 0; i < characterCount; i++)
	{
		//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
		wchar_t characterId = static_cast<wchar_t>(pBinReader->Read<int>());
		//> Check if CharacterId is valid (SpriteFont::IsCharValid), Log Warning and advance to next character if not valid
		if (!SpriteFont::IsCharValid(characterId))
		{
			Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid characterId");
			pBinReader->MoveBufferPosition(8);
			continue;
		}
		//> Retrieve the corresponding FontMetric (SpriteFont::GetMetric) [REFERENCE!!!]
		FontMetric& pMetric = pSpriteFont->GetMetric(characterId);
		//> Set IsValid to true [FontMetric::IsValid]
		pMetric.IsValid = true;
		//> Set Character (CharacterId) [FontMetric::Character]
		pMetric.Character = characterId;
		//> Retrieve Xposition (store Local)
		int xPosition = pBinReader->Read<short>();
		//> Retrieve Yposition (store Local)
		int yPosition = pBinReader->Read<short>();
		//> Retrieve & Set Width [FontMetric::Width]
		pMetric.Width = pBinReader->Read<short>();
		//> Retrieve & Set Height [FontMetric::Height]
		pMetric.Height = pBinReader->Read<short>();
		//> Retrieve & Set OffsetX [FontMetric::OffsetX]
		pMetric.OffsetX = pBinReader->Read<short>();
		//> Retrieve & Set OffsetY [FontMetric::OffsetY]
		pMetric.OffsetY = pBinReader->Read<short>();
		//> Retrieve & Set AdvanceX [FontMetric::AdvanceX]
		pMetric.AdvanceX = pBinReader->Read<short>();
		//> Retrieve & Set Page [FontMetric::Page]
		pMetric.Page = pBinReader->Read<char>();
		//> Retrieve Channel (BITFIELD!!!) 
		pMetric.Channel = pBinReader->Read<char>();
		//	> See documentation for BitField meaning [FontMetrix::Channel]
		if (pMetric.Channel & 0b0001)
			pMetric.Channel = 2;
		else if (pMetric.Channel & 0b0010)
			pMetric.Channel = 1;
		else if (pMetric.Channel & 0b0100)
			pMetric.Channel = 0;
		else if (pMetric.Channel & 0b1000)
			pMetric.Channel = 3;

		//> Calculate Texture Coordinates using Xposition, Yposition, TextureWidth & TextureHeight [FontMetric::TexCoord]
		pMetric.TexCoord = { (float)xPosition / (float)pSpriteFont->GetTextureWidth(), (float)yPosition / (float)pSpriteFont->GetTextureHeight() };
	}
	//DONE :)
	delete pBinReader;
	return pSpriteFont;
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
