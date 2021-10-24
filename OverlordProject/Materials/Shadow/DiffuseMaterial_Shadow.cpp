//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "DiffuseMaterial_Shadow.h"
#include "GeneralStructs.h"
#include "Logger.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "Components.h"

ID3DX11EffectShaderResourceVariable* DiffuseMaterial_Shadow::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectShaderResourceVariable* DiffuseMaterial_Shadow::m_pShadowSRVvariable = nullptr;
ID3DX11EffectVectorVariable* DiffuseMaterial_Shadow::m_pLightDirectionVariable = nullptr;
ID3DX11EffectMatrixVariable* DiffuseMaterial_Shadow::m_pLightWVPvariable = nullptr;

DiffuseMaterial_Shadow::DiffuseMaterial_Shadow() : Material(L"./Resources/Effects/Shadow/PosNormTex3D_Shadow.fx"),
	m_pDiffuseTexture(nullptr)
{}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	UNREFERENCED_PARAMETER(assetFile);
	//TODO: store the diffuse texture in the appropriate member
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void DiffuseMaterial_Shadow::SetLightDirection(DirectX::XMFLOAT3 dir)
{
	UNREFERENCED_PARAMETER(dir);
	//TODO: store the light direction in the appropriate member
	m_LightDirection = dir;
}

void DiffuseMaterial_Shadow::LoadEffectVariables()
{
	//TODO: load all the necessary shader variables
	if (!m_pDiffuseSRVvariable)
	{
		DiffuseMaterial_Shadow::m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseSRVvariable->IsValid())
		{
			Logger::LogWarning(L"DiffuseMaterial::LoadEffectVariables() > \'gDiffuseMap\' variable not found!");
			m_pDiffuseSRVvariable = nullptr;
		}
	}

	if (!m_pShadowSRVvariable)
	{
		DiffuseMaterial_Shadow::m_pShadowSRVvariable = GetEffect()->GetVariableByName("gShadowMap")->AsShaderResource();
		if (!m_pShadowSRVvariable->IsValid())
		{
			Logger::LogWarning(L"DiffuseMaterial::LoadEffectVariables() > \'gShadowMap\' variable not found!");
			m_pShadowSRVvariable = nullptr;
		}
	}

	if (!m_pLightDirectionVariable)
	{
		DiffuseMaterial_Shadow::m_pLightDirectionVariable = GetEffect()->GetVariableByName("gLightDirection")->AsVector();
		if (!m_pLightDirectionVariable->IsValid())
		{
			Logger::LogWarning(L"DiffuseMaterial::LoadEffectVariables() > \'gLightDirection\' variable not found!");
			m_pLightDirectionVariable = nullptr;
		}

	}

	if (!m_pLightWVPvariable)
	{
		DiffuseMaterial_Shadow::m_pLightWVPvariable = GetEffect()->GetVariableByName("gWorldViewProj_Light")->AsMatrix();
		if (!m_pLightWVPvariable->IsValid())
		{
			Logger::LogWarning(L"DiffuseMaterial::LoadEffectVariables() > \'gWorldViewProj_Light\' variable not found!");
			m_pLightWVPvariable = nullptr;
		}
	}

}

void DiffuseMaterial_Shadow::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);
	//TODO: update all the necessary shader variables

	if (m_pDiffuseTexture && m_pDiffuseSRVvariable)
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());

	if (m_pShadowSRVvariable && gameContext.pShadowMapper)
		m_pShadowSRVvariable->SetResource(gameContext.pShadowMapper->GetShadowMap());

	if (m_pLightDirectionVariable)
	{
		m_pLightDirectionVariable->SetFloatVector(&m_LightDirection.x);
	}

	if (m_pLightWVPvariable)
	{
		//Calculate the light world view projection matrix
		DirectX::XMFLOAT4X4 lightVPMatrix = gameContext.pShadowMapper->GetLightVP();
		DirectX::XMFLOAT4X4 worldMatrix = pModelComponent->GetTransform()->GetWorld();

		//Load it
		DirectX::XMMATRIX lightVPLoaded = DirectX::XMLoadFloat4x4(&lightVPMatrix);
		DirectX::XMMATRIX worldLoaded = DirectX::XMLoadFloat4x4(&worldMatrix);
		DirectX::XMMATRIX lightWVPLoaded = worldLoaded * lightVPLoaded;
		
		//Store it
		DirectX::XMFLOAT4X4 lightWVPMatrix;
		DirectX::XMStoreFloat4x4(&lightWVPMatrix, lightWVPLoaded);

		m_pLightWVPvariable->SetMatrix(&lightWVPMatrix._11);
	}

	


}