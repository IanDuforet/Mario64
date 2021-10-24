#pragma once
#include "GameScene.h"

class ModelComponent;
class SpriteFont;
class Character;
class Pickup;
class MainScene final : public GameScene
{
public:
	MainScene();
	virtual ~MainScene() = default;

	MainScene(const MainScene& other) = delete;
	MainScene(MainScene&& other) noexcept = delete;
	MainScene& operator=(const MainScene& other) = delete;
	MainScene& operator=(MainScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Reset() override;
	void SceneActivated() override;
	void SceneDeactivated() override;
private:
	ModelComponent* m_pModel = nullptr;

	FMOD::Sound* m_pSound;
	SpriteFont* m_pFont = nullptr;
	Character* m_pCharacter;
	std::vector<Pickup*> m_pPickups;
	FMOD::Channel* m_pChannel;

	float m_pLevelScale;
	void CreateLevel();
	void CreateMaterials();
	void CreateCoins();
};

