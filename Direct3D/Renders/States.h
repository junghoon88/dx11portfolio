#pragma once

enum class BlendMethod
{
	ComposeLinear,			//선형합성
	ComposePlus,			//덧셈합성
	ComposeMinus,			//뺄셈합성
	ComposeProduct,			//곱셈합성
	ComposeSquare,			//제곱합성
	NegatinationPositive,	//네거티네이션 포지티브
	OnlyTexture1,			//텍스처1
	OnlyTexture2,			//텍스처2
	AddTexture12,			//텍스처12 합성
};


class States
{
public:
	static void Create(void);
	static void Delete(void);

	static void GetRasterizerDesc(D3D11_RASTERIZER_DESC* desc);
	static void CreateRasterizer(D3D11_RASTERIZER_DESC* desc, ID3D11RasterizerState** state);
	static void SetRasterizer(ID3D11RasterizerState* state);

	static void GetDepthStencilDesc(D3D11_DEPTH_STENCIL_DESC* desc);
	static void CreateDepthStencil(D3D11_DEPTH_STENCIL_DESC* desc, ID3D11DepthStencilState** state);
	static void SetDepthStencil(ID3D11DepthStencilState* state);

	static void GetSamplerDesc(D3D11_SAMPLER_DESC* desc);
	static void CreateSampler(D3D11_SAMPLER_DESC* desc, ID3D11SamplerState** state);
	static void SetSampler(UINT slot, UINT count, ID3D11SamplerState* state);

	static void GetBlendDesc(D3D11_BLEND_DESC* desc);
	static void GetBlendDesc(D3D11_BLEND_DESC* desc, UINT rendertargetIndex, BlendMethod method);
	static void CreateBlend(D3D11_BLEND_DESC* desc, ID3D11BlendState** state);

private:
	static void CreateRasterizerDesc(void);
	static void CreateDepthStencilDesc(void);
	static void CreateSamplerDesc(void);
	static void CreateBlendDesc(void);

	static D3D11_RASTERIZER_DESC	rasterizerDesc;
	static D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	static D3D11_SAMPLER_DESC		samplerDesc;
	static D3D11_BLEND_DESC			blendDesc;
};

