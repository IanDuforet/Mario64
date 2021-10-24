#include "stdafx.h"
#include "PostPixelate.h"
#include "RenderTarget.h"

PostPixelate::PostPixelate()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Pixelate.fx", 2),
	m_pTextureMapVariabele(nullptr)
{
}

void PostPixelate::LoadEffectVariables()
{
	//TODO: Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	//Check if valid!
	if (!m_pTextureMapVariabele)
	{
		PostPixelate::m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
		if (!m_pTextureMapVariabele->IsValid())
		{
			Logger::LogWarning(L"PostBlur::LoadEffectVariables() > \'gTexture\' variable not found!");
			m_pTextureMapVariabele = nullptr;
		}
	}
}

void PostPixelate::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	UNREFERENCED_PARAMETER(pRendertarget);
	//TODO: Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	if (m_pTextureMapVariabele && pRendertarget)
	{
		m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
	}
}