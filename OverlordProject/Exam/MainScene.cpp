//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "MainScene.h"
#include "GameObject.h"

#include "SoundManager.h"
#include "ContentManager.h"
#include "Components.h"
#include "ModelAnimator.h"
#include "PhysxProxy.h"
#include "PhysxManager.h"
#include "../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "../Materials/DiffuseMaterial.h"
#include "../Materials/SkinnedDiffuseMaterial.h"
#include "../Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "../Exam/PostPixelate.h"
#include "../CourseObjects/Week 5/Character.h"
#include "Pickup.h"
#include "SpriteFont.h"
#include "TextRenderer.h"
#include "SceneManager.h"

MainScene::MainScene(void) :
	GameScene(L"MainScene")
	, m_pLevelScale{30}
	, m_pChannel{nullptr}
	, m_pPickups{ std::vector<Pickup*>() }
	, m_pSound{nullptr}
	, m_pCharacter{nullptr}
	, m_pFont{nullptr}
{}


void MainScene::SceneActivated()
{
	m_pChannel->setPaused(false);
}

void MainScene::SceneDeactivated()
{
	m_pChannel->setPaused(true);
}

void MainScene::Initialize()
{
	FMOD::System* pSystem = SoundManager::GetInstance()->GetSystem();

	//Create sounds
	pSystem->createStream("./Resources/Exam/Sounds/maintheme.mp3", FMOD_DEFAULT, 0, &m_pSound);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D.
	m_pSound->setMode(FMOD_LOOP_NORMAL);

	//Play maintheme
	pSystem->playSound(m_pSound, 0, false, &m_pChannel);
	m_pChannel->setVolume(0.05f);
	m_pChannel->setPaused(true);

	//GetPhysxProxy()->EnablePhysxDebugRendering(true);	
	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/Exam/Fonts/Mario64_32.fnt");
	const auto gameContext = GetGameContext();
	gameContext.pShadowMapper->SetLight({ -20,50,-40 }, { 0.4f, -0.6f, 0.3f });
	CreateMaterials();
	CreateLevel();
	CreateCoins();

	//Character
	m_pCharacter = new Character(2,5,20);
	m_pCharacter->GetTransform()->Translate(150, 5, 190);
	AddChild(m_pCharacter);
	AddPostProcessingEffect(new PostPixelate());

	//MAKE INPUT FOR PAUSE
	const InputAction pause{ InputIds::Pause, InputTriggerState::Pressed, VK_ESCAPE };
	InputManager* pInput{ gameContext.pInput };
	pInput->AddInputAction(pause);
}

void MainScene::Reset()
{
	m_pCharacter->GetTransform()->Translate(150, 5, 190);
	m_pCharacter->ResetCollectedCoins();
	for (Pickup* p : m_pPickups)
	{
		p->Reset();
	}
	FMOD::System* pSystem = SoundManager::GetInstance()->GetSystem();
	pSystem->playSound(m_pSound, 0, false, &m_pChannel);
	m_pChannel->setVolume(0.05f);

}

void MainScene::CreateCoins()
{
	Pickup* pPickup0 = new Pickup({ 97,5,-93 });
	AddChild(pPickup0);
	m_pPickups.push_back(pPickup0);

	Pickup* pPickup1 = new Pickup({ -20, 27, 214 });
	AddChild(pPickup1);
	m_pPickups.push_back(pPickup1);

	Pickup* pPickup2 = new Pickup({ 11, 85, -143});
	AddChild(pPickup2);
	m_pPickups.push_back(pPickup2);

	Pickup* pPickup3 = new Pickup({ -123, 97, -61 });
	AddChild(pPickup3);
	m_pPickups.push_back(pPickup3);

	Pickup* pPickup4 = new Pickup({ -61, 134, -120 });
	AddChild(pPickup4);
	m_pPickups.push_back(pPickup4);

	Pickup* pPickup5 = new Pickup({ 5, 28, 68 });
	AddChild(pPickup5);
	m_pPickups.push_back(pPickup5);

	Pickup* pPickup6 = new Pickup({ 177, 27, 83 });
	AddChild(pPickup6);
	m_pPickups.push_back(pPickup6);

	Pickup* pPickup7 = new Pickup({ 55, 26, -100 });
	AddChild(pPickup7);
	m_pPickups.push_back(pPickup7);

	Pickup* pPickup8 = new Pickup({ 228, 4, 190 });
	AddChild(pPickup8);
	m_pPickups.push_back(pPickup8);

	Pickup* pPickup9 = new Pickup({ 25, 52, -182 });
	AddChild(pPickup9);
	m_pPickups.push_back(pPickup9);

}

void MainScene::CreateMaterials()
{
	const auto gameContext = GetGameContext();

	//GRASS
	auto grassMat = new DiffuseMaterial_Shadow();
	grassMat->SetDiffuseTexture(L"./Resources/Exam/Textures/Grass.png");
	grassMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(grassMat, 0);

	//PATH
	auto pathMat = new DiffuseMaterial_Shadow();
	pathMat->SetDiffuseTexture(L"./Resources/Exam/Textures/Path.png");
	pathMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pathMat, 1);

	//WOOD
	auto woodMat = new DiffuseMaterial_Shadow();
	woodMat->SetDiffuseTexture(L"./Resources/Exam/Textures/Wood.png");
	woodMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(woodMat, 2);

	//MUD
	auto mudMat = new DiffuseMaterial_Shadow();
	mudMat->SetDiffuseTexture(L"./Resources/Exam/Textures/Mud.png");
	mudMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(mudMat, 3);

	//MARBLE
	auto pMarbleMat = new DiffuseMaterial_Shadow();
	pMarbleMat->SetDiffuseTexture(L"./Resources/Exam/Textures/Marble.png");
	pMarbleMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMarbleMat, 4);

	//BLACK MARBLE
	auto pBlackMarbleMat = new DiffuseMaterial_Shadow();
	pBlackMarbleMat->SetDiffuseTexture(L"./Resources/Exam/Textures/BlackMarble.png");
	pBlackMarbleMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pBlackMarbleMat, 5);

	//BLUE STONE
	auto pBlueStoneMat = new DiffuseMaterial_Shadow();
	pBlueStoneMat->SetDiffuseTexture(L"./Resources/Exam/Textures/BlueStone.png");
	pBlueStoneMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pBlueStoneMat, 6);

	//TUNNEL
	auto pTunnelMat = new DiffuseMaterial_Shadow();
	pTunnelMat->SetDiffuseTexture(L"./Resources/Exam/Textures/Tunnel.png");
	pTunnelMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pTunnelMat, 7);

	//BIG PATH
	auto pBigPathMat = new DiffuseMaterial_Shadow();
	pBigPathMat->SetDiffuseTexture(L"./Resources/Exam/Textures/BigPath.png");
	pBigPathMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pBigPathMat, 8);

	//COBBLESTONE
	auto pCobbleStoneMat = new DiffuseMaterial_Shadow();
	pCobbleStoneMat->SetDiffuseTexture(L"./Resources/Exam/Textures/CobbleStone.png");
	pCobbleStoneMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pCobbleStoneMat, 9);

	//SLATE
	auto pSlateMat = new DiffuseMaterial_Shadow();
	pSlateMat->SetDiffuseTexture(L"./Resources/Exam/Textures/Slate.png");
	pSlateMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pSlateMat, 10);

	//BARS
	auto pBarsMat = new DiffuseMaterial_Shadow();
	pBarsMat->SetDiffuseTexture(L"./Resources/Exam/Textures/Bars.png");
	pBarsMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pBarsMat, 11);

	//Mario
	auto pSkinnedDiffuseMaterial = new SkinnedDiffuseMaterial_Shadow();
	pSkinnedDiffuseMaterial->SetDiffuseTexture(L"./Resources/Exam/Textures/Mario/mario.png");
	pSkinnedDiffuseMaterial->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pSkinnedDiffuseMaterial, 12);

	//Coin
	auto pCoinMat = new DiffuseMaterial_Shadow();
	pCoinMat->SetDiffuseTexture(L"./Resources/Exam/Textures/Pickups/Coin.png");
	pCoinMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pCoinMat, 13);

	//Sky
	auto pSkyMat = new DiffuseMaterial_Shadow();
	pSkyMat->SetDiffuseTexture(L"./Resources/Exam/Textures/Skybox/Skybox.png");
	pSkyMat->SetLightDirection({0, 1.f, 0});
	gameContext.pMaterialManager->AddMaterial(pSkyMat, 14);

}

void MainScene::CreateLevel()
{
	
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	//PhysxMaterials
	physx::PxMaterial* pDefaultPhysxMat = physX->createMaterial(0.5f, 0.5f, 0.1f);

	//Ground
	GameObject* pGroundPlane = new GameObject();
	pGroundPlane->AddComponent(new RigidBodyComponent(true));
	std::shared_ptr<physx::PxGeometry> pGeometry(new physx::PxPlaneGeometry());
	pGroundPlane->AddComponent(new ColliderComponent(pGeometry, *pDefaultPhysxMat,
		physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(pGroundPlane);



	//GRASS
	auto pGrassObj = new GameObject();
	auto pGrassModel = new ModelComponent(L"./Resources/Exam/Meshes/GrassMesh.ovm");
	pGrassModel->SetMaterial(0);

	pGrassObj->AddComponent(pGrassModel);

	//Add collider
	RigidBodyComponent* pRigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pMesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/GrassMesh.ovpt") };
	std::shared_ptr<physx::PxGeometry> pGrassGeo{ new physx::PxTriangleMeshGeometry(pMesh, physx::PxMeshScale(m_pLevelScale))};

	ColliderComponent* pCollider{ new ColliderComponent(pGrassGeo, *pDefaultPhysxMat) };
	pGrassObj->AddComponent(pRigidBody);
	pGrassObj->AddComponent(pCollider);

	pGrassObj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);
	AddChild(pGrassObj);

	//Grass 2
	auto pGrass2Obj = new GameObject();
	auto pGrass2Model = new ModelComponent(L"./Resources/Exam/Meshes/GrassMesh2.ovm");
	pGrass2Model->SetMaterial(0);
	pGrass2Obj->AddComponent(pGrass2Model);

	//Add collider
	RigidBodyComponent* pGrass2RigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pGrass2Mesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/GrassMesh2.ovpt") };
	std::shared_ptr<physx::PxGeometry> pGrass2Geo{ new physx::PxTriangleMeshGeometry(pGrass2Mesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pGrass2Collider{ new ColliderComponent(pGrass2Geo, *pDefaultPhysxMat) };
	pGrass2Obj->AddComponent(pGrass2RigidBody);
	pGrass2Obj->AddComponent(pGrass2Collider);

	pGrass2Obj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pGrass2Obj);

	//PATH
	auto pPathObj = new GameObject();
	auto pPathModel = new ModelComponent(L"./Resources/Exam/Meshes/PathMesh.ovm");
	pPathModel->SetMaterial(1);

	//Add collider
	RigidBodyComponent* pPathRigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pPathMesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/PathMesh.ovpt") };
	std::shared_ptr<physx::PxGeometry> pPathGeo{ new physx::PxTriangleMeshGeometry(pPathMesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pPathCollider{ new ColliderComponent(pPathGeo, *pDefaultPhysxMat) };
	pPathObj->AddComponent(pPathRigidBody);
	pPathObj->AddComponent(pPathCollider);

	pPathObj->AddComponent(pPathModel);
	pPathObj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pPathObj);

	auto pPath2Obj = new GameObject();
	auto pPath2Model = new ModelComponent(L"./Resources/Exam/Meshes/PathMesh2.ovm");
	pPath2Model->SetMaterial(1);

	pPath2Obj->AddComponent(pPath2Model);

	//Add collider
	RigidBodyComponent* pPath2RigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pPath2Mesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/PathMesh2.ovpt") };
	std::shared_ptr<physx::PxGeometry> pPath2Geo{ new physx::PxTriangleMeshGeometry(pPath2Mesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pPath2Collider{ new ColliderComponent(pPath2Geo, *pDefaultPhysxMat) };
	pPath2Obj->AddComponent(pPath2RigidBody);
	pPath2Obj->AddComponent(pPath2Collider);

	pPath2Obj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pPath2Obj);


	//BRIDGES

	auto pBridgesObj = new GameObject();
	auto pBridgesModel = new ModelComponent(L"./Resources/Exam/Meshes/BridgesMesh.ovm");
	pBridgesModel->SetMaterial(2);

	pBridgesObj->AddComponent(pBridgesModel);

	//Add collider
	RigidBodyComponent* pBridgeRigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pBridgeMesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/BridgesMesh.ovpt") };
	std::shared_ptr<physx::PxGeometry> pBridgeGeo{ new physx::PxTriangleMeshGeometry(pBridgeMesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pBridgeCollider{ new ColliderComponent(pBridgeGeo, *pDefaultPhysxMat) };
	pBridgesObj->AddComponent(pBridgeRigidBody);
	pBridgesObj->AddComponent(pBridgeCollider);

	pBridgesObj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pBridgesObj);

	//MUD

	auto pMudObj = new GameObject();
	auto pMudModel = new ModelComponent(L"./Resources/Exam/Meshes/MudMesh.ovm");
	pMudModel->SetMaterial(3);

	pMudObj->AddComponent(pMudModel);

	//Add collider
	RigidBodyComponent* pMudRigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pMudMesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/MudMesh.ovpt") };
	std::shared_ptr<physx::PxGeometry> pMudGeo{ new physx::PxTriangleMeshGeometry(pMudMesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pMudCollider{ new ColliderComponent(pMudGeo, *pDefaultPhysxMat) };
	pMudObj->AddComponent(pMudRigidBody);
	pMudObj->AddComponent(pMudCollider);

	pMudObj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pMudObj);

	//MUD 2
	auto pMud2Obj = new GameObject();
	auto pMud2Model = new ModelComponent(L"./Resources/Exam/Meshes/MudMesh2.ovm");
	pMud2Model->SetMaterial(3);

	pMud2Obj->AddComponent(pMud2Model);

	//Add collider
	RigidBodyComponent* pMud2RigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pMud2Mesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/MudMesh2.ovpt") };
	std::shared_ptr<physx::PxGeometry> pMud2Geo{ new physx::PxTriangleMeshGeometry(pMud2Mesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pMud2Collider{ new ColliderComponent(pMud2Geo, *pDefaultPhysxMat) };
	pMud2Obj->AddComponent(pMud2RigidBody);
	pMud2Obj->AddComponent(pMud2Collider);

	pMud2Obj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pMud2Obj);

	//MUDHOLES
	auto pMudHolesObj = new GameObject();
	auto pMudHolesModel = new ModelComponent(L"./Resources/Exam/Meshes/MudHolesMesh.ovm");
	pMudHolesModel->SetMaterial(3);

	pMudHolesObj->AddComponent(pMudHolesModel);

	//Add collider
	RigidBodyComponent* pMudHolesRigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pMudHolesMesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/MudHolesMesh.ovpt") };
	std::shared_ptr<physx::PxGeometry> pMudHolesGeo{ new physx::PxTriangleMeshGeometry(pMudHolesMesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pMudHolesCollider{ new ColliderComponent(pMudHolesGeo, *pDefaultPhysxMat) };
	pMudHolesObj->AddComponent(pMudHolesRigidBody);
	pMudHolesObj->AddComponent(pMudHolesCollider);

	pMudHolesObj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pMudHolesObj);

	//MARBLE

	auto pMarbleObj = new GameObject();
	auto pMarbleModel = new ModelComponent(L"./Resources/Exam/Meshes/MarbleMesh.ovm");
	pMarbleModel->SetMaterial(4);

	pMarbleObj->AddComponent(pMarbleModel);

	//Add collider
	RigidBodyComponent* pMarbleRigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pMarbleMesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/MarbleMesh.ovpt") };
	std::shared_ptr<physx::PxGeometry> pMarbleGeo{ new physx::PxTriangleMeshGeometry(pMarbleMesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pMarbleCollider{ new ColliderComponent(pMarbleGeo, *pDefaultPhysxMat) };
	pMarbleObj->AddComponent(pMarbleRigidBody);
	pMarbleObj->AddComponent(pMarbleCollider);

	pMarbleObj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pMarbleObj);
	
	//BLACK MARBLE
	
	auto pBlackMarbleObj = new GameObject();
	auto pBlackMarbleModel = new ModelComponent(L"./Resources/Exam/Meshes/BlackMarble.ovm");
	pBlackMarbleModel->SetMaterial(5);

	pBlackMarbleObj->AddComponent(pBlackMarbleModel);

	//Add collider
	RigidBodyComponent* pBlackMarbleRigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pBlackMarbleMesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/BlackMarble.ovpt") };
	std::shared_ptr<physx::PxGeometry> pBlackMarbleGeo{ new physx::PxTriangleMeshGeometry(pBlackMarbleMesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pBlackMarbleCollider{ new ColliderComponent(pBlackMarbleGeo, *pDefaultPhysxMat) };
	pBlackMarbleObj->AddComponent(pBlackMarbleRigidBody);
	pBlackMarbleObj->AddComponent(pBlackMarbleCollider);

	pBlackMarbleObj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pBlackMarbleObj);

	//CANNON PLATFORM

	auto pCannonPlatformObj = new GameObject();
	auto pCannonPlatformModel = new ModelComponent(L"./Resources/Exam/Meshes/CannonPlatform.ovm");
	pCannonPlatformModel->SetMaterial(6);

	pCannonPlatformObj->AddComponent(pCannonPlatformModel);

	//Add collider
	RigidBodyComponent* pCannonPlatformRigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pCannonPlatformMesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/CannonPlatform.ovpt") };
	std::shared_ptr<physx::PxGeometry> pCannonPlatformGeo{ new physx::PxTriangleMeshGeometry(pCannonPlatformMesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pCannonPlatformCollider{ new ColliderComponent(pCannonPlatformGeo, *pDefaultPhysxMat) };
	pCannonPlatformObj->AddComponent(pCannonPlatformRigidBody);
	pCannonPlatformObj->AddComponent(pCannonPlatformCollider);

	pCannonPlatformObj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pCannonPlatformObj);


	//TUNNEL
	
	auto pTunnelObj = new GameObject();
	auto pTunnelModel = new ModelComponent(L"./Resources/Exam/Meshes/TunnelMesh.ovm");
	pTunnelModel->SetMaterial(7);

	pTunnelObj->AddComponent(pTunnelModel);

	//Add collider
	RigidBodyComponent* pTunnelRigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pTunnelMesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/TunnelMesh.ovpt") };
	std::shared_ptr<physx::PxGeometry> pTunnelGeo{ new physx::PxTriangleMeshGeometry(pTunnelMesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pTunnelCollider{ new ColliderComponent(pTunnelGeo, *pDefaultPhysxMat) };
	pTunnelObj->AddComponent(pTunnelRigidBody);
	pTunnelObj->AddComponent(pTunnelCollider);

	pTunnelObj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pTunnelObj);


	//BIG PATH
	
	auto pBigPathObj = new GameObject();
	auto pBigPathModel = new ModelComponent(L"./Resources/Exam/Meshes/BigPathMesh.ovm");
	pBigPathModel->SetMaterial(8);

	pBigPathObj->AddComponent(pBigPathModel);

	//Add collider
	RigidBodyComponent* pBigPathRigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pBigPathMesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/BigPathMesh.ovpt") };
	std::shared_ptr<physx::PxGeometry> pBigPathGeo{ new physx::PxTriangleMeshGeometry(pBigPathMesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pBigPathCollider{ new ColliderComponent(pBigPathGeo, *pDefaultPhysxMat) };
	pBigPathObj->AddComponent(pBigPathRigidBody);
	pBigPathObj->AddComponent(pBigPathCollider);

	pBigPathObj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pBigPathObj);

	//CANNON 2
	
	auto pCannon2Obj = new GameObject();
	auto pCannon2Model = new ModelComponent(L"./Resources/Exam/Meshes/Cannon2Mesh.ovm");
	pCannon2Model->SetMaterial(9);

	pCannon2Obj->AddComponent(pCannon2Model);

	//Add collider
	RigidBodyComponent* pCannon2RigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pCannon2Mesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/Cannon2Mesh.ovpt") };
	std::shared_ptr<physx::PxGeometry> pCannon2Geo{ new physx::PxTriangleMeshGeometry(pCannon2Mesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pCannon2Collider{ new ColliderComponent(pCannon2Geo, *pDefaultPhysxMat) };
	pCannon2Obj->AddComponent(pCannon2RigidBody);
	pCannon2Obj->AddComponent(pCannon2Collider);

	pCannon2Obj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pCannon2Obj);


	//SLATES
	
	auto pSlateObj = new GameObject();
	auto pSlateModel = new ModelComponent(L"./Resources/Exam/Meshes/SlateMesh.ovm");
	pSlateModel->SetMaterial(10);

	pSlateObj->AddComponent(pSlateModel);

	//Add collider
	RigidBodyComponent*	pSlatesRigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pSlatesMesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/SlateMesh.ovpt") };
	std::shared_ptr<physx::PxGeometry> pSlatesGeo{ new physx::PxTriangleMeshGeometry(pSlatesMesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pSlatesCollider{ new ColliderComponent(pSlatesGeo, *pDefaultPhysxMat) };
	pSlateObj->AddComponent(pSlatesRigidBody);
	pSlateObj->AddComponent(pSlatesCollider);

	pSlateObj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pSlateObj);

	//BARS
	
	auto pBarsObj = new GameObject();
	auto pBarsModel = new ModelComponent(L"./Resources/Exam/Meshes/BarsMesh.ovm");
	pBarsModel->SetMaterial(11);

	pBarsObj->AddComponent(pBarsModel);

	//Add collider
	RigidBodyComponent* pBarsRigidBody{ new RigidBodyComponent(true) };
	physx::PxTriangleMesh* pBarsMesh{ ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Exam/Meshes/BarsMesh.ovpt") };
	std::shared_ptr<physx::PxGeometry> pBarsGeo{ new physx::PxTriangleMeshGeometry(pBarsMesh, physx::PxMeshScale(m_pLevelScale)) };

	ColliderComponent* pBarsCollider{ new ColliderComponent(pBarsGeo, *pDefaultPhysxMat) };
	pBarsObj->AddComponent(pBarsRigidBody);
	pBarsObj->AddComponent(pBarsCollider);

	pBarsObj->GetTransform()->Scale(m_pLevelScale, m_pLevelScale, m_pLevelScale);

	AddChild(pBarsObj);

	//SKYBOX
	auto pSkyBoxObj = new GameObject();
	auto pSkyBoxModel = new ModelComponent(L"./Resources/Exam/Meshes/Skybox/Skybox.ovm");
	pSkyBoxModel->SetMaterial(14);
	pSkyBoxObj->AddComponent(pSkyBoxModel);
	pSkyBoxObj->GetTransform()->Translate(0, -20, 0);
	AddChild(pSkyBoxObj);

}

void MainScene::Update()
{
	if (m_pCharacter->GetCoins() >= 10)
	{
		SceneManager::GetInstance()->SetActiveGameScene(L"EndScene");
	}

	const auto gameContext = GetGameContext();
	InputManager* pInput{ gameContext.pInput };
	if (pInput->IsActionTriggered(InputIds::Pause))
	{
		SceneManager::GetInstance()->SetActiveGameScene(L"PauseScene");
	}

}

void MainScene::Draw()
{
	const auto gameContext = GetGameContext();

	if (m_pFont->GetFontName() != L"")
	{
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Coins collected:", DirectX::XMFLOAT2(10, 15), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Black));
		std::wstringstream sFps;
		sFps << L"FPS: " << gameContext.pGameTime->GetFPS();

		TextRenderer::GetInstance()->DrawText(m_pFont, std::to_wstring(m_pCharacter->GetCoins()), DirectX::XMFLOAT2(10, 50), DirectX::XMFLOAT4(DirectX::Colors::Yellow));
	}

}
