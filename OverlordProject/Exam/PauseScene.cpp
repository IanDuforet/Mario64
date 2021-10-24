#include "stdafx.h"
#include "PauseScene.h"
#include "SpriteComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "OverlordGame.h"
#include "SceneManager.h"
#include "MainScene.h"

PauseScene::PauseScene(void)
	: GameScene(L"PauseScene")
	, m_SelectedButton(SelectedButton::Menu)
	, m_ExitSprite{nullptr}
	, m_MenuSprite{nullptr}
	, m_RestartSprite{nullptr}
{

}

void PauseScene::Initialize()
{
	int windowWidth = OverlordGame::GetGameSettings().Window.Width;
	int windowHeight = OverlordGame::GetGameSettings().Window.Height;

	//BACKGROUND
	GameObject* pBackground = new GameObject();
	SpriteComponent* pBackgroundSprite = new SpriteComponent(L"./Resources/Exam/Textures/Menu/PausedBackground.png", DirectX::XMFLOAT2( 0.5f,0.5f ));
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
	m_RestartSprite = new SpriteComponent(L"./Resources/Exam/Textures/Menu/Restart.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	pControls->AddComponent(m_RestartSprite);
	//Move to middle of screen
	AddChild(pControls);
	pControls->GetTransform()->Translate((float)windowWidth / 2.f, 430, 0.1f);

	//START
	GameObject* pStart = new GameObject();
	m_MenuSprite = new SpriteComponent(L"./Resources/Exam/Textures/Menu/MenuSelected.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	pStart->AddComponent(m_MenuSprite);
	//Move to middle of screen
	AddChild(pStart);
	pStart->GetTransform()->Translate((float)windowWidth / 2.f, 300, 0.1f);

	const auto gameContext = GetGameContext();

	//MAKE INPUT
	const InputAction down{ MenuInputIds::Down, InputTriggerState::Pressed, 'S' };
	const InputAction up{ MenuInputIds::Up, InputTriggerState::Pressed, 'W' };
	const InputAction select{ MenuInputIds::Select, InputTriggerState::Pressed, VK_RETURN };
	const InputAction resume{ InputIds::Pause, InputTriggerState::Pressed, VK_ESCAPE };

	InputManager* pInput{ gameContext.pInput };
	pInput->AddInputAction(down);
	pInput->AddInputAction(up);
	pInput->AddInputAction(select);
	pInput->AddInputAction(resume);
}

void PauseScene::Draw()
{

}

void PauseScene::Update()
{
	const auto gameContext = GetGameContext();
	InputManager* pInput{ gameContext.pInput };
	if (pInput->IsActionTriggered(MenuInputIds::Down))
	{
		if (m_SelectedButton == SelectedButton::Exit)
			m_SelectedButton = SelectedButton::Menu;
		else
			m_SelectedButton = (SelectedButton)((int)m_SelectedButton + 1);
		UpdateButtonTexture();
	}	
	if (pInput->IsActionTriggered(MenuInputIds::Up))
	{
		if (m_SelectedButton == SelectedButton::Menu)
			m_SelectedButton = SelectedButton::Exit;
		else
			m_SelectedButton = (SelectedButton)((int)m_SelectedButton - 1);
		UpdateButtonTexture();	
	}
	if (pInput->IsActionTriggered(MenuInputIds::Select))
	{
		PressedButton();
	}	
	if (pInput->IsActionTriggered(InputIds::Pause))
	{
		SceneManager::GetInstance()->SetActiveGameScene(L"MainScene");
	}

}

void PauseScene::PressedButton()
{
	switch (m_SelectedButton)
	{
	case SelectedButton::Menu:
		SceneManager::GetInstance()->SetActiveGameScene(L"StartScene");
		break;
	case SelectedButton::Restart:
		SceneManager::GetInstance()->ResetScene(L"MainScene");
		SceneManager::GetInstance()->SetActiveGameScene(L"MainScene");
		break;
	case SelectedButton::Exit:
		PostQuitMessage(0);
		break;
	}
}

void PauseScene::UpdateButtonTexture()
{
	m_MenuSprite->SetTexture(L"./Resources/Exam/Textures/Menu/Menu.png");
	m_ExitSprite->SetTexture(L"./Resources/Exam/Textures/Menu/Exit.png");
	m_RestartSprite->SetTexture(L"./Resources/Exam/Textures/Menu/Restart.png");


	switch (m_SelectedButton)
	{
		case SelectedButton::Menu:
			m_MenuSprite->SetTexture(L"./Resources/Exam/Textures/Menu/MenuSelected.png");
		break;
		case SelectedButton::Restart:
			m_RestartSprite->SetTexture(L"./Resources/Exam/Textures/Menu/RestartSelected.png");
		break;
		case SelectedButton::Exit:
			m_ExitSprite->SetTexture(L"./Resources/Exam/Textures/Menu/ExitSelected.png");
		break;

	}
}

