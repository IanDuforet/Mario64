//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ComponentsTestScene.h"

#include "PhysxProxy.h"
#include "PhysxManager.h"
#include "Prefabs.h"
#include "Components.h"

ComponentsTestScene::ComponentsTestScene(void) :
GameScene(L"ComponentsTestScene")
{}

void ComponentsTestScene::Initialize()
{
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	
	physx::PxPhysics* pPhysics = PhysxManager::GetInstance()->GetPhysics();
	physx::PxMaterial* pBouncyMaterial = pPhysics->createMaterial(0.5f, 0.5f, 1.f);

	//Ground
	GameObject* pGameObject = new GameObject();
	pGameObject->AddComponent(new RigidBodyComponent(true));
	std::shared_ptr<physx::PxGeometry> pGeometry(new physx::PxPlaneGeometry());
	pGameObject->AddComponent(new ColliderComponent(pGeometry, *pBouncyMaterial, 
		physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0,0,1)))));
	AddChild(pGameObject);

	//SPHERE
	GameObject* pSphere = new SpherePrefab();
	pSphere->GetTransform()->Translate(0, 30.f, 0);

	RigidBodyComponent* pRigidBody = new RigidBodyComponent();
	pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group0);
	pRigidBody->SetCollisionIgnoreGroups(static_cast<CollisionGroupFlag>((UINT)CollisionGroupFlag::Group1 | (UINT)CollisionGroupFlag::Group2));
	pSphere->AddComponent(pRigidBody);

	std::shared_ptr<physx::PxGeometry> pSphereGeo(new physx::PxSphereGeometry(1.0f));
	pSphere->AddComponent(new ColliderComponent(pSphereGeo, *pBouncyMaterial));
	
	AddChild(pSphere);

	//SPHERE 2
	pSphere = new SpherePrefab();
	pSphere->GetTransform()->Translate(0, 20.f, 0);

	pRigidBody = new RigidBodyComponent();
	pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group1);
	pSphere->AddComponent(pRigidBody);
	pSphere->AddComponent(new ColliderComponent(pSphereGeo, *pBouncyMaterial));

	
	AddChild(pSphere);

	//SPHERE 3
	pSphere = new SpherePrefab();
	pSphere->GetTransform()->Translate(0, 10.f, 0);

	pRigidBody = new RigidBodyComponent();
	pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group2);
	pSphere->AddComponent(pRigidBody);
	pSphere->AddComponent(new ColliderComponent(pSphereGeo, *pBouncyMaterial));


	AddChild(pSphere);
	
}

void ComponentsTestScene::Update()
{
	const auto gameContext = GetGameContext();
	m_FpsInterval += gameContext.pGameTime->GetElapsed();
	if(m_FpsInterval >= 1.f)
	{
		--m_FpsInterval;
		Logger::LogFormat(LogLevel::Info, L"FPS: %i", gameContext.pGameTime->GetFPS());
	}
}

void ComponentsTestScene::Draw()
{
}
