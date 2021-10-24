#pragma once
#include "GameScene.h"

class SpherePrefab;
class CubePrefab;
class FixedCamera;
class PongScene final : public GameScene
{
public:
	PongScene();
	virtual ~PongScene() = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	float m_PaddleSpeed;
	SpherePrefab* m_pSphere;
	CubePrefab* m_pPaddleLeft;
	CubePrefab* m_pPaddleRight;
	FixedCamera* m_pTopCamera;
	void Initialize() override;
	void Update() override;
	void Draw() override;
};


