#pragma once
#include "GameScene.h"
class SpriteComponent;
class PauseScene final : public GameScene
{
public:

	enum class SelectedButton
	{
		Menu,
		Restart,
		Exit
	};

	PauseScene();
	virtual ~PauseScene() = default;

	PauseScene(const PauseScene& other) = delete;
	PauseScene(PauseScene&& other) noexcept = delete;
	PauseScene& operator=(const PauseScene& other) = delete;
	PauseScene& operator=(PauseScene&& other) noexcept = delete;

	void UpdateButtonTexture();
	void PressedButton();

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	SelectedButton m_SelectedButton;
	SpriteComponent* m_MenuSprite;
	SpriteComponent* m_RestartSprite;
	SpriteComponent* m_ExitSprite;
};
