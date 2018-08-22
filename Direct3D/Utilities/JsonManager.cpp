#include "stdafx.h"
#include "JsonManager.h"
#include <fstream>

void Json::GetValue(Json::Value & parent, string name, bool & value)
{
	value = parent[name.c_str()].asBool();
}

void Json::SetValue(Json::Value & parent, string name, bool & value)
{
	parent[name.c_str()] = value;
}

void Json::GetValue(Json::Value & parent, string name, float & value)
{
	value = parent[name.c_str()].asFloat();
}

void Json::SetValue(Json::Value & parent, string name, float & value)
{
	parent[name.c_str()] = value;
}

void Json::GetValue(Json::Value & parent, string name, int & value)
{
	value = parent[name.c_str()].asInt();
}

void Json::SetValue(Json::Value & parent, string name, int & value)
{
	parent[name.c_str()] = value;
}

void Json::GetValue(Json::Value & parent, string name, UINT & value)
{
	value = parent[name.c_str()].asUInt();
}

void Json::SetValue(Json::Value & parent, string name, UINT & value)
{
	parent[name.c_str()] = value;
}

void Json::GetValue(Json::Value & parent, string name, D3DXVECTOR2 & value)
{
	for (int i = 0; i < 2; i++)
		value[i] = parent[name.c_str()][i].asFloat();
}

void Json::SetValue(Json::Value & parent, string name, D3DXVECTOR2 & value)
{
	for (int i = 0; i < 2; i++)
		parent[name.c_str()].append(value[i]);
}

void Json::GetValue(Json::Value & parent, string name, D3DXVECTOR3 & value)
{
	for (int i = 0; i < 3; i++)
		value[i] = parent[name.c_str()][i].asFloat();
}

void Json::SetValue(Json::Value & parent, string name, D3DXVECTOR3 & value)
{
	for (int i = 0; i < 3; i++)
		parent[name.c_str()].append(value[i]);
}

void Json::GetValue(Json::Value & parent, string name, D3DXCOLOR & value)
{
	for (int i = 0; i < 4; i++)
		value[i] = parent[name.c_str()][i].asFloat();
}

void Json::SetValue(Json::Value & parent, string name, D3DXCOLOR & value)
{
	for (int i = 0; i < 4; i++)
		parent[name.c_str()].append(value[i]);
}

void Json::GetValue(Json::Value & parent, string name, D3DXMATRIX & value)
{
	for (int i = 0; i < 16; i++)
		value[i] = parent[name.c_str()][i].asFloat();
}

void Json::SetValue(Json::Value & parent, string name, D3DXMATRIX & value)
{
	for (int i = 0; i < 16; i++)
		parent[name.c_str()].append(value[i]);
}

void Json::GetValue(Json::Value & parent, string name, string & value)
{
	value = "";

	unsigned int size = parent[name.c_str()].size();
	for (size_t i = 0; i < size; i++)
		value.append(parent[name.c_str()][i].asString());
}

void Json::SetValue(Json::Value & parent, string name, string & value)
{
	parent[name.c_str()].append(value);
}

void Json::GetValue(Json::Value & parent, string name, wstring & value)
{
	string v("");
	Json::GetValue(parent, name, v);
	value = String::ToWString(v);
}

void Json::SetValue(Json::Value & parent, string name, wstring & value)
{
	parent[name.c_str()].append(String::ToString(value));
}

void Json::ReadData(wstring file, Json::Value * root)
{
	ifstream stream;

	stream.open(file);
	{
		Json::Reader reader;
		reader.parse(stream, *root);
	}
	stream.close();
}

void Json::WriteData(wstring file, Json::Value * root)
{
	ofstream stream;

	string temp = String::ToString(file);
	stream.open(temp);
	{
		Json::StyledWriter writer;

		stream << writer.write(*root);
	}
	stream.close();
}