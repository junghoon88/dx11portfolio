#pragma once

class Shader
{
public:
	Shader(wstring shaderFile, string vsName = "VS", string psName = "PS");
	~Shader();

	void Render(void);

	void SelectVSFunc(string vsName);
	void SelectPSFunc(string psName);


private:
	void Create(void);
	void Delete(void);

	void CreateVertexShader(void);
	void CreatePixelShader(void);
	void CheckShaderError(HRESULT hr, ID3DBlob* error);
	void CreateInputLayout(void);

public:
	inline wstring GetFile(void) { return shaderFile; }

private:
	wstring shaderFile;
	string vsName;
	string psName;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;

	ID3D11InputLayout* inputLayout;

	ID3DBlob* vertexBlob;
	ID3DBlob* pixelBlob;

	ID3D11ShaderReflection* reflection;
};