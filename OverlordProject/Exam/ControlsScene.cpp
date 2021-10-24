#include "stdafx.h"
#include "ControlsScene.h"
#include "SpriteComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "OverlordGame.h"
#include "SceneManager.h"

ControlsScene::ControlsScene(void)
	: GameScene(L"ControlsScene")
	, m_SelectedButton(SelectedButton::Back)
{

}

void ControlsScene::Initialize()
{
	int windowWidth = OverlordGame::GetGameSettings().Window.Width;
	int windowHeight = OverlordGame::GetGameSettings().Window.Height;

	//BACKGROUND
	GameObject* pBackground = new GameObject();
	SpriteComponent* pBackgroundSprite = new SpriteComponent(L"./Resources/Exam/Textures/Menu/ControlsBackground.png", DirectX::XMFLOAT2( 0.5f,0.5f ));
	pBackground->AddComponent(pBackgroundSprite);
	//Move to middle of screen
	AddChild(pBackground);
	pBackground->GetTransform()->Translate((float)windowWidth/2.f, (float)windowHeight/2.f, 0.9f);

	//EXIT
	GameObject* pBack = new GameObject();
	SpriteComponent* pBackSprite = new SpriteComponent(L"./Resources/Exam/Textures/Menu/BackSelected.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	pBack->AddComponent(pBackSprite);
	//Move to middle of screen
	AddChild(pBack);
	pBack->GetTransform()->Translate((float)windowWidth / 2.f, 585, 0.1f);

	const auto gameContext = GetGameContext();

	//MAKE INPUT
	const InputAction select{ MenuInputIds::Select, InputTriggerState::Pressed, VK_RETURN };

	InputManager* pInput{ gameContext.pInput };
	pInput->AddInputAction(select);
}

void ControlsScene::Draw()
{

}

void ControlsScene::Update()
{
	const auto gameContext = GetGameContext();
	InputManager* pInput{ gameContext.pInput };
	if (pInput->IsActionTriggered(MenuInputIds::Select))
	{
		PressedButton();
	}

}

void ControlsScene::PressedButton()
{
	switch (m_SelectedButton)
	{
	case SelectedButton::Back:
		SceneManager::GetInstance()->SetActiveGameScene(L"StartScene");
		break;
	}
}


