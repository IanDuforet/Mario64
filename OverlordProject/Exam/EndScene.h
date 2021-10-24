#pragma once
#include "GameScene.h"
class SpriteComponent;
class EndScene final : public GameScene
{
public:

	enum class SelectedButton
	{
		Menu,
		Restart,
		Exit
	};

	EndScene();
	virtual ~EndScene() = default;

	EndScene(const EndScene& other) = delete;
	EndScene(EndScene&& other) noexcept = delete;
	EndScene& operator=(const EndScene& other) = delete;
	EndScene& operator=(EndScene&& other) noexcept = delete;

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
