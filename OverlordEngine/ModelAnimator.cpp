#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
m_pMeshFilter(pMeshFilter),
m_Transforms(std::vector<DirectX::XMFLOAT4X4>()),
m_IsPlaying(false), 
m_Reversed(false),
m_ClipSet(false),
m_TickCount(0),
m_AnimationSpeed(1.0f)
{
	SetAnimation(0, true);
}

void ModelAnimator::SetAnimation(UINT clipNumber, bool reset)
{
	UNREFERENCED_PARAMETER(clipNumber);
	//THANK YOU MAURO FOR TEACHING ME ABOUT THE RESET IMPLEMENTATION
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (clipNumber < m_pMeshFilter->m_AnimationClips.size())
	{
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//	Call SetAnimation(AnimationClip clip)
		SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber], reset);
	}
	else
	{
	//If not,
		//	Call Reset
		//	Log a warning with an appropriate message
		//	return
		if (reset)
			Reset();

		Logger::LogWarning(L"Can't set animations");
		return;
	}
}

void ModelAnimator::SetAnimation(std::wstring clipName, bool reset)
{
	UNREFERENCED_PARAMETER(clipName);
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	for (unsigned int i = 0; i < m_pMeshFilter->m_AnimationClips.size(); i++)
	{
		if (clipName == m_pMeshFilter->m_AnimationClips[i].Name)
		{
			//If found,
			//	Call SetAnimation(Animation Clip) with the found clip
			SetAnimation(m_pMeshFilter->m_AnimationClips[i], reset);
		}
		else
		{
			//Else
			//	Call Reset
			if(reset)
				Reset();
			//	Log a warning with an appropriate message
			Logger::LogWarning(L"Can't set animations, animation not found");

		}
	}
}

void ModelAnimator::SetAnimation(AnimationClip clip, bool reset)
{
	UNREFERENCED_PARAMETER(clip);
	//TODO: complete
	//Set m_ClipSet to true
	m_ClipSet = true;
	//Set m_CurrentClip
	m_CurrentClip = clip;
	//Call Reset(false)
	if(reset)
		Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	UNREFERENCED_PARAMETER(pause);
	//TODO: complete
	//If pause is true, set m_IsPlaying to false
	if (pause)
		m_IsPlaying = false;
	//Set m_TickCount to zero
	m_TickCount = 0;
	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.0f;

	//If m_ClipSet is true
		//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
		//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
	if (m_ClipSet)
		m_Transforms.assign(m_CurrentClip.Keys[0].BoneTransforms.begin(), m_CurrentClip.Keys[0].BoneTransforms.end());
	else
	{
		//Else
		//	Create an IdentityMatrix 
		//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
		DirectX::XMFLOAT4X4 identity;
		identity._11 = 1.0f; //reset to 1 each for idendity
		identity._22 = 1.0f;
		identity._33 = 1.0f;
		identity._44 = 1.0f;
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, identity);
	}
}

void ModelAnimator::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: complete
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		//auto passedTicks = ...
		auto passedTicks = gameContext.pGameTime->GetElapsed() * m_CurrentClip.TicksPerSecond * m_AnimationSpeed;
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		passedTicks = fmod(passedTicks, m_CurrentClip.Duration);

		//2. 
		//IF m_Reversed is true
		if (m_Reversed)
		{
			//	Subtract passedTicks from m_TickCount
			m_TickCount -= passedTicks;
			//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
			if (m_TickCount < 0)
				m_TickCount += m_CurrentClip.Duration;
		}
		else
		{
			//ELSE
			//	Add passedTicks to m_TickCount
			m_TickCount += passedTicks;
			//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
			if (m_TickCount > m_CurrentClip.Duration)
				m_TickCount -= m_CurrentClip.Duration;
		}

		//3.
		//Find the enclosing keys
		AnimationKey keyA = m_CurrentClip.Keys[0];
		AnimationKey keyB = m_CurrentClip.Keys[m_CurrentClip.Keys.size()-1];
		//Iterate all the keys of the clip and find the following keys:
		for (const AnimationKey& key : m_CurrentClip.Keys)
		{
			//keyA > Closest Key with Tick before/smaller than m_TickCount
			if (key.Tick > keyA.Tick && key.Tick < m_TickCount)
				keyA = key;
			//keyB > Closest Key with Tick after/bigger than m_TickCount
			else if (key.Tick < keyB.Tick && key.Tick > m_TickCount)
				keyB = key;
		}

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		float blendFactor = (m_TickCount - keyA.Tick) / (keyB.Tick - keyA.Tick); //Thank you Enrique for helping me with this formula

		//Clear the m_Transforms vector
		m_Transforms.clear();
		//FOR every boneTransform in a key (So for every bone)
		
		for (unsigned int i = 0; i < keyA.BoneTransforms.size(); i++)
		{
		//	Retrieve the transform from keyA (transformA)
		//	auto transformA = ...
			auto transformA = keyA.BoneTransforms[i];
		// 	Retrieve the transform from keyB (transformB)
		//	auto transformB = ...
			auto transformB = keyB.BoneTransforms[i];
		//	Decompose both transforms
			DirectX::XMVECTOR positionA;
			DirectX::XMVECTOR rotationA;
			DirectX::XMVECTOR scaleA;
			DirectX::XMMATRIX loadedMatrixA = DirectX::XMLoadFloat4x4(&transformA);
			DirectX::XMMatrixDecompose(&scaleA, &rotationA, &positionA, loadedMatrixA);

			DirectX::XMVECTOR positionB;
			DirectX::XMVECTOR rotationB;
			DirectX::XMVECTOR scaleB;
			DirectX::XMMATRIX loadedMatrixB = DirectX::XMLoadFloat4x4(&transformB);
			DirectX::XMMatrixDecompose(&scaleB, &rotationB, &positionB, loadedMatrixB);

		//	Lerp between all the transformations (Position, Scale, Rotation)
			DirectX::XMVECTOR lerpedPosition = DirectX::XMVectorLerp(positionA, positionB, blendFactor);
			DirectX::XMVECTOR lerpedRotation = DirectX::XMQuaternionSlerp(rotationA, rotationB, blendFactor);;
			DirectX::XMVECTOR lerpedScale = DirectX::XMVectorLerp(scaleA, scaleB, blendFactor);;

		////	Compose a transformation matrix with the lerp-results
			DirectX::XMMATRIX translationMatrix{ DirectX::XMMatrixTranslationFromVector(lerpedPosition) };
			DirectX::XMMATRIX rotationMatrix{ DirectX::XMMatrixRotationQuaternion(lerpedRotation) };
			DirectX::XMMATRIX scaleMatrix{ DirectX::XMMatrixScalingFromVector(lerpedScale)};

			DirectX::XMMATRIX finalMatrix{scaleMatrix * rotationMatrix * translationMatrix};
			DirectX::XMFLOAT4X4 storedFinalMatrix;
			DirectX::XMStoreFloat4x4(&storedFinalMatrix, finalMatrix);

		//	Add the resulting matrix to the m_Transforms vector
			m_Transforms.push_back(storedFinalMatrix);
		}


	}
}
