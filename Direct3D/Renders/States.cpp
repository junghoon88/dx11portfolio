#include "stdafx.h"
#include "States.h"

D3D11_RASTERIZER_DESC		States::rasterizerDesc;
D3D11_DEPTH_STENCIL_DESC	States::depthStencilDesc;
D3D11_SAMPLER_DESC			States::samplerDesc;
D3D11_BLEND_DESC			States::blendDesc;

void States::Create(void)
{
	CreateRasterizerDesc();
	CreateDepthStencilDesc();
	CreateSamplerDesc();
	CreateBlendDesc();
}

void States::Delete(void)
{
}

void States::GetRasterizerDesc(D3D11_RASTERIZER_DESC* desc)
{
	memcpy(desc, &rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
}

void States::CreateRasterizer(D3D11_RASTERIZER_DESC* desc, ID3D11RasterizerState** state)
{
	HRESULT hr;

	hr = gDevice->CreateRasterizerState(desc, state);
	assert(SUCCEEDED(hr));
}

void States::SetRasterizer(ID3D11RasterizerState * state)
{
	gDC->RSSetState(state);
}

void States::GetDepthStencilDesc(D3D11_DEPTH_STENCIL_DESC* desc)
{
	memcpy(desc, &depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
}

void States::CreateDepthStencil(D3D11_DEPTH_STENCIL_DESC * desc, ID3D11DepthStencilState ** state)
{
	HRESULT hr;

	hr = gDevice->CreateDepthStencilState(desc, state);
	assert(SUCCEEDED(hr));
}

void States::SetDepthStencil(ID3D11DepthStencilState * state)
{
	gDC->OMSetDepthStencilState(state, 1);
}

void States::GetSamplerDesc(D3D11_SAMPLER_DESC * desc)
{
	memcpy(desc, &samplerDesc, sizeof(D3D11_SAMPLER_DESC));
}

void States::CreateSampler(D3D11_SAMPLER_DESC * desc, ID3D11SamplerState ** state)
{
	HRESULT hr;

	hr = gDevice->CreateSamplerState(desc, state);
	assert(SUCCEEDED(hr));
}

void States::SetSampler(UINT slot, UINT count, ID3D11SamplerState * state)
{
	gDC->PSSetSamplers(slot, count, &state);
}

void States::GetBlendDesc(D3D11_BLEND_DESC * desc)
{
	memcpy(desc, &blendDesc, sizeof(D3D11_BLEND_DESC));
}

void States::GetBlendDesc(D3D11_BLEND_DESC * desc, UINT rendertargetIndex, BlendMethod method)
{
	memcpy(desc, &blendDesc, sizeof(D3D11_BLEND_DESC));

	desc->RenderTarget[0].BlendEnable = true;

	switch (method)
	{
	case BlendMethod::ComposeLinear:
		desc->RenderTarget[rendertargetIndex].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc->RenderTarget[rendertargetIndex].SrcBlend  = D3D11_BLEND_SRC_ALPHA;
		desc->RenderTarget[rendertargetIndex].BlendOp   = D3D11_BLEND_OP_ADD;
		break;
	case BlendMethod::ComposePlus:
		desc->RenderTarget[rendertargetIndex].DestBlend = D3D11_BLEND_ONE;
		desc->RenderTarget[rendertargetIndex].SrcBlend  = D3D11_BLEND_SRC_ALPHA;
		desc->RenderTarget[rendertargetIndex].BlendOp   = D3D11_BLEND_OP_ADD;
		break;
	case BlendMethod::ComposeMinus:
		desc->RenderTarget[rendertargetIndex].DestBlend = D3D11_BLEND_ONE;
		desc->RenderTarget[rendertargetIndex].SrcBlend  = D3D11_BLEND_SRC_ALPHA;
		desc->RenderTarget[rendertargetIndex].BlendOp   = D3D11_BLEND_OP_REV_SUBTRACT;
		break;
	case BlendMethod::ComposeProduct:
		desc->RenderTarget[rendertargetIndex].DestBlend = D3D11_BLEND_SRC_COLOR;
		desc->RenderTarget[rendertargetIndex].SrcBlend  = D3D11_BLEND_ZERO;
		desc->RenderTarget[rendertargetIndex].BlendOp   = D3D11_BLEND_OP_ADD;
		break;
	case BlendMethod::ComposeSquare:
		desc->RenderTarget[rendertargetIndex].DestBlend = D3D11_BLEND_DEST_COLOR;
		desc->RenderTarget[rendertargetIndex].SrcBlend  = D3D11_BLEND_ZERO;
		desc->RenderTarget[rendertargetIndex].BlendOp   = D3D11_BLEND_OP_ADD;
		break;
	case BlendMethod::NegatinationPositive:
		desc->RenderTarget[rendertargetIndex].DestBlend = D3D11_BLEND_ZERO;
		desc->RenderTarget[rendertargetIndex].SrcBlend  = D3D11_BLEND_INV_DEST_COLOR;
		desc->RenderTarget[rendertargetIndex].BlendOp   = D3D11_BLEND_OP_ADD;
		break;
	case BlendMethod::OnlyTexture1:
		desc->RenderTarget[rendertargetIndex].DestBlend = D3D11_BLEND_ONE;
		desc->RenderTarget[rendertargetIndex].SrcBlend  = D3D11_BLEND_ZERO;
		desc->RenderTarget[rendertargetIndex].BlendOp   = D3D11_BLEND_OP_ADD;
		break;
	case BlendMethod::OnlyTexture2:
		desc->RenderTarget[rendertargetIndex].DestBlend = D3D11_BLEND_ZERO;
		desc->RenderTarget[rendertargetIndex].SrcBlend = D3D11_BLEND_ONE;
		desc->RenderTarget[rendertargetIndex].BlendOp = D3D11_BLEND_OP_ADD;
		break;
	case BlendMethod::AddTexture12:
		desc->RenderTarget[rendertargetIndex].DestBlend = D3D11_BLEND_ONE;
		desc->RenderTarget[rendertargetIndex].SrcBlend = D3D11_BLEND_ONE;
		desc->RenderTarget[rendertargetIndex].BlendOp = D3D11_BLEND_OP_ADD;
		desc->RenderTarget[rendertargetIndex].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc->RenderTarget[rendertargetIndex].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc->RenderTarget[rendertargetIndex].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		break;
	}
}

void States::CreateBlend(D3D11_BLEND_DESC * desc, ID3D11BlendState ** state)
{
	HRESULT hr;

	hr = gDevice->CreateBlendState(desc, state);
	assert(SUCCEEDED(hr));
}


void States::CreateRasterizerDesc(void)
{
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
}

void States::CreateDepthStencilDesc(void)
{
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
}

void States::CreateSamplerDesc(void)
{
	//MIN_MAG_MIP  
	//MIN : �ؽ��İ� ��ҵ�����(��:�ؽ���(256x256)�� ȭ�鿡�� ( 64x 64)�� ���� ��)
	//MAG : �ؽ��İ� Ȯ�������(��:�ؽ���(256x256)�� ȭ�鿡�� (512x512)�� ���� ��)
	//MIP : �Ӹ� : �ؽ��Ŀ��� �������� 2�ǽ¼� ũ�⸶�� ���ο� �ؽ��ĸ� �����.
	//             (��: �ؽ���(256x256) -> �ؽ���1(512x512), �ؽ���2(128x128), �ؽ���3(64x64) ...
	//             ���Ŀ� ������ �������� �Ѵܰ�ū��, �Ѵܰ� �������� ���� �ؽ��� 2���� �������� �����Ѵ�.
	//POINT : �������
	//LINEAR : ��������

	//���� ���͸� : ī�޶� ������ ������ ������� ǰ�������� ���� ���

	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
}

void States::CreateBlendDesc(void)
{
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlend	= D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp	= D3D11_BLEND_OP_ADD;

	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].SrcBlendAlpha	 = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha	 = D3D11_BLEND_OP_ADD;

	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
}