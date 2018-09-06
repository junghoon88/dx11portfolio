#pragma once

class Camera;
class Perspective;

class FrustumCulling
{
public:
	FrustumCulling(float zFar, Camera* camera, Perspective* perspective);
	~FrustumCulling();

	void Update(void);

	bool ContainPoint(float x, float y, float z);
	bool ContainPoint(D3DXVECTOR3 position);

public:
	inline void SetZFar(float value) { zFar = value; }

private:
	D3DXPLANE planes[6];

	float zFar;

	Camera*		 camera;
	Perspective* perspective;
};
