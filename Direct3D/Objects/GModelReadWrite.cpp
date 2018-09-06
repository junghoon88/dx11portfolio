#include "stdafx.h"
#include "GModelReadWrite.h"
#include "GameModel.h"
#include "../Units/GamePlayer.h"
#include "../Units/GameEnemyRobot.h"

#include "../Model/Bounding/BoundingBox.h"
#include "../Model/Bounding/BoundingCapsule.h"

void GModelReadWriter::OpenGModel(wstring file, GModelReadClass readClass, void** model)
{
	Json::Value root;
	Json::ReadData(file, &root);
	Json::Value val = root["Model Property"];
	wstring matFile, meshFile, shaderFile;
	D3DXMATRIX matRoot;
	int animType = -1;
	int clipCount = 0;
	TRANSFORM rootTransform;

	//1. model - matFile, meshFile
	{
		Json::GetValue(val, "matFile", matFile);
		Json::GetValue(val, "meshFile", meshFile);
		Json::GetValue(val, "shaderFile", shaderFile);
	}
	//2. model - rootAxis
	{
		Json::GetValue(val, "RootTransformS", rootTransform.Scale);
		Json::GetValue(val, "RootTransformR", rootTransform.RotationDeg);
		Json::GetValue(val, "RootTransformT", rootTransform.Position);
		matRoot = rootTransform.GetMatrix();
	}
	//3. animation - type
	{
		Json::GetValue(val, "AnimType", animType);
	}

	wstring matmeshfile = Path::GetDirectoryName(matFile) + Path::GetFileNameWithoutExtension(matFile);

	GameModel* gmodel = NULL;
	switch (readClass)
	{
		case GModelReadClass::GameModel:
			gmodel = new GameModel(matmeshfile, (ANIMATION_TYPE)animType);
		break;
		case GModelReadClass::GamePlayer:
			gmodel = new GamePlayer(matmeshfile, (ANIMATION_TYPE)animType);
		break;
		case GModelReadClass::GameEnemyRobot:
			gmodel = new GameEnemyRobot(matmeshfile);
		break;
		case GModelReadClass::GameEnemyBoss:
		break;
	}

	assert(gmodel != NULL);

	gmodel->SetShader(shaderFile);
	gmodel->SetRootAxis(matRoot);

	//4. animation - clips
	{
		Json::GetValue(val, "AnimClipCount", clipCount);

		wstring name;
		bool bLoop = false;
		for (int i = 0; i < clipCount; i++)
		{
			Json::GetValue(val, String::SInt("AnimClip%d", i), name);
			Json::GetValue(val, String::SInt("AnimClipLoop%d", i), bLoop);

			gmodel->AddClip(name, bLoop);
		}
	}

	//5. Boundings
	{
		UINT count = 0;
		Json::GetValue(val, "BoundingCount", count);


		for (UINT i = 0; i < count; i++)
		{
			Json::Value valBounding = val[String::SInt("Bounding%d", i).c_str()];

			int type, prop;
			string name;
			int socketNum;
			D3DXCOLOR color;

			Json::GetValue(valBounding, String::SInt("Type", i), type);
			Json::GetValue(valBounding, String::SInt("Prop", i), prop);
			Json::GetValue(valBounding, String::SInt("Name", i), name);
			Json::GetValue(valBounding, String::SInt("SocketNum", i), socketNum);
			Json::GetValue(valBounding, String::SInt("Color", i), color);

			TRANSFORM tr;
			Json::GetValue(valBounding, String::SInt("TR-RotationDeg", i), tr.RotationDeg);
			Json::GetValue(valBounding, String::SInt("TR-Position", i), tr.Position);


			switch ((BoundingType)type)
			{
			case BoundingType::Box:
			{
				D3DXVECTOR3 Min, Max;
				Json::GetValue(valBounding, String::SInt("Box-Min", i), Min);
				Json::GetValue(valBounding, String::SInt("Box-Max", i), Max);

				BoundingBox* box = new BoundingBox(gmodel, Min, Max);
				box->SetBoundingProp((BoundingProp)prop);
				box->SetName(name);
				box->SetSocketnum(socketNum);
				box->SetColor(color);
				box->SetTransform(tr);

				gmodel->boundings.push_back(box);
			}
			break;
			case BoundingType::Sphere:
				break;
			case BoundingType::Capsule:
			{
				D3DXVECTOR3 center;
				float radius, height;
				Json::GetValue(valBounding, String::SInt("Capsule-Center", i), center);
				Json::GetValue(valBounding, String::SInt("Capsule-Radius", i), radius);
				Json::GetValue(valBounding, String::SInt("Capsule-Height", i), height);

				BoundingCapsule* capsule = new BoundingCapsule(gmodel, center, radius, height);
				capsule->SetBoundingProp((BoundingProp)prop);
				capsule->SetName(name);
				capsule->SetSocketnum(socketNum);
				capsule->SetColor(color);
				capsule->SetTransform(tr);

				gmodel->boundings.push_back(capsule);
			}
			break;
			}
		}
	}

	*model = gmodel;
}
