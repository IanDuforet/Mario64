#pragma once
#include <GameObject.h>
	class ModelComponent;
	class ParticleEmitterComponent;
class Pickup :
    public GameObject
{
public:
	Pickup(DirectX::XMFLOAT3 position);
	virtual ~Pickup() = default;

	Pickup(const Pickup& other) = delete;
	Pickup(Pickup&& other) noexcept = delete;
	Pickup& operator=(const Pickup& other) = delete;
	Pickup& operator=(Pickup&& other) noexcept = delete;

	void Initialize(const GameContext & gameContext) override;
	void PostInitialize(const GameContext & gameContext) override;
	void Update(const GameContext & gameContext) override;
	void Reset();

private:
	FMOD::Channel* m_pChannel;
	FMOD::Sound* m_pCoinSound;
	DirectX::XMFLOAT3 m_Position;
	ParticleEmitterComponent* m_pParticleEmitter;
	float m_CoinSize;
	float m_TotalRotation;
	float m_RotationSpeed;
};

