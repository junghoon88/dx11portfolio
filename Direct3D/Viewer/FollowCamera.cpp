#include "stdafx.h"
#include "FollowCamera.h"
#include "../Objects/GameModel.h"

FollowCamera::FollowCamera(GameModel* model, float distance)
	: model(model), distance(distance)
{
}

FollowCamera::~FollowCamera()
{

}

void FollowCamera::Update()
{
	//Translation
	if (GetIsMovable())
	{
		D3DXVECTOR3 pos, dir;
		model->GetPosition(pos);
		model->GetDirection(dir);
		dir.y = 0.0f;
		D3DXVec3Normalize(&dir, &dir);

		//¸¶¿ì½º ÈÙ
		float mouseWheel = gMouse->GetMoveValue().z;
		if (mouseWheel > 0.0f)
		{
			distance -= 10.0f;
			distance = Math::Max(40.0f, distance);
		}
		else if (mouseWheel < 0.0f)
		{
			distance += 10.0f;
			distance = Math::Min(80.0f, distance);
		}

		float angleX = 10.0f;
		float offset = 20.0f;

		pos.x -= dir.x * distance;
		pos.z -= dir.z * distance;
		pos.y += sinf(Math::ToRadian(angleX)) * distance + offset;

		float angleY = atan2(dir.z, dir.x) - Math::ToRadian(90.0f);
		Camera::SetRotation(Math::ToRadian(angleX), -angleY);
		Camera::SetPosition(pos);
	}

	//Camera::UpdateRotation();
	//Camera::UpdateView();

}
