#pragma once
#include "Camera.h"

class GameModel;

class FollowCamera : public Camera
{
public:
	FollowCamera(GameModel* model, float distance);
	~FollowCamera();

	void Update();

private:
	GameModel* model;
	float distance;
};