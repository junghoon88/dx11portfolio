#include "stdafx.h"
#include "Free.h"

Free::Free(float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
{

}

Free::~Free()
{

}

void Free::Update()
{
	D3DXVECTOR3 forward, right, up;
	Camera::GetForward(&forward);
	Camera::GetRight(&right);
	Camera::GetUp(&up);

	//Translation
	if (GetIsMovable())
	{
		D3DXVECTOR3 position;
		Camera::GetPosition(&position);

		if (gMouse->IsPress(MOUSE_RBUTTON))
		{
			if (gKeyboard->IsPress('W'))
				position += forward * moveSpeed * Time::Delta();
			else if (gKeyboard->IsPress('S'))
				position -= forward * moveSpeed * Time::Delta();

			if (gKeyboard->IsPress('A'))
				position -= right * moveSpeed * Time::Delta();
			else if (gKeyboard->IsPress('D'))
				position += right * moveSpeed * Time::Delta();

			if (gKeyboard->IsPress('E'))
				position += up * moveSpeed * Time::Delta();
			else if (gKeyboard->IsPress('Q'))
				position -= up * moveSpeed * Time::Delta();

			//¸¶¿ì½º ÈÙ
			position += forward * moveSpeed * Time::Delta() * gMouse->GetMoveValue().z * 0.3f;
		}

		Camera::SetPosition(position.x, position.y, position.z);
	}

	//Rotation
	if (GetIsMovable())
	{
		D3DXVECTOR2 rotation;
		Camera::GetRotation(&rotation);

		if (gMouse->IsPress(MOUSE_RBUTTON))
		{
			D3DXVECTOR3 move = gMouse->GetMoveValue();

			rotation.x += move.y * rotationSpeed * Time::Delta();
			rotation.y += move.x * rotationSpeed * Time::Delta();
		}

		Camera::SetRotation(rotation.x, rotation.y);
	}
}
