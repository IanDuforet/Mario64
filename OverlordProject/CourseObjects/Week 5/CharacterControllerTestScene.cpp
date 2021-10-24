#include "stdafx.h"
#include "CharacterControllerTestScene.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "Components.h"
#include "Prefabs.h"
#include "Character.h"
#include "Material.h"
#include "ContentManager.h"
#include "../../Materials/ColorMaterial.h"
CharacterControllerTestScene::CharacterControllerTestScene()
	: GameScene(L"CharacterControllerTestScene")
	, m_pCharacter(nullptr)
	, m_pLevel(nullptr)
{
	
}

void CharacterControllerTestScene::Initialize()
{
	const auto gameContext = GetGameContext();
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	GetGameContext().pGameTime->ForceElapsedUpperbound(true);

	//PhysxMaterials
	physx::PxMaterial* pDefaultPhysxMat = physX->createMaterial(0.5f, 0.5f, 0.1f);

	//Ground
	GameObject* pGroundPlane = new GameObject();
	pGroundPlane->AddComponent(new RigidBodyComponent(true));
	std::shared_ptr<physx::PxGeometry> pGeometry(new physx::PxPlaneGeometry());
	pGroundPlane->AddComponent(new ColliderComponent(pGeometry, *pDefaultPhysxMat,
		physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(pGroundPlane);

	//Character
	m_pCharacter = new Character();
	AddChild(m_pCharacter);
	m_pCharacter->GetTransform()->Translate(0, 2, 0);

	m_pLevel = new GameObject();
	ModelComponent* pLevelModel = new ModelComponent(L"./Resources/Meshes/SimpleLevel.ovm");
	Material* material{ new ColorMaterial() };
	gameContext.pMaterialManager->AddMaterial(material, 0);
	pLevelModel->SetMaterial(0);
	m_pLevel->AddComponent(pLevelModel);

	RigidBodyComponent* pRigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pMesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Meshes/SimpleLevel.ovpt") };
	std::shared_ptr<physx::PxGeometry> pLevelGeo{ new physx::PxTriangleMeshGeometry(pMesh) };

	ColliderComponent* pCollider{ new ColliderComponent(pLevelGeo, *pDefaultPhysxMat) };
	m_pLevel->AddComponent(pRigidBody);
	m_pLevel->AddComponent(pCollider);

	AddChild(m_pLevel);
}

void CharacterControllerTestScene::Update()
{
}

void CharacterControllerTestScene::Draw()
{
}
