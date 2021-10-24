#pragma once
#include "GameScene.h"

class ComponentsTestScene final : public GameScene
{
public:
	ComponentsTestScene();
	virtual ~ComponentsTestScene() = default;

	ComponentsTestScene(const ComponentsTestScene& other) = delete;
	ComponentsTestScene(ComponentsTestScene&& other) noexcept = delete;
	ComponentsTestScene& operator=(const ComponentsTestScene& other) = delete;
	ComponentsTestScene& operator=(ComponentsTestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	float m_FpsInterval{};
};


