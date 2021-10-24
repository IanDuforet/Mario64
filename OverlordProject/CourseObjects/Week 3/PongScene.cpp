//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "PongScene.h"
#include "FixedCamera.h"
#include "Prefabs.h"
#include "PhysxManager.h"
#include "Components.h"
#include "PhysxProxy.h"

enum InputIdsPong :int
{
	Up1,
	Down1,
	Up2,
	Down2
};

PongScene::PongScene(void) :
GameScene(L"PongScene"), m_pTopCamera{nullptr}, m_PaddleSpeed(15)
{}

void PongScene::Initialize()
{
	GetPhysxProxy()->EnablePhysxDebugRendering(false);
	
	//Get Physics
	physx::PxPhysics* pPhysics = PhysxManager::GetInstance()->GetPhysics();
	physx::PxMaterial* pBouncyMaterial = pPhysics->createMaterial(0.0f, 0.0f, 1.f);

	//Create FixedCamera
	auto fixedCam = new FixedCamera();
	fixedCam->Translate(0, 50, 0);
	fixedCam->SetRotation(90, 0, 0);
	AddChild(fixedCam);
	CameraComponent* comp = fixedCam->GetComponent<CameraComponent>();
	comp->UseOrthographicProjection();
	SetActiveCamera(comp);

	//Ground
	GameObject* pGameObject = new GameObject();
	pGameObject->AddComponent(new RigidBodyComponent(true));
	std::shared_ptr<physx::PxGeometry> pGeometry(new physx::PxPlaneGeometry());
	pGameObject->AddComponent(new ColliderComponent(pGeometry, *pBouncyMaterial,
		physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(pGameObject);
	
	//Create walls
	float wallWidth = 50;
	CubePrefab* topWall = new CubePrefab(wallWidth, 1, 1, {0,1,0,1});
	topWall->AddComponent(new RigidBodyComponent(true));
	topWall->GetTransform()->Translate(0,1, 12);
	std::shared_ptr<physx::PxGeometry> pBoxGeometry(new physx::PxBoxGeometry(wallWidth/2, 0.5f, 0.5f));
	topWall->AddComponent(new ColliderComponent(pBoxGeometry, *pBouncyMaterial));
	AddChild(topWall);

	CubePrefab* bottomWall = new CubePrefab(wallWidth, 1, 1, { 0,1,0,1 });
	bottomWall->AddComponent(new RigidBodyComponent(true));
	bottomWall->GetTransform()->Translate(0, 1, -12);
	bottomWall->AddComponent(new ColliderComponent(pBoxGeometry, *pBouncyMaterial));
	AddChild(bottomWall);

	//SPHERE
	m_pSphere = new SpherePrefab();
	m_pSphere->GetTransform()->Translate(0, 0, 0);

	RigidBodyComponent* pRigidBody = new RigidBodyComponent();
	m_pSphere->AddComponent(pRigidBody);

	std::shared_ptr<physx::PxGeometry> pSphereGeo(new physx::PxSphereGeometry(1.0f));
	m_pSphere->AddComponent(new ColliderComponent(pSphereGeo, *pBouncyMaterial));
	
	AddChild(m_pSphere);

	//Add begin force
	m_pSphere->GetComponent<RigidBodyComponent>()->AddForce({ 10,0,10 }, physx::PxForceMode::eIMPULSE);


	//Make paddles
	physx::PxMaterial* pPaddleMaterial = pPhysics->createMaterial(0.0f, 0.0f, 1.5f);
	
	float paddleHeight = 10;
	m_pPaddleLeft = new CubePrefab(1, 1, paddleHeight, { 1,1,1,1 });
	m_pPaddleLeft->AddComponent(new RigidBodyComponent());
	m_pPaddleLeft->GetComponent<RigidBodyComponent>()->SetKinematic(true);
	m_pPaddleLeft->GetTransform()->Translate(-17, 1, 0);
	std::shared_ptr<physx::PxGeometry> pPaddleGeo(new physx::PxBoxGeometry(0.5f, 0.5f, paddleHeight / 2));
	m_pPaddleLeft->AddComponent(new ColliderComponent(pPaddleGeo, *pPaddleMaterial));
	AddChild(m_pPaddleLeft);
	
	m_pPaddleRight = new CubePrefab(1, 1, paddleHeight, { 1,1,1,1 });
	m_pPaddleRight->AddComponent(new RigidBodyComponent());
	m_pPaddleRight->GetComponent<RigidBodyComponent>()->SetKinematic(true);
	m_pPaddleRight->GetTransform()->Translate(17, 1, 0);
	m_pPaddleRight->AddComponent(new ColliderComponent(pPaddleGeo, *pPaddleMaterial));
	AddChild(m_pPaddleRight);
	
	//Add Inputs
	const InputAction up1{ InputIdsPong::Up1, InputTriggerState::Down, 'R'};
	const InputAction down1{ InputIdsPong::Down1, InputTriggerState::Down, 'F' };
	const InputAction up2{ InputIdsPong::Up2, InputTriggerState::Down, 'I' };
	const InputAction down2{ InputIdsPong::Down2, InputTriggerState::Down, 'K' };


	//OUTPUT CONTROLS
	Logger::LogInfo(L"CONTROLS: Left player up down = R and F, Left player = I and K");
}

void PongScene::Update()
{
	float elapsedSec = GetGameContext().pGameTime->GetElapsed();

	//PADDLE LEFT
	if(GetGameContext().pInput->IsActionTriggered(InputIdsPong::Up1))
		m_pPaddleLeft->GetTransform()->Translate(m_pPaddleLeft->GetTransform()->GetPosition().x , m_pPaddleLeft->GetTransform()->GetPosition().y, m_pPaddleLeft->GetTransform()->GetPosition().z + (m_PaddleSpeed * elapsedSec));
	if (GetGameContext().pInput->IsActionTriggered(InputIdsPong::Down1))
		m_pPaddleLeft->GetTransform()->Translate(m_pPaddleLeft->GetTransform()->GetPosition().x, m_pPaddleLeft->GetTransform()->GetPosition().y, m_pPaddleLeft->GetTransform()->GetPosition().z - (m_PaddleSpeed * elapsedSec));

	//PADDLE RIGHT
	if (GetGameContext().pInput->IsActionTriggered(InputIdsPong::Up2))
		m_pPaddleRight->GetTransform()->Translate(m_pPaddleRight->GetTransform()->GetPosition().x, m_pPaddleRight->GetTransform()->GetPosition().y, m_pPaddleRight->GetTransform()->GetPosition().z + (m_PaddleSpeed * elapsedSec));
	if (GetGameContext().pInput->IsActionTriggered(InputIdsPong::Down2))
		m_pPaddleRight->GetTransform()->Translate(m_pPaddleRight->GetTransform()->GetPosition().x, m_pPaddleRight->GetTransform()->GetPosition().y, m_pPaddleRight->GetTransform()->GetPosition().z - (m_PaddleSpeed * elapsedSec));


	//RESET BALL
	if(m_pSphere->GetTransform()->GetPosition().x < -20 || m_pSphere->GetTransform()->GetPosition().x > 20)
	{
		m_pSphere->GetTransform()->Translate(0, 0, 0);
		m_pSphere->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->setLinearVelocity({ 0,0,0 });
		m_pSphere->GetComponent<RigidBodyComponent>()->AddForce({ 10,0,10 }, physx::PxForceMode::eIMPULSE);
	}


	//CLAMP PADDLES
	float clampValue = 6;
	if(m_pPaddleLeft->GetTransform()->GetPosition().z > clampValue)
		m_pPaddleLeft->GetTransform()->Translate(m_pPaddleLeft->GetTransform()->GetPosition().x, m_pPaddleLeft->GetTransform()->GetPosition().y, clampValue);
	if (m_pPaddleLeft->GetTransform()->GetPosition().z < -clampValue)
		m_pPaddleLeft->GetTransform()->Translate(m_pPaddleLeft->GetTransform()->GetPosition().x, m_pPaddleLeft->GetTransform()->GetPosition().y, -clampValue);
	if (m_pPaddleRight->GetTransform()->GetPosition().z > clampValue)
		m_pPaddleRight->GetTransform()->Translate(m_pPaddleRight->GetTransform()->GetPosition().x, m_pPaddleRight->GetTransform()->GetPosition().y, clampValue);
	if (m_pPaddleRight->GetTransform()->GetPosition().z < -clampValue)
		m_pPaddleRight->GetTransform()->Translate(m_pPaddleRight->GetTransform()->GetPosition().x, m_pPaddleRight->GetTransform()->GetPosition().y, -clampValue);
}

void PongScene::Draw()
{
}
