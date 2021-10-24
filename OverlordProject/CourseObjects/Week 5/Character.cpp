
#include "stdafx.h"
#include "Character.h"
#include "Components.h"
#include "Prefabs.h"
#include "GameScene.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "ModelAnimator.h"
#include "ModelComponent.h"
#include "SoundManager.h"

Character::Character(float radius, float height, float moveSpeed) : 
	m_Radius(radius),
	m_Height(height),
	m_MoveSpeed(moveSpeed),
	m_pCamera(nullptr),
	m_pController(nullptr),
	m_TotalPitch(0), 
	m_TotalYaw(0),
	m_RotationSpeed(90.f),
	//Running
	m_MaxRunVelocity(50.0f), 
	m_TerminalVelocity(100),
	m_Gravity(98.1f), 
	m_RunAccelerationTime(1.f), 
	m_JumpAccelerationTime(0.8f), 
	m_RunAcceleration(m_MaxRunVelocity/m_RunAccelerationTime), 
	m_JumpAcceleration(m_Gravity/m_JumpAccelerationTime), 
	m_RunVelocity(0), 
	m_JumpVelocity(0),
	m_Velocity(0,0,0),
	m_JumpHeight(50),
	m_SpringArmDistance{-40},
	m_CollectedCoins{ 0 },
	m_IsJumping{false},
	m_pCameraPivot{nullptr},
	m_TotalModelRoll{ 0 },
	m_pPivotedModel{nullptr},
	m_Animator{nullptr},
	m_pJumpSound{ nullptr },
	m_pChannel{nullptr}
{}

void Character::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: Create controller
	m_pController = new ControllerComponent(PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.0f, 0.0f, 0.0f));
	AddComponent(m_pController);
	//TODO: Add a fixed camera as child
	FixedCamera* pCamera{ new FixedCamera() };
	m_pCamera = pCamera->GetComponent<CameraComponent>();
	AddChild(pCamera);
	//TODO: Register all Input Actions
	const InputAction reset{ InputIds::Reset, InputTriggerState::Down, 'R' };
	const InputAction up{ FORWARD, InputTriggerState::Down, 'W' };
	const InputAction left{ LEFT, InputTriggerState::Down, 'A' };
	const InputAction down{ BACKWARD, InputTriggerState::Down, 'S' };
	const InputAction right{ RIGHT, InputTriggerState::Down, 'D' };
	const InputAction jump{ JUMP, InputTriggerState::Pressed, VK_SPACE };
	InputManager* pInput{ gameContext.pInput };
	pInput->AddInputAction(reset);
	pInput->AddInputAction(up);
	pInput->AddInputAction(down);
	pInput->AddInputAction(left);
	pInput->AddInputAction(right);
	pInput->AddInputAction(jump);


	GameObject* pPivotedModel = new GameObject();
	ModelComponent* pCharacterModel = new ModelComponent(L"./Resources/Exam/Meshes/Mario/Mario.ovm");
	pCharacterModel->SetMaterial(12);
	pPivotedModel->AddComponent(pCharacterModel);
	pPivotedModel->GetTransform()->Translate(0, -5, 0);
	m_pPivotedModel = pPivotedModel;
	AddChild(m_pPivotedModel);

	//Create sound
	FMOD::System* pSystem = SoundManager::GetInstance()->GetSystem();
	pSystem->createStream("./Resources/Exam/Sounds/jump.mp3", FMOD_DEFAULT, 0, &m_pJumpSound);

}

void Character::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: Set the camera as active
	// We need to do this in the PostInitialize because child game objects only get initialized after the Initialize of the current object finishes
	m_pCamera = GetChild<FixedCamera>()->GetComponent<CameraComponent>();
	m_pCamera->GetTransform()->Translate(0, 0, m_SpringArmDistance);
	GetScene()->SetActiveCamera(m_pCamera);
	m_pCameraPivot = new GameObject();
	RemoveChild(m_pCamera->GetGameObject());
	m_pCameraPivot->AddChild(m_pCamera->GetGameObject());
	AddChild(m_pCameraPivot);
	
	m_Animator = m_pPivotedModel->GetComponent<ModelComponent>()->GetAnimator();
	m_Animator->SetAnimation(1, true);
	m_Animator->Play();

	//Move model down to fix the pivot (the idle animation pivot is wrong)
	m_pPivotedModel->GetTransform()->Translate(0, -1.2f, 0);

	ShowCursor(false);
	gameContext.pInput->ForceMouseToCenter(true);

}


void Character::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: Update the character (Camera rotation, Character Movement, Character Gravity)
	InputManager* pInput{ gameContext.pInput };
	DirectX::XMFLOAT2 moveInput{ 0,0 };

	//camera rotation calculation
	DirectX::XMFLOAT2 look = DirectX::XMFLOAT2(0, 0);
	const POINT mouseMove = InputManager::GetMouseMovement();
	look.x = static_cast<float>(mouseMove.x);
	look.y = static_cast<float>(mouseMove.y);

	//Forward backwards
	if (pInput->IsActionTriggered(FORWARD))
		moveInput.y = 1.0f;
	else if (pInput->IsActionTriggered(BACKWARD))
		moveInput.y = -1.0f;
	else
		moveInput.y = 0.0f;

	//Left right
	if (pInput->IsActionTriggered(RIGHT))
		moveInput.x = 1.0f;
	else if (pInput->IsActionTriggered(LEFT))
		moveInput.x = -1.0f;
	else
		moveInput.x = 0.0f;

	float deltaTime = gameContext.pGameTime->GetElapsed();

	DirectX::XMVECTOR forwardVec =  DirectX::XMLoadFloat3(&m_pCamera->GetTransform()->GetForward());
	DirectX::XMVECTOR rightVec = DirectX::XMLoadFloat3(&m_pCamera->GetTransform()->GetRight());
	DirectX::XMVECTOR directionVec{};

	using namespace DirectX; //This line for using the overloaded operators
	directionVec += forwardVec * moveInput.y;
	directionVec += rightVec * moveInput.x;
	DirectX::XMVector3Normalize(directionVec);

	DirectX::XMFLOAT3 direction;
	DirectX::XMStoreFloat3(&direction, directionVec);

	m_RunVelocity += m_RunAcceleration * deltaTime;

	//Cap the speed
	if (m_RunVelocity >= m_MaxRunVelocity)
		m_RunVelocity = m_MaxRunVelocity;

	//Apply gravity if not on floor
	if (m_pController->GetCollisionFlags() != physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		m_JumpVelocity -= m_JumpAcceleration * deltaTime;
		
		//Cap the fall speed
		if (m_JumpVelocity <= -m_TerminalVelocity)
			m_JumpVelocity = -m_TerminalVelocity;
	}
	else if(pInput->IsActionTriggered(JUMP))
	{
			m_IsJumping = true;
			m_JumpVelocity = m_JumpHeight;
			m_Velocity.y = 0;
			FMOD::System* pSystem = SoundManager::GetInstance()->GetSystem();
			pSystem->playSound(m_pJumpSound, 0, 0, &m_pChannel);
			m_pChannel->setVolume(0.7f);
	}
	else
	{
		m_IsJumping = false;
		m_Velocity.y = 0;
	}
	
	m_Velocity = DirectX::XMFLOAT3(direction.x * m_RunVelocity * deltaTime, (m_Velocity.y + m_JumpVelocity) * deltaTime , direction.z * m_RunVelocity * deltaTime);

	//Move character
	DirectX::XMFLOAT3 toMove;
	toMove = m_Velocity;
	GetComponent<ControllerComponent>()->Move(toMove);

	//rotating camera
	m_TotalPitch += look.y * m_RotationSpeed * gameContext.pGameTime->GetElapsed();
	m_TotalYaw += look.x * m_RotationSpeed * gameContext.pGameTime->GetElapsed();
	//Clamp y rotation
	Clamp(m_TotalPitch, 60.f, -20.f);

	float heading = atan2f(-direction.x, -direction.z);
	m_pCameraPivot->GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);

	//HANDLE ANIMATIONS
	if (m_IsJumping)
	{
		m_pPivotedModel->GetTransform()->Rotate(0, heading, 0, false);
		m_pPivotedModel->GetTransform()->Translate(0, -5, 0);
		m_Animator->SetAnimation(2, false);
	}
	else if (m_Velocity.x != 0 || m_Velocity.z != 0)
	{
		//Move model up to fix the pivot (the idle animation pivot is wrong)
		
		m_pPivotedModel->GetTransform()->Rotate(0, heading, 0, false);
		m_pPivotedModel->GetTransform()->Translate(0, -5, 0);
		m_Animator->SetAnimation(0, false);
	}
	else
	{
		//Move model down to fix the pivot (the idle animation pivot is wrong)
		m_pPivotedModel->GetTransform()->Translate(0, -1.2f, 0);
		m_Animator->SetAnimation(1, false);
	}


	//SPRINGARM
	DirectX::XMFLOAT3 pivotWorldPos = m_pCameraPivot->GetTransform()->GetWorldPosition();

	physx::PxVec3 rayOrigin = {
		pivotWorldPos.x + (m_Radius + (-GetTransform()->GetForward().x + 2))
	   ,pivotWorldPos.y
	   ,pivotWorldPos.z + (m_Radius + (-GetTransform()->GetForward().z + 2))
	};

	physx::PxVec3 rayDir = {-m_pCamera->GetTransform()->GetForward().x, -m_pCamera->GetTransform()->GetForward().y, -m_pCamera->GetTransform()->GetForward().z };

	physx::PxRaycastBuffer hitInfo;


	if (GetScene()->GetPhysxProxy()->Raycast(rayOrigin, rayDir, -(m_SpringArmDistance), hitInfo, physx::PxHitFlag::eMESH_MULTIPLE))
	{
		m_pCamera->GetTransform()->Translate(m_pCamera->GetTransform()->GetPosition().x, m_pCamera->GetTransform()->GetPosition().y, -(hitInfo.block.distance - 5));
	}
	else
	{
		m_pCamera->GetTransform()->Translate(0, 0, m_SpringArmDistance);
	}


}

int Character::GetCoins()
{
	return m_CollectedCoins;
}

void Character::CollectCoin()
{
	m_CollectedCoins++;
}