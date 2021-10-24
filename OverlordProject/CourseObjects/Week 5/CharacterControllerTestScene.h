#pragma once
#include <GameScene.h>
class CharacterControllerTestScene :
    public GameScene
{
public:
	CharacterControllerTestScene();
	virtual ~CharacterControllerTestScene() = default;

	CharacterControllerTestScene(const CharacterControllerTestScene& other) = delete;
	CharacterControllerTestScene(CharacterControllerTestScene&& other) noexcept = delete;
	CharacterControllerTestScene& operator=(const CharacterControllerTestScene& other) = delete;
	CharacterControllerTestScene& operator=(CharacterControllerTestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	GameObject* m_pCharacter;
	GameObject* m_pLevel;
};

