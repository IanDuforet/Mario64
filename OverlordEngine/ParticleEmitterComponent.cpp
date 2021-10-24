#include "stdafx.h"
#include "ParticleEmitterComponent.h"
 #include <utility>
#include "EffectHelper.h"
#include "ContentManager.h"
#include "TextureDataLoader.h"
#include "Particle.h"
#include "TransformComponent.h"

ParticleEmitterComponent::ParticleEmitterComponent(std::wstring  assetFile, int particleCount):
	m_pVertexBuffer(nullptr),
	m_pEffect(nullptr),
	m_pParticleTexture(nullptr),
	m_pInputLayout(nullptr),
	m_pInputLayoutSize(0),
	m_Settings(ParticleEmitterSettings()),
	m_ParticleCount(particleCount),
	m_ActiveParticles(0),
	m_LastParticleInit(0.0f),
	m_AssetFile(std::move(assetFile))
{
	//TODO: See Lab9_2
	for (int i = 0; i < m_ParticleCount; i++)
	{
		m_Particles.push_back(new Particle(m_Settings));
	}
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	//TODO: See Lab9_2
	for (Particle* pP : m_Particles)
	{
		SafeDelete(pP);
	}
	m_Particles.clear();

	m_pInputLayout->Release();
	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
	LoadEffect(gameContext);
	CreateVertexBuffer(gameContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::LoadEffect(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ParticleRenderer.fx");
	m_pDefaultTechnique = m_pEffect->GetTechniqueByIndex(0);
	m_pWvpVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
	m_pTextureVariable = m_pEffect->GetVariableByName("gParticleTexture")->AsShaderResource();

	EffectHelper::BuildInputLayout(gameContext.pDevice, m_pDefaultTechnique, &m_pInputLayout, m_pInputLayoutSize);

}

void ParticleEmitterComponent::CreateVertexBuffer(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	//*************
	//VERTEX BUFFER
	D3D11_BUFFER_DESC vertexBuffDesc;
	vertexBuffDesc.ByteWidth = m_ParticleCount * sizeof(ParticleVertex);
	vertexBuffDesc.BindFlags = D3D10_BIND_FLAG::D3D10_BIND_VERTEX_BUFFER;
	vertexBuffDesc.CPUAccessFlags = D3D10_CPU_ACCESS_FLAG::D3D10_CPU_ACCESS_WRITE;
	vertexBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vertexBuffDesc.MiscFlags = 0;
	HRESULT result = gameContext.pDevice->CreateBuffer(&vertexBuffDesc, nullptr, &m_pVertexBuffer);
	Logger::LogHResult(result, L"ParticleEmitterComponent_CreateVertexBuffer", false);
}

void ParticleEmitterComponent::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
	float averageEnergy = (m_Settings.MaxEnergy + m_Settings.MinEnergy) / 2;
	float particleInterval = averageEnergy/m_ParticleCount;
	
	m_LastParticleInit += gameContext.pGameTime->GetElapsed();

	m_ActiveParticles = 0;

	//BUFFER MAPPING CODE [PARTIAL :)]
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ParticleVertex* pBuffer = (ParticleVertex*) mappedResource.pData;

	for (Particle* pP : m_Particles)
	{
		pP->Update(gameContext);
		if (pP->IsActive())
		{
			pBuffer[m_ActiveParticles] = pP->GetVertexInfo();
			m_ActiveParticles++;
		}
		else if (m_LastParticleInit >= particleInterval)
		{
			pP->Init(GetTransform()->GetWorldPosition());
			pBuffer[m_ActiveParticles] = pP->GetVertexInfo();
			m_ActiveParticles++;
			m_LastParticleInit = 0;
		}
	}

	gameContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);

}

void ParticleEmitterComponent::Draw(const GameContext& )
{}

void ParticleEmitterComponent::PostDraw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
	if (gameContext.pCamera)
	{	
		m_pWvpVariable->SetMatrix(&gameContext.pCamera->GetViewProjection()._11);
		m_pViewInverseVariable->SetMatrix(&gameContext.pCamera->GetViewInverse()._11);
	}

	m_pTextureVariable->SetResource(m_pParticleTexture->GetShaderResourceView());

	UINT strideSize = sizeof(ParticleVertex);
	UINT offset = 0;

	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &strideSize, &offset);

	D3DX11_TECHNIQUE_DESC desc;
	m_pDefaultTechnique->GetDesc(&desc);
	for (uint32_t i = 0; i < desc.Passes; i++)
	{
		m_pDefaultTechnique->GetPassByIndex(i)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}
