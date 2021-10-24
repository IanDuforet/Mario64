#pragma once
#include "GameObject.h"

struct GameContext;
class CameraComponent;

class FixedCamera final : public GameObject
{
	CameraComponent* m_pCamera{};
public:
	void SetRotation(float x, float y, float z);
	void Translate(float x, float y, float z);
	
protected:
	void Initialize(const GameContext& gameContext) override;
};
