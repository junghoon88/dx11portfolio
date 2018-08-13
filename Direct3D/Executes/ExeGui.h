#pragma once
#include "Execute.h"

class ExeGui : public Execute
{
public:
	ExeGui(ExecuteValues* values);
	~ExeGui();

	// Execute을(를) 통해 상속됨
	virtual void Update(void)		override;
	virtual void PreRender(void)	override {}
	virtual void Render(void)		override;
	virtual void PostRender(void)	override;
	virtual void ResizeScreen(void) override {}

private:
	void ChangeShowGlobalLightWindow(void);
	void ChangeShowEnvironmentWindow(void);
	void ChangeShowLandscapeWindow(void);
	void ChangeShowSystemInfoWindow(void);
	void ChangeShowDemoWindow(void);
	void ChangeShowEditorSettingWindow(void);
	void ChangeShowLightManagerWindow(void);


	void LoadJsonFile(void);
	void SaveJsonFile(void);

};