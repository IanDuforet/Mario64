#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "ContentManager.h"
#include "ShadowMapMaterial.h"
#include "RenderTarget.h"
#include "MeshFilter.h"
#include "SceneManager.h"
#include "OverlordGame.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	//TODO: make sure you don't have memory leaks and/or resource leaks :) -> Figure out if you need to do something here
	SafeDelete(m_pShadowMat);
	SafeDelete(m_pShadowRT);
}

void ShadowMapRenderer::Initialize(const GameContext& gameContext)
{
	if (m_IsInitialized)
		return;

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: create shadow generator material + initialize it
	m_pShadowMat = new ShadowMapMaterial();
	m_pShadowMat->Initialize(gameContext);

	//TODO: create a rendertarget with the correct settings (hint: depth only) for the shadow generator using a RENDERTARGET_DESC
	m_pShadowRT = new RenderTarget(gameContext.pDevice);
	RENDERTARGET_DESC RTsettings;
	RTsettings.EnableColorBuffer = false;
	RTsettings.EnableDepthBuffer = true;
	RTsettings.EnableDepthSRV = true;
	RTsettings.Width = OverlordGame::GetGameSettings().Window.Width;
	RTsettings.Height = OverlordGame::GetGameSettings().Window.Height;

	//Create it
	m_pShadowRT->Create(RTsettings);

	m_IsInitialized = true;
}

void ShadowMapRenderer::SetLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction)
{
	UNREFERENCED_PARAMETER(position);
	UNREFERENCED_PARAMETER(direction);
	//TODO: store the input parameters in the appropriate datamembers
	m_LightPosition = position;
	m_LightDirection = direction;

	//TODO: calculate the Light VP matrix (Directional Light only ;)) and store it in the appropriate datamember

	//Calculate LightView Matrix
	DirectX::XMVECTOR loadedPosition = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR loadedDirection = DirectX::XMLoadFloat3(&direction);
	//Make world up
	DirectX::XMFLOAT3 worldUp = { 0.f, 1.f, 0.f };
	DirectX::XMVECTOR worldUpLoaded = DirectX::XMLoadFloat3(&worldUp);

	using namespace DirectX;
	DirectX::XMMATRIX ViewMatrix = DirectX::XMMatrixLookAtLH(loadedPosition, loadedPosition + loadedDirection, worldUpLoaded);

	//Calculate Projection Matrix
	DirectX::XMMATRIX LightProjectionMatrix = DirectX::XMMatrixOrthographicLH(m_Size * OverlordGame::GetGameSettings().Window.AspectRatio, m_Size, 0,400.f);

	DirectX::XMMATRIX LVPMatrix = ViewMatrix * LightProjectionMatrix;

	DirectX::XMStoreFloat4x4(&m_LightVP, LVPMatrix);
	

}


void ShadowMapRenderer::Begin(const GameContext& gameContext) const
{
	//Reset Texture Register 5 (Unbind)
	ID3D11ShaderResourceView *const pSRV[] = { nullptr };
	gameContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: set the appropriate render target that our shadow generator will write to (hint: use the OverlordGame::SetRenderTarget function through SceneManager)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(m_pShadowRT);
	//TODO: clear this render target
	//Thank you Ruben for telling me about the offset
	FLOAT offset = 0;
	m_pShadowRT->Clear(gameContext, &offset);
	//TODO: set the shader variables of this shadow generator material
	m_pShadowMat->SetLightVP(m_LightVP);
	m_pShadowMat->SetBones(nullptr, 0);
}

void ShadowMapRenderer::End(const GameContext& gameContext) const
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: restore default render target (hint: passing nullptr to OverlordGame::SetRenderTarget will do the trick)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(nullptr);
}

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, DirectX::XMFLOAT4X4 world, const std::vector<DirectX::XMFLOAT4X4>& bones) const
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pMeshFilter);
	UNREFERENCED_PARAMETER(world);
	UNREFERENCED_PARAMETER(bones);

	//TODO: update shader variables in material
	m_pShadowMat->SetBones(reinterpret_cast<const float*>(bones.data()), bones.size());
	m_pShadowMat->SetWorld(world);
	//TODO: set the correct inputlayout, buffers, topology (some variables are set based on the generation type Skinned or Static)

	int filterType = (int)pMeshFilter->m_HasAnimations;

	VertexBufferData buffer = pMeshFilter->GetVertexBufferData(gameContext, m_pShadowMat->m_InputLayoutIds[filterType]);
	UINT offset = 0;

	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &buffer.pVertexBuffer, &buffer.VertexStride, &offset);
	gameContext.pDeviceContext->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gameContext.pDeviceContext->IASetInputLayout(m_pShadowMat->m_pInputLayouts[filterType]);
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//TODO: invoke draw call
	D3DX11_TECHNIQUE_DESC desc;
	m_pShadowMat->m_pShadowTechs[filterType]->GetDesc(&desc);
	for (size_t i = 0; i < desc.Passes; i++)
	{
		m_pShadowMat->m_pShadowTechs[filterType]->GetPassByIndex(0)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(pMeshFilter->m_IndexCount, 0, 0);
	}

}

void ShadowMapRenderer::UpdateMeshFilter(const GameContext& gameContext, MeshFilter* pMeshFilter)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pMeshFilter);
	//TODO: based on the type (Skinned or Static) build the correct vertex buffers for the MeshFilter (Hint use MeshFilter::BuildVertexBuffer)
	int filterType = (int)pMeshFilter->m_HasAnimations;
	pMeshFilter->BuildVertexBuffer(gameContext, m_pShadowMat->m_InputLayoutIds[filterType], m_pShadowMat->m_InputLayoutSizes[filterType], 
		m_pShadowMat->m_InputLayoutDescriptions[filterType]);
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	//TODO: return the depth shader resource view of the shadow generator render target
	return m_pShadowRT->GetDepthShaderResourceView();
}
