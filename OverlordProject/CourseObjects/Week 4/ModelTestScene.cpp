#include "stdafx.h"

#include "ModelTestScene.h"
#include "GameObject.h"
#include "Components.h"
#include "ContentManager.h"
#include "PhysxManager.h"
#include "../../Materials/ColorMaterial.h"
#include "../../Materials/DiffuseMaterial.h"
#include "PhysxProxy.h"

ModelTestScene::ModelTestScene() :
GameScene(L"ModelTestScene"),
m_pChair(nullptr)
{}

void ModelTestScene::Initialize()
{
	const auto gameContext = GetGameContext();

	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	
	//GROUND PLANE
	//************
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto pBouncyMaterial = physX->createMaterial(0, 0, 1);
	auto pGround = new GameObject();
	pGround->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geom(new physx::PxPlaneGeometry());
	pGround->AddComponent(new ColliderComponent(geom, *pBouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(pGround);

	//CHAIR OBJECT
	//************
	m_pChair = new GameObject();

	//1. Attach a modelcomponent (chair.ovm)
	ModelComponent* pModelComponent = new ModelComponent(L"Resources/Meshes/Chair.ovm");
	m_pChair->AddComponent(pModelComponent);
	//2. Create a ColorMaterial and add it to the material manager
	Material* pColorMat = new ColorMaterial();
	pColorMat->Initialize(gameContext);
	gameContext.pMaterialManager->AddMaterial(pColorMat, 0);
	
	//3. Assign the material to the previous created modelcomponent
	pModelComponent->SetMaterial(0);

	//4. Create a DiffuseMaterial (using PosNormTex3D.fx)
	//		Make sure you are able to set a texture (DiffuseMaterial::SetDiffuseTexture(const wstring& assetFile))
	//		Load the correct shadervariable and set it during the material variable update
	DiffuseMaterial* pDiffuse = new DiffuseMaterial(L"Resources/Effects/PosNormTex3D.fx");
	pDiffuse->SetDiffuseTexture(L"Resources/Textures/Chair_Dark.dds");
	pDiffuse->SetEffectVariables(gameContext, pModelComponent);
	gameContext.pMaterialManager->AddMaterial(pDiffuse, 1);

	//5. Assign the material to the modelcomponent
	pModelComponent->SetMaterial(1);	

	//6. Attach a rigidbody component (pure-dynamic)
	RigidBodyComponent* pRBC = new RigidBodyComponent();
	m_pChair->AddComponent(pRBC);
	
	//7. Attach a collider component (Use a PxConvexMeshGeometry [chair.ovpc])
	physx::PxConvexMesh* pMesh = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Meshes/Chair.ovpc");
	std::shared_ptr<physx::PxGeometry> pGeometry(new physx::PxConvexMeshGeometry(pMesh));
	physx::PxMaterial* pMaterial = physX->createMaterial(0.5f, 0.5f, 0.1f);
	ColliderComponent* pColliderComponent = new ColliderComponent(pGeometry, *pMaterial);
	m_pChair->AddComponent(pColliderComponent);

	
	//Move the chair
	m_pChair->GetTransform()->Translate(0, 5, 0);
	
	AddChild(m_pChair);
}

void ModelTestScene::Update()
{}

void ModelTestScene::Draw()
{}
