#include "stdafx.h"
#include "XmlToAnim.h"
#include "../Utilities/Xml.h"
#include "../Utilities/BinaryFile.h"

Fbx::XmlToAnim::XmlToAnim(wstring xmlFile)
{
	document = new Xml::XMLDocument();

	string path = String::ToString(xmlFile);
	Xml::XMLError error = document->LoadFile(path.c_str());
	assert(error == Xml::XML_SUCCESS);

	root = document->FirstChildElement();
}

Fbx::XmlToAnim::~XmlToAnim()
{
	SAFE_DELETE(document);
}

void Fbx::XmlToAnim::ExportAnimations(wstring animPath, wstring savePath)
{
	Fbx::XmlToAnim* anim = NULL;

	//Grund
	vector<wstring> files;
	Path::GetFiles(&files, animPath, L"*.Animation", true);
	for (size_t i = 0; i < files.size(); i++)
	{
		anim = new Fbx::XmlToAnim(files[i]);
		wstring name = Path::GetFileNameWithoutExtension(files[i]);
		anim->ExportAnimation(savePath, name);
		SAFE_DELETE(anim);
	}
}


void Fbx::XmlToAnim::ExportAnimation(wstring saveFolder, wstring saveName)
{
	ReadAnimation();

	WriteAnimation(saveFolder, saveName);
}

void Fbx::XmlToAnim::ReadAnimation()
{
	XmlElement* element;

	element = root->FirstChildElement();
	keyFrameCount = element->UnsignedText();

	element = element->NextSiblingElement();
	duration = element->FloatText();

	element = element->NextSiblingElement();

	Xml::XMLElement* temp = element->FirstChildElement();
	ReadKeyframe(temp);
}

void Fbx::XmlToAnim::ReadKeyframe(Xml::XMLElement * element)
{
	for (UINT i = 0; i < keyFrameCount; i++)
	{
		if (element == NULL) break;
		Xml::XMLElement* keyframe = element->FirstChildElement();

		XmlKeyframe* data = new XmlKeyframe();
		data->BoneName = keyframe->GetText();

		keyframe = keyframe->NextSiblingElement();
		data->KeyCount = keyframe->UnsignedText();

		keyframe = keyframe->NextSiblingElement();
		data->Duration = keyframe->FloatText();

		keyframe = keyframe->NextSiblingElement();
		data->KeyInterval = keyframe->FloatText();

		keyframe = keyframe->NextSiblingElement();
		bool HasTranslation = keyframe->BoolText();

		keyframe = keyframe->NextSiblingElement();
		bool HasRotation = keyframe->BoolText();

		keyframe = keyframe->NextSiblingElement();
		bool HasScale = keyframe->BoolText();

		keyframe = keyframe->NextSiblingElement();
		bool HasTime = keyframe->BoolText();

		keyframe = keyframe->NextSiblingElement();
		bool FixedTranslation = keyframe->BoolText();

		keyframe = keyframe->NextSiblingElement();
		bool FixedRotation = keyframe->BoolText();

		keyframe = keyframe->NextSiblingElement();
		bool FixedScale = keyframe->BoolText();

		if (HasTranslation == true)
		{
			keyframe = keyframe->NextSiblingElement();
			ReadTranslation(data, keyframe, data->KeyCount, FixedTranslation);
		}

		if (HasRotation == true)
		{
			keyframe = keyframe->NextSiblingElement();
			ReadRotation(data, keyframe, data->KeyCount, FixedRotation);
		}

		if (HasScale == true)
		{
			keyframe = keyframe->NextSiblingElement();
			ReadScale(data, keyframe, data->KeyCount, FixedScale);
		}

		if (HasTime == true)
		{
			keyframe = keyframe->NextSiblingElement();
			ReadTime(data, keyframe);
		}

		keyframes.push_back(data);
		element = element->NextSiblingElement();
	}
}

void Fbx::XmlToAnim::ReadTranslation(XmlKeyframe* keyframe, Xml::XMLElement * element, UINT keyCount, bool bFixed)
{
	Xml::XMLElement* tElement = element->FirstChildElement();

	if (bFixed == true)
	{
		D3DXVECTOR3 vec = GetVector3(tElement);
		keyframe->Translations.push_back(vec);
	}
	else
	{
		for (UINT i = 0; i < keyCount; i++)
		{
			if (tElement == NULL) break;

			D3DXVECTOR3 vec = GetVector3(tElement);
			keyframe->Translations.push_back(vec);

			tElement = tElement->NextSiblingElement();
		}
	}
}

void Fbx::XmlToAnim::ReadRotation(XmlKeyframe * keyframe, Xml::XMLElement * element, UINT keyCount, bool bFixed)
{
	Xml::XMLElement* tElement = element->FirstChildElement();

	if (bFixed == true)
	{
		D3DXQUATERNION q = GetQuaternion(tElement);
		keyframe->Rotations.push_back(q);
	}
	else
	{
		for (UINT i = 0; i < keyCount; i++)
		{
			if (tElement == NULL) break;

			D3DXQUATERNION q = GetQuaternion(tElement);
			keyframe->Rotations.push_back(q);

			tElement = tElement->NextSiblingElement();
		}
	}
}

void Fbx::XmlToAnim::ReadScale(XmlKeyframe * keyframe, Xml::XMLElement * element, UINT keyCount, bool bFixed)
{
	Xml::XMLElement* tElement = element->FirstChildElement();

	if (bFixed == true)
	{
		D3DXVECTOR3 vec = GetVector3(tElement);
		keyframe->Scales.push_back(vec);
	}
	else
	{
		for (UINT i = 0; i < keyCount; i++)
		{
			if (tElement == NULL) break;

			D3DXVECTOR3 vec = GetVector3(tElement);
			keyframe->Scales.push_back(vec);

			tElement = tElement->NextSiblingElement();
		}
	}
}

void Fbx::XmlToAnim::ReadTime(XmlKeyframe * keyframe, Xml::XMLElement * element)
{
	Xml::XMLElement* tElement = element->FirstChildElement();

	while (tElement != NULL)
	{
		float time = tElement->FloatText();
		keyframe->Times.push_back(time);

		tElement = tElement->NextSiblingElement();
	}
}

D3DXVECTOR3 Fbx::XmlToAnim::GetVector3(Xml::XMLElement * element)
{
	D3DXVECTOR3 temp(0, 0, 0);

	Xml::XMLElement* valElement = element->FirstChildElement();
	temp.x = valElement->FloatText();

	valElement = valElement->NextSiblingElement();
	temp.y = valElement->FloatText();

	valElement = valElement->NextSiblingElement();
	temp.z = valElement->FloatText();

	return temp;
}

D3DXQUATERNION Fbx::XmlToAnim::GetQuaternion(Xml::XMLElement * element)
{
	D3DXQUATERNION temp(0, 0, 0, 1);

	Xml::XMLElement* valElement = element->FirstChildElement();
	temp.x = valElement->FloatText();

	valElement = valElement->NextSiblingElement();
	temp.y = valElement->FloatText();

	valElement = valElement->NextSiblingElement();
	temp.z = valElement->FloatText();

	valElement = valElement->NextSiblingElement();
	temp.w = valElement->FloatText();

	return temp;
}

void Fbx::XmlToAnim::WriteAnimation(wstring saveFolder, wstring saveName)
{
	Path::CreateFolders(saveFolder);

	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + saveName + L".anim");

	w->UInt(keyFrameCount);
	w->Float(duration);

	UINT size = 0;
	for (XmlKeyframe* keyframe: keyframes)
	{
		w->String(keyframe->BoneName);
		w->UInt(keyframe->KeyCount);
		w->Float(keyframe->Duration);
		w->Float(keyframe->KeyInterval);

		size = keyframe->Translations.size();
		w->UInt(size);
		if (size > 0)
		{
			//벡터 복사 하지 않고 배열복사하도록
			D3DXVECTOR3* temp = (D3DXVECTOR3*)malloc(sizeof(D3DXVECTOR3)*size);
			assert(temp != NULL);
			for (size_t i = 0; i < size; i++)
			{
				temp[i] = keyframe->Translations[i];
			}
			w->Byte(&temp[0], sizeof(D3DXVECTOR3)*size);
			free(temp);
		}

		size = keyframe->Rotations.size();
		w->UInt(size);
		if (size > 0)
		{
			D3DXQUATERNION* temp = (D3DXQUATERNION*)malloc(sizeof(D3DXQUATERNION)*size);
			assert(temp != NULL);
			for (size_t i = 0; i < size; i++)
			{
				temp[i] = keyframe->Rotations[i];
			}
			w->Byte(&temp[0], sizeof(D3DXQUATERNION)*size);
			free(temp);
		}

		size = keyframe->Scales.size();
		w->UInt(size);
		if (size > 0)
		{
			D3DXVECTOR3* temp = (D3DXVECTOR3*)malloc(sizeof(D3DXVECTOR3)*size);
			assert(temp != NULL);
			for (size_t i = 0; i < size; i++)
			{
				temp[i] = keyframe->Scales[i];
			}
			w->Byte(&temp[0], sizeof(D3DXVECTOR3)*size);
			free(temp);
		}

		size = keyframe->Times.size();
		w->UInt(size);
		if (size > 0)
		{
			float* temp = (float*)malloc(sizeof(float)*size);
			assert(temp != NULL);
			for (size_t i = 0; i < size; i++)
			{
				temp[i] = keyframe->Times[i];
			}
			w->Byte(&temp[0], sizeof(float)*size);
			free(temp);
		}
	}


	w->Close();
	SAFE_DELETE(w);
}