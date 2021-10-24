#pragma once
#include "PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;

class PostPixelate : public PostProcessingMaterial
{
public:
	PostPixelate();
	virtual ~PostPixelate() = default;

	PostPixelate(const PostPixelate& other) = delete;
	PostPixelate(PostPixelate&& other) noexcept = delete;
	PostPixelate& operator=(const PostPixelate& other) = delete;
	PostPixelate& operator=(PostPixelate&& other) noexcept = delete;
protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
};
