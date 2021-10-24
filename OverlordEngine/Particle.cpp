#include "stdafx.h"
#include "Particle.h"
#include "MathHelper.h"
// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
using namespace DirectX;

Particle::Particle(const ParticleEmitterSettings& emitterSettings):
	m_VertexInfo(ParticleVertex()),
	m_EmitterSettings(emitterSettings),
	m_IsActive(false),
	m_TotalEnergy(0),
	m_CurrentEnergy(0),
	m_SizeGrow(0),
	m_InitSize(0)
{}

void Particle::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
	if (!m_IsActive)
		return;

	//Update energy
	m_CurrentEnergy -= gameContext.pGameTime->GetElapsed();
	if (m_CurrentEnergy <= 0)
	{
		m_IsActive = false;
		return;
	}


	//Update variables
	//Position
	DirectX::XMVECTOR loadedVelocity = DirectX::XMLoadFloat3(&m_EmitterSettings.Velocity);
	DirectX::XMVECTOR loadedPosition = DirectX::XMLoadFloat3(&m_VertexInfo.Position);
	loadedPosition += loadedVelocity * gameContext.pGameTime->GetElapsed();
	DirectX::XMFLOAT3 loadedNewPosition;
	DirectX::XMStoreFloat3(&loadedNewPosition, loadedPosition);
	m_VertexInfo.Position = loadedNewPosition;

	//Color
	m_VertexInfo.Color = m_EmitterSettings.Color;
	float particleLifePercent = m_CurrentEnergy/m_TotalEnergy;
	m_VertexInfo.Color.w = particleLifePercent * 2;

	//Size
	//THE DOCUMENT SAID BIGGER OR SMALLER THAN 1, BUT IT'S 0 ACCORDING TO THE HEADER COMMENT
	m_VertexInfo.Size = m_InitSize + (m_InitSize * (1 - particleLifePercent) * m_SizeGrow);
	
}

void Particle::Init(XMFLOAT3 initPosition)
{
	UNREFERENCED_PARAMETER(initPosition);
	//TODO: See Lab9_2
	m_IsActive = true;
	
	//Init Energy
	m_TotalEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);
	m_CurrentEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);

	//Init position
	DirectX::XMVECTOR randomDirection = {0,0,1};
	DirectX::XMMATRIX randomRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI));
	DirectX::XMVECTOR rotatedRandom = DirectX::XMVector3TransformNormal(randomDirection, randomRotationMatrix);
	float randomDistanceStartingPoint = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);

	DirectX::XMVECTOR loadedInitPos = DirectX::XMLoadFloat3(&initPosition);
	DirectX::XMVECTOR newRandomPositionLoaded = rotatedRandom * randomDistanceStartingPoint + loadedInitPos;
	DirectX::XMFLOAT3 newRandomPosition;
	DirectX::XMStoreFloat3(&newRandomPosition, newRandomPositionLoaded);
	m_VertexInfo.Position = newRandomPosition;

	//Size
	m_InitSize = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	m_VertexInfo.Size = m_InitSize;
	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow);

	//Rotation
	m_VertexInfo.Rotation = randF(-XM_PI, XM_PI);


}

