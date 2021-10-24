#pragma once
#include "GameObject.h"

class ControllerComponent;
class CameraComponent;
class ModelAnimator;
class Character : public GameObject
{
public:
	enum CharacterMovement : UINT
	{
		LEFT = 0,
		RIGHT,
		FORWARD,
		BACKWARD,
		JUMP
	};

	Character(float radius = 2, float height = 5, float moveSpeed = 100);
	virtual ~Character() = default;

	Character(const Character& other) = delete;
	Character(Character&& other) noexcept = delete;
	Character& operator=(const Character& other) = delete;
	Character& operator=(Character&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	int GetCoins();
	void ResetCollectedCoins() { m_CollectedCoins = 0; }
	void CollectCoin();

	CameraComponent* GetCamera() const { return m_pCamera; }
	FMOD::Channel* m_pChannel;
	FMOD::Sound* m_pJumpSound;

protected:
	GameObject* m_pCameraPivot;
	GameObject* m_pPivotedModel;
	CameraComponent* m_pCamera;
	ControllerComponent* m_pController;
	ModelAnimator* m_Animator;
	float m_TotalPitch, m_TotalYaw;
	float m_TotalModelRoll;
	float m_MoveSpeed, m_RotationSpeed;
	float m_Radius, m_Height;
	float m_SpringArmDistance;
	int m_CollectedCoins;

	//Running
	float m_MaxRunVelocity,
		m_TerminalVelocity,
		m_Gravity,
		m_RunAccelerationTime,
		m_JumpAccelerationTime,
		m_RunAcceleration,
		m_JumpAcceleration,
		m_RunVelocity,
		m_JumpHeight,
		m_JumpVelocity;
	bool m_IsJumping;

	DirectX::XMFLOAT3 m_Velocity;
};


