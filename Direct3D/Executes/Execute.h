#pragma once
#include "./Viewer/Camera.h"
#include "../Viewer/Perspective.h"
#include "../Viewer/Viewport.h"

#include "../Command/Command.h"
#include "../Model/Bounding/BoundingFrustum.h"

enum class ExcuteFlags
{
	//ExcuteDrawModel
	Model		= 0x0001,
	//ExcuteDrawTerrain
	Terrain		= 0x0002,
	Tree		= 0x0004,
	Ocean		= 0x0008,
	Sky			= 0x0010,


	All			= Model | Terrain | Tree | Ocean | Sky
};


struct ExecuteValues
{
	class ViewProjectionBuffer* ViewProjection;
	class Perspective*			Perspective;
	class Viewport*				Viewport;
	class Camera*				MainCamera;
	class FrustumCulling*		FrustumCulling;
	class LightBuffer*			GlobalLight;
	class TimeBuffer*			GlobalTime;
	struct GuiSettings*			GuiSettings;

	Json::Value*				jsonRoot;
	ULONG						editableFlag;
	ULONG						renderFlag;



	ExecuteValues()
	{
		ViewProjection	= NULL;
		Perspective		= NULL;
		Viewport		= NULL;
		MainCamera		= NULL;
		FrustumCulling  = NULL;
		GlobalLight		= NULL;
		GuiSettings		= NULL;

		editableFlag = 0;
		renderFlag = 0;
	}
	
	void GetMainCameraFrustum(BoundingFrustum* CameraFrustum)
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);
		RECT rc = { 0, 0, desc.Width, desc.Height };

		D3DXMATRIX view, projection;
		MainCamera->GetMatrix(&view);
		Perspective->GetMatrix(&projection);

		CameraFrustum->Modify(rc, Viewport, projection, view);
	}

};

class Execute
{
public:
	Execute(ExecuteValues* settings)
	{
		this->values = settings;
	}
	virtual ~Execute() {}

	virtual void Update(void) = 0;
	
	virtual void PreRender(void)  = 0;
	virtual void Render(void)     = 0;
	virtual void PostRender(void) = 0;

	virtual void ResizeScreen(void) = 0;

protected:
	ExecuteValues* values;
};