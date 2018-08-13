#pragma once

class GameSettings;
class ModelShape;

class DrawModel : public Execute
{
public:
	DrawModel(ExecuteValues* values);
	~DrawModel();

	void Update(void);

	void PreRender(void);
	void Render(void);
	void PostRender(void);

	void ResizeScreen(void) {}

private:
	void PostRenderModelMenu(void);
	void OpenExportDialog(wstring file = L"");
	void Export(wstring fbxFile, wstring saveFile);
	void OpenModelDialog(wstring file);


private:
	GameSettings* settings;
	ModelShape* modelShape;

};