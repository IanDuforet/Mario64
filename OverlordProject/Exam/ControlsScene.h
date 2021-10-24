#pragma once
#include "GameScene.h"
class SpriteComponent;
class ControlsScene final : public GameScene
{
public:

	enum class SelectedButton
	{
		Back
	};

	ControlsScene();
	virtual ~ControlsScene() = default;

	ControlsScene(const ControlsScene& other) = delete;
	ControlsScene(ControlsScene&& other) noexcept = delete;
	ControlsScene& operator=(const ControlsScene& other) = delete;
	ControlsScene& operator=(ControlsScene&& other) noexcept = delete;

	void PressedButton();

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	SelectedButton m_SelectedButton;

};
