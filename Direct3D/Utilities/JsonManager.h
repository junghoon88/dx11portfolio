#pragma once

namespace Json
{
	void GetValue(Json::Value& parent, string name, bool& value);
	void SetValue(Json::Value& parent, string name, bool& value);

	void GetValue(Json::Value& parent, string name, float& value);
	void SetValue(Json::Value& parent, string name, float& value);

	void GetValue(Json::Value& parent, string name, int& value);
	void SetValue(Json::Value& parent, string name, int value);

	void GetValue(Json::Value& parent, string name, UINT& value);
	void SetValue(Json::Value& parent, string name, UINT& value);

	void GetValue(Json::Value& parent, string name, D3DXVECTOR2& value);
	void SetValue(Json::Value& parent, string name, D3DXVECTOR2& value);

	void GetValue(Json::Value& parent, string name, D3DXVECTOR3& value);
	void SetValue(Json::Value& parent, string name, D3DXVECTOR3& value);

	void GetValue(Json::Value& parent, string name, D3DXCOLOR& value);
	void SetValue(Json::Value& parent, string name, D3DXCOLOR& value);

	void GetValue(Json::Value& parent, string name, D3DXMATRIX& value);
	void SetValue(Json::Value& parent, string name, D3DXMATRIX& value);

	void GetValue(Json::Value& parent, string name, string& value);
	void SetValue(Json::Value& parent, string name, string& value);

	void GetValue(Json::Value& parent, string name, wstring& value);
	void SetValue(Json::Value& parent, string name, wstring& value);

	void ReadData(wstring file, Json::Value* root);
	void WriteData(wstring file, Json::Value* root);
}