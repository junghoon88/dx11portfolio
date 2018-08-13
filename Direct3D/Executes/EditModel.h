#pragma once

class ModelShape;
class GameModel;

class EditModel : public Execute
{
public:
	EditModel(ExecuteValues* values);
	~EditModel();

	void Update(void);

	void PreRender(void);
	void Render(void);
	void PostRender(void);

	void ResizeScreen(void) {}

private:
	void PostRenderModelMenu(void);
	void PostRenderModelProp(void);

	void OpenExportDialog(wstring file = L"");
	void Export(wstring fbxFile, wstring saveFile);
	void OpenModelDialog(wstring file = L"");


private:
	ModelShape* modelShape;
	GameModel* model;
};