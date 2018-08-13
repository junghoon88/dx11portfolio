#pragma once

#define gLight LightManager::Get()

class PointLight;
class SpotLight;

//쉐이더에 넣는 정보이므로 데이터 순서 중요함
struct PointLightData
{
	D3DXVECTOR3 Position = D3DXVECTOR3(0, 0, 0);
	float Intensity = 2.0f;
	D3DXVECTOR3 Color = D3DXVECTOR3(1, 1, 1);
	float Range = 5.0f;
};

//쉐이더에 넣는 정보이므로 데이터 순서 중요함
struct SpotLightData
{
	D3DXVECTOR3 Position	= D3DXVECTOR3(0, 2, 0);
	float		InnerAngle	= 0.75f;
	D3DXVECTOR3 Color		= D3DXVECTOR3(0, 1, 0);
	float		OuterAngle	= 0.25f;
	D3DXVECTOR3 Direction	= D3DXVECTOR3(0, -1, 0);
	float		Range		= 0.0f; //미적용
};


class LightManager
{
public:
	inline static LightManager* Get(void) { return instance; }
	inline static void Create(void) { instance = new LightManager(); }
	inline static void Delete(void) { SAFE_DELETE(instance); }

	void Update(void);
	void PostRender(void);

	void AddPointLight(wstring name, PointLightData data);
	void DeletePointLight(wstring name);
	PointLightData* GetPointLight(wstring name);

	void AddSpotLight(wstring name, SpotLightData data);
	void DeleteSpotLight(wstring name);
	SpotLightData* GetSpotLight(wstring name);

private:
	LightManager();
	~LightManager();

public:
	void LoadData(Json::Value* value);
	void SaveData(Json::Value* value);


private:
	static LightManager* instance;

	PointLight* pointLight;
	SpotLight*  spotLight;

	vector<pair<wstring, PointLightData>> pointLights;
	vector<pair<wstring, SpotLightData>>  spotLights;

	//PostRender 변수
	bool inputText1 = false;
	bool inputText2 = false;
	char nameBuf1[128] = "";
	char nameBuf2[128] = "";
	int  showPointLightIndex = -1;
	int  showSpotLightIndex = -1;

};
