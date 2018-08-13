#pragma once

namespace Xml
{
	class XMLDocument;
	class XMLElement;
}

namespace Fbx
{
	struct XmlKeyframe
	{
		string BoneName;
		UINT KeyCount;
		float Duration;
		float KeyInterval;

		vector<D3DXVECTOR3> Translations;
		vector<D3DXQUATERNION> Rotations;
		vector<D3DXVECTOR3> Scales;
		vector<float> Times;
	};

	class XmlToAnim
	{
	public:
		XmlToAnim(wstring xmlFile);
		~XmlToAnim();

	public:
		void ExportAnimations(wstring animPath, wstring savePath);


	public:
		void ExportAnimation(wstring saveFolder, wstring saveName);

	private:
		void ReadAnimation();
		void ReadKeyframe(Xml::XMLElement* element);

		void ReadTranslation(XmlKeyframe* keyframe, Xml::XMLElement* element, UINT keyCount, bool bFixed);
		void ReadRotation(XmlKeyframe* keyframe, Xml::XMLElement* element, UINT keyCount, bool bFixed);
		void ReadScale(XmlKeyframe* keyframe, Xml::XMLElement* element, UINT keyCount, bool bFixed);
		void ReadTime(XmlKeyframe* keyframe, Xml::XMLElement* element);

		D3DXVECTOR3 GetVector3(Xml::XMLElement * element);
		D3DXQUATERNION GetQuaternion(Xml::XMLElement * element);

		void WriteAnimation(wstring saveFolder, wstring saveName);

	private:
		class Xml::XMLDocument* document;
		Xml::XMLElement* root;

	private:
		UINT keyFrameCount;
		float duration;

		vector<XmlKeyframe*> keyframes;
	};
}