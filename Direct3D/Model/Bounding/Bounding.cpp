#pragma once
#include "stdafx.h"
#include "Bounding.h"

Bounding::Bounding(GameModel * mymodel)
	: mymodel(mymodel)
	, socketnum(-1)
	, color(1, 1, 0, 1)
	, prop(BoundingProp::Unknown)
	, bShow(false)
{
	D3DXMatrixIdentity(&socketTransform);
	D3DXMatrixIdentity(&myWorld);
}

void Bounding::Update(void)
{
	D3DXMATRIX W = transform.GetMatrix();

	D3DXMatrixIdentity(&socketTransform);

	D3DXMATRIX modelRoot, modelWorld;
	mymodel->GetRootAxis(modelRoot);
	mymodel->GetWorld(modelWorld);

	if (mymodel != NULL && socketnum > -1)
	{
		socketTransform = mymodel->GetModel()->GetAbsoluteBoneTo(socketnum);
	}

	myWorld = W * socketTransform * (modelRoot * modelWorld);
}

void Bounding::PostRender(void)
{
	switch (type)
	{
	case BoundingType::Box:
		ImGui::Text("Type-Box");
		break;
	case BoundingType::Sphere:
		ImGui::Text("Type-Sphere");
		break;
	case BoundingType::Capsule:
		ImGui::Text("Type-Capsule");
		break;
	}

	ImGui::InputInt("socketNum", &socketnum);
	socketnum = Math::Clamp(socketnum, -1, mymodel->GetModel()->GetBoneCount() - 1);

	//ImGui::SliderFloat3("S", transform.Scale, -10, 10);
	ImGui::DragFloat3("R", transform.RotationDeg, 1.0f, -360, 360);
	ImGui::DragFloat3("T", transform.Position, 0.1f, -100, 100);

	char tempName[128];
	strcpy_s(tempName, name.c_str());
	ImGui::InputText("Name", tempName, 128);
	name = string(tempName);

	ImGui::ColorEdit3("Color", &color[0]);

	ImGui::Checkbox("Show", &bShow);


	ImGui::SliderInt("Prop", (int*)&prop, 0, (int)BoundingProp::Count - 1);
	switch (prop)
	{
		case BoundingProp::Unknown:
			ImGui::Text("Prop-Unknown");
		break;
		case BoundingProp::Weapon:
			ImGui::Text("Prop-Weapon");
		break;
		case BoundingProp::Armor:
			ImGui::Text("Prop-Armor");
		break;
		case BoundingProp::FrustumCulling:
			ImGui::Text("Prop-FrustumCulling");
		break;
	}
}
