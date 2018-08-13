#pragma once

#define gDevice D3D::GetDevice()
#define gDC		D3D::GetDC()

#define WINSIZEX	1280
#define WINSIZEY	720

struct D3DDesc
{
	wstring AppName;
	HINSTANCE Instance;
	HWND Handle;
	float Width;
	float Height;
	bool bVsync;
	bool bFullScreen;

	D3DDesc()
	{
		AppName		= L"FrameworkDX11";
		Instance	= NULL;
		Handle		= NULL;
		Width		= WINSIZEX;
		Height		= WINSIZEY;
		bVsync		= false;
		bFullScreen = false;
	}
};

class D3D
{
public:
	inline static D3D* Get(void) { return instance; }

	inline static void Create(void) { assert(instance == NULL); instance = new D3D(); }
	inline static void Delete(void) { SAFE_DELETE(instance); }

	inline static ID3D11Device* GetDevice(void) { return device; }
	inline static ID3D11DeviceContext* GetDC(void) { return deviceContext; }
	inline static IDXGISwapChain* GetSwapChain(void) { return swapChain; }

	inline static void GetDesc(D3DDesc* desc) { *desc = d3dDesc; }
	inline static void SetDesc(D3DDesc& desc) { d3dDesc = desc; }

	void SetRenderTarget(ID3D11RenderTargetView* rtv = NULL, ID3D11DepthStencilView* dsv = NULL);

	void Clear(D3DXCOLOR color = D3DXCOLOR(0xFF555566), ID3D11RenderTargetView* rtv = NULL, ID3D11DepthStencilView* dsv = NULL);
	void Present(void);

	void ResizeScreen(float width, float height);

private:
	D3D();
	~D3D();

	void SetGpuInfo(void);
	void CreateSwapChainAndDevice(void);
	void CreateBackBuffer(float width, float height);
	void DeleteBackBuffer(void);

private:
	static D3D*					instance;

	static D3DDesc				d3dDesc;
	static ID3D11Device*		device;
	static ID3D11DeviceContext* deviceContext;
	static IDXGISwapChain*		swapChain;

	UINT gpuMemorySize;
	wstring gpuDescription;

	UINT numerator;
	UINT denominator;

	ID3D11Texture2D* backBuffer;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RenderTargetView* renderTargetView;
};

