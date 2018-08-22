#pragma once
#include "Camera.h"

class FixedCamera : public Camera
{
public:
	FixedCamera(float moveSpeed = 20, float rotationSpeed = 2.5f);
	~FixedCamera();

	void Update(void);

private:
	float moveSpeed;
	float rotationSpeed;

};