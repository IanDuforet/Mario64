#include "stdafx.h"
#include "FixedCamera.h"
#include "TransformComponent.h"

void FixedCamera::SetRotation(float x, float y, float z)
{
	GetTransform()->Rotate(x, y, z);
}

void FixedCamera::Translate(float x, float y, float z)
{
	GetTransform()->Translate(x, y, z);
}

void FixedCamera::Initialize(const GameContext& )
{
	m_pCamera = new CameraComponent();
	AddComponent(m_pCamera);
}
