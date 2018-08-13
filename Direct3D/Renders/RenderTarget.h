#pragma once

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	void Set(void);

	void Create(UINT width = 0, UINT height = 0);

private:
	void Delete(void);

public:
	inline UINT GetWidth(void)  { return width; }
	inline UINT GetHeight(void) { return height; }


	inline ID3D11Texture2D*			 GetTexture(void) { return texture; }
	inline ID3D11RenderTargetView*	 GetRTV(void) { return rtv; }
	inline ID3D11ShaderResourceView* GetSRV(void) { return srv; }


private:
	UINT width, height;

	ID3D11Texture2D*			texture; // 
	ID3D11RenderTargetView*		rtv;	 // 포스트 랜더링 작업 결과를 받기 위해
	ID3D11ShaderResourceView*	srv;	 // 다시 랜더링하기 위해
};
