//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ShadowMapMaterial.h"
#include "ContentManager.h"

ShadowMapMaterial::~ShadowMapMaterial()
{
	//TODO: make sure you don't have memory leaks and/or resource leaks :) -> Figure out if you need to do something here
	for (int i{}; i < NUM_TYPES; i++)
	{
		m_InputLayoutDescriptions[i].clear();
		SafeRelease(m_pInputLayouts[i]);
	}
}

void ShadowMapMaterial::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	if (!m_IsInitialized)
	{
		//TODO: initialize the effect, techniques, shader variables, input layouts (hint use EffectHelper::BuildInputLayout), etc.
		m_pShadowEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ShadowMapGenerator.fx");
		m_pBoneTransforms = m_pShadowEffect->GetVariableByName("gBones")->AsMatrix();
		m_pLightVPMatrixVariable = m_pShadowEffect->GetVariableByName("gLightViewProj")->AsMatrix();
		m_pWorldMatrixVariable = m_pShadowEffect->GetVariableByName("gWorld")->AsMatrix();

		for (int i = 0; i < NUM_TYPES; i++)
		{
			m_pShadowTechs[i] = m_pShadowEffect->GetTechniqueByIndex(i);
			EffectHelper::BuildInputLayout(gameContext.pDevice, m_pShadowTechs[i], 
				&m_pInputLayouts[i], m_InputLayoutDescriptions[i], m_InputLayoutSizes[i], m_InputLayoutIds[i]);
		}


		m_IsInitialized = true;
	}
}

void ShadowMapMaterial::SetLightVP(DirectX::XMFLOAT4X4 lightVP) const
{
	UNREFERENCED_PARAMETER(lightVP);
	//TODO: set the correct shader variable
	if (m_pLightVPMatrixVariable)
		m_pLightVPMatrixVariable->SetMatrix(&lightVP._11);
}

void ShadowMapMaterial::SetWorld(DirectX::XMFLOAT4X4 world) const
{
	UNREFERENCED_PARAMETER(world);
	//TODO: set the correct shader variable
	if (m_pWorldMatrixVariable)
		m_pWorldMatrixVariable->SetMatrix(&world._11);
}

void ShadowMapMaterial::SetBones(const float* pData, int count) const
{
	UNREFERENCED_PARAMETER(pData);
	UNREFERENCED_PARAMETER(count);
	//TODO: set the correct shader variable
	if (m_pBoneTransforms)
		m_pBoneTransforms->SetMatrixArray(pData, 0, count);
}
