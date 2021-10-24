#pragma once
#include "GameScene.h"
class SpriteComponent;
class StartScene final : public GameScene
{
public:

	enum class SelectedButton
	{
		Start,
		Controls,
		Exit
	};

	StartScene();
	virtual ~StartScene() = default;

	StartScene(const StartScene& other) = delete;
	StartScene(StartScene&& other) noexcept = delete;
	StartScene& operator=(const StartScene& other) = delete;
	StartScene& operator=(StartScene&& other) noexcept = delete;

	void UpdateButtonTexture();
	void PressedButton();

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	SelectedButton m_SelectedButton;
	SpriteComponent* m_StartSprite;
	SpriteComponent* m_ControlsSprite;
	SpriteComponent* m_ExitSprite;
};
