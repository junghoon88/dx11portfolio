#pragma once

class Camera
{
protected:
	Camera();
public:
	virtual ~Camera() {}

public:
	virtual void Update(void) = 0;

protected:
	virtual void UpdateMove(void);
	virtual void UpdateRotation(void);
	virtual void UpdateView(void);

public:
	D3DXVECTOR3 GetDirection(class Viewport* vp, class Perspective* perspective);
	void Shake(float magnitude, float duration);


public:
	inline void SetIsMovable(bool movable) { isMovable = movable; }
	inline bool GetIsMovable(void) { return isMovable; }

	inline void GetPosition(D3DXVECTOR3* pos)			{ *pos = position; }
	inline void GetRotation(D3DXVECTOR2* rot)			{ *rot = rotation; }
	inline void GetRotationDegree(D3DXVECTOR2* rot)		{ *rot = rotation * 180.0f / (float)D3DX_PI; }

	inline void SetPosition(float x, float y, float z)	{ position = D3DXVECTOR3(x, y, z); UpdateView(); }
	inline void SetPosition(D3DXVECTOR3 pos) { position = pos; UpdateView(); }
	inline void SetRotation(float x, float y)			{ rotation = D3DXVECTOR2(x, y); UpdateRotation(); }
	inline void SetRotationDegree(float x, float y)		{ rotation = D3DXVECTOR2(x, y) * (float)D3DX_PI / 180.0f; UpdateRotation(); }

	inline void GetMatrix(D3DXMATRIX* view)				{ *view = matView; }
	inline void GetForward(D3DXVECTOR3* vec)			{ *vec  = forward; }
	inline void GetRight(D3DXVECTOR3* vec)				{ *vec  = right; }
	inline void GetUp(D3DXVECTOR3* vec)					{ *vec  = up; }


private:
	bool isMovable;

	D3DXVECTOR3 position;
	D3DXVECTOR2 rotation;

	D3DXVECTOR3 forward; //¿¸πÊ∫§≈Õ
	D3DXVECTOR3 right;	 //øÏ√¯∫§≈Õ
	D3DXVECTOR3 up;		 //ªÛπÊ∫§≈Õ

	D3DXMATRIX matRotation;
	D3DXMATRIX matView;

	//Shake
	// Are we shaking?
	bool shaking;
	// The maximum magnitude of our shake offset
	float shakeMagnitude;
	// The total duration of the current shake
	float shakeDuration;
	// A timer that determines how far into our shake we are
	float shakeTimer;
	// The shake offset vector
	D3DXVECTOR3 shakeOffset;

};