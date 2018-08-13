#pragma once

#include "../Model/Bounding/ModelShape.h"

class GameRender
{
public:
	GameRender();
	virtual ~GameRender();

public:
	virtual void Update(ModelShape* modelShape) = 0;
	virtual void Render(void) = 0;
	virtual void PostRender(void) {}

public:
	void SetEnable(bool val);
	bool GetEanble(void);
	void SetVisible(bool val);
	bool GetVisible(void);

	void GetRootAxis(D3DXMATRIX& mat); 
	void SetRootAxis(D3DXMATRIX& mat); 
	void GetWorld(D3DXMATRIX& mat);
	void SetWorld(D3DXMATRIX& mat);

	void GetPosition(D3DXVECTOR3& vec);
	void SetPosition(D3DXVECTOR3& vec);
	void GetDirection(D3DXVECTOR3& vec);
	void SetDirection(D3DXVECTOR3& vec);
	void GetUp(D3DXVECTOR3& vec);
	void SetUp(D3DXVECTOR3& vec);
	void GetRight(D3DXVECTOR3& vec);
	void SetRight(D3DXVECTOR3& vec);

	D3DXMATRIX Transformed(void);


public:
	vector<function<void()>> Enableds;
	vector<function<void()>> Visibled;


protected:
	bool enable;
	bool visible;

	D3DXMATRIX rootAxis;

private:
	D3DXMATRIX world;

	D3DXVECTOR3 position;
	D3DXVECTOR3 direction;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;
};
