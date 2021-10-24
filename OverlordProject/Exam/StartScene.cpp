#include "stdafx.h"
#include "StartScene.h"
#include "SpriteComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "OverlordGame.h"
#include "SceneManager.h"

StartScene::StartScene(void) 
	: GameScene(L"StartScene")
	, m_SelectedButton(SelectedButton::Start)
	, m_StartSprite{nullptr}
	, m_ExitSprite{nullptr}
	, m_ControlsSprite{nullptr}
{

}

void StartScene::Initialize()
{
	int windowWidth = OverlordGame::GetGameSettings().Window.Width;
	int windowHeight = OverlordGame::GetGameSettings().Window.Height;

	//BACKGROUND
	GameObject* pBackground = new GameObject();
	SpriteComponent* pBackgroundSprite = new SpriteComponent(L"./Resources/Exam/Textures/Menu/StartBackground.png", DirectX::XMFLOAT2( 0.5f,0.5f ));
	pBackground->AddComponent(pBackgroundSprite);
	//Move to middle of screen
	AddChild(pBackground);
	pBackground->GetTransform()->Translate((float)windowWidth/2.f, (float)windowHeight/2.f, 0.9f);

	//EXIT
	GameObject* pExit = new GameObject();
	m_ExitSprite = new SpriteComponent(L"./Resources/Exam/Textures/Menu/Exit.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	pExit->AddComponent(m_ExitSprite);
	//Move to middle of screen
	AddChild(pExit);
	pExit->GetTransform()->Translate((float)windowWidth / 2.f, 560, 0.1f);

	//CONTROLS
	GameObject* pControls = new GameObject();
	m_ControlsSprite = new SpriteComponent(L"./Resources/Exam/Textures/Menu/Controls.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	pControls->AddComponent(m_ControlsSprite);
	//Move to middle of screen
	AddChild(pControls);
	pControls->GetTransform()->Translate((float)windowWidth / 2.f, 430, 0.1f);

	//START
	GameObject* pStart = new GameObject();
	m_StartSprite = new SpriteComponent(L"./Resources/Exam/Textures/Menu/StartSelected.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	pStart->AddComponent(m_StartSprite);
	//Move to middle of screen
	AddChild(pStart);
	pStart->GetTransform()->Translate((float)windowWidth / 2.f, 300, 0.1f);

	const auto gameContext = GetGameContext();

	//MAKE INPUT
	const InputAction down{ MenuInputIds::Down, InputTriggerState::Pressed, 'S' };
	const InputAction up{ MenuInputIds::Up, InputTriggerState::Pressed, 'W' };
	const InputAction select{ MenuInputIds::Select, InputTriggerState::Pressed, VK_RETURN };

	InputManager* pInput{ gameContext.pInput };
	pInput->AddInputAction(down);
	pInput->AddInputAction(up);
	pInput->AddInputAction(select);
}

void StartScene::Draw()
{

}

void StartScene::Update()
{
	const auto gameContext = GetGameContext();
	InputManager* pInput{ gameContext.pInput };
	if (pInput->IsActionTriggered(MenuInputIds::Down))
	{
		if (m_SelectedButton == SelectedButton::Exit)
			m_SelectedButton = SelectedButton::Start;
		else
			m_SelectedButton = (SelectedButton)((int)m_SelectedButton + 1);
		UpdateButtonTexture();
	}	
	if (pInput->IsActionTriggered(MenuInputIds::Up))
	{
		if (m_SelectedButton == SelectedButton::Start)
			m_SelectedButton = SelectedButton::Exit;
		else
			m_SelectedButton = (SelectedButton)((int)m_SelectedButton - 1);
		UpdateButtonTexture();	
	}
	if (pInput->IsActionTriggered(MenuInputIds::Select))
	{
		PressedButton();
	}

}

void StartScene::PressedButton()
{
	switch (m_SelectedButton)
	{
	case SelectedButton::Start:
		SceneManager::GetInstance()->ResetScene(L"MainScene");
		SceneManager::GetInstance()->SetActiveGameScene(L"MainScene");
		break;
	case SelectedButton::Controls:
		SceneManager::GetInstance()->SetActiveGameScene(L"ControlsScene");
		break;
	case SelectedButton::Exit:
		PostQuitMessage(0);
		break;
	}
}

void StartScene::UpdateButtonTexture()
{
	m_StartSprite->SetTexture(L"./Resources/Exam/Textures/Menu/Start.png");
	m_ExitSprite->SetTexture(L"./Resources/Exam/Textures/Menu/Exit.png");
	m_ControlsSprite->SetTexture(L"./Resources/Exam/Textures/Menu/Controls.png");


	switch (m_SelectedButton)
	{
		case SelectedButton::Start:
			m_StartSprite->SetTexture(L"./Resources/Exam/Textures/Menu/StartSelected.png");
		break;
		case SelectedButton::Controls:
			m_ControlsSprite->SetTexture(L"./Resources/Exam/Textures/Menu/ControlsSelected.png");
		break;
		case SelectedButton::Exit:
			m_ExitSprite->SetTexture(L"./Resources/Exam/Textures/Menu/ExitSelected.png");
		break;

	}
}

