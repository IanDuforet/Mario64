#include "stdafx.h"
#include "Pickup.h"
#include "Components.h"
#include "ContentManager.h"
#include "PhysxManager.h"
#include "GameScene.h"
#include "ParticleEmitterComponent.h"
#include "../CourseObjects/Week 5/Character.h"
#include "SoundManager.h"
Pickup::Pickup(DirectX::XMFLOAT3 position)
	: m_Position{ position }
	, m_RotationSpeed{100}
	, m_TotalRotation{0}
	, m_CoinSize{0.3f}
	, m_pChannel{}
	, m_pCoinSound{}
{
	
}

void Pickup::Initialize(const GameContext& gameContext)
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	UNREFERENCED_PARAMETER(gameContext);
	GameObject* pPivotedModel = new GameObject();
	ModelComponent* pCoinModel = new ModelComponent(L"./Resources/Exam/Meshes/Pickups/Coin.ovm");
	pCoinModel->SetMaterial(13);
	pPivotedModel->AddComponent(pCoinModel);
	pPivotedModel->GetTransform()->Translate(0, -6.f, 0);
	AddChild(pPivotedModel);

	//PhysxMaterials
	physx::PxMaterial* pDefaultPhysxMat = physX->createMaterial(0.5f, 0.5f, 0.1f);

	//Add collider
	RigidBodyComponent* pRigidBody{ new RigidBodyComponent(false) };
	pRigidBody->SetKinematic(true);
	pRigidBody->SetConstraint(RigidBodyConstraintFlag::TransY, true);
	std::shared_ptr<physx::PxGeometry> pSphereGeo(new physx::PxSphereGeometry(2.0f));

	ColliderComponent* pCollider{ new ColliderComponent(pSphereGeo, *pDefaultPhysxMat) };
	pCollider->EnableTrigger(true);
	AddComponent(pRigidBody);
	AddComponent(pCollider);

	auto pParticleEmitter = new GameObject();
	m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Exam/Textures/Pickups/Sparkle.png", 10);
	m_pParticleEmitter->SetVelocity(DirectX::XMFLOAT3(0,0,0));
	m_pParticleEmitter->SetMinSize(0.2f);
	m_pParticleEmitter->SetMaxSize(1.0f);
	m_pParticleEmitter->SetMinEnergy(0.5f);
	m_pParticleEmitter->SetMaxEnergy(1.f);
	m_pParticleEmitter->SetMinSizeGrow(-3.5f);
	m_pParticleEmitter->SetMaxSizeGrow(5.5f);
	m_pParticleEmitter->SetMinEmitterRange(0.5f);
	m_pParticleEmitter->SetMaxEmitterRange(2.f);
	m_pParticleEmitter->SetColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.6f));
	pParticleEmitter->AddComponent(m_pParticleEmitter);
	AddChild(pParticleEmitter);

	//Create sound
	FMOD::System* pSystem = SoundManager::GetInstance()->GetSystem();
	pSystem->createStream("./Resources/Exam/Sounds/coin.mp3", FMOD_DEFAULT, 0, &m_pCoinSound);

	auto pickUpEvent = [this](GameObject* trigger, GameObject* other, GameObject::TriggerAction action)
	{
		UNREFERENCED_PARAMETER(trigger);
		UNREFERENCED_PARAMETER(other);


		if (action == TriggerAction::ENTER)
		{
			static_cast<Character*>(other)->CollectCoin();
			//Teleport pickcup model under map
			trigger->GetTransform()->Translate(0, -10, 0);
			FMOD::System* pSystem = SoundManager::GetInstance()->GetSystem();
			pSystem->playSound(m_pCoinSound, 0, 0, &m_pChannel);
			m_pChannel->setVolume(0.05f);
		}
	
	};
	SetOnTriggerCallBack(pickUpEvent);

	GetTransform()->Translate(m_Position);
	GetTransform()->Scale(m_CoinSize, m_CoinSize, m_CoinSize);

}

void Pickup::Reset()
{
	GetTransform()->Translate(m_Position);
}

void Pickup::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	
}

void Pickup::Update(const GameContext& gameContext)
{
	//Rotation
	UNREFERENCED_PARAMETER(gameContext);
	m_TotalRotation += m_RotationSpeed * gameContext.pGameTime->GetElapsed();
	if (m_TotalRotation > 360)
		m_TotalRotation = 0;
	GetTransform()->Rotate(0, m_TotalRotation, 0, true);
}