#pragma once

class GameModel;
class Grid3Axis;

class EditModel : public Execute
{
private:
	enum class EXPORT_MODE
	{
		EXPORT_MODE_MATMESH, EXPORT_MODE_ANIMATIONS
	};

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

	void OpenExportDialog(EXPORT_MODE mode, wstring file = L"");
	void ExportMatMesh(wstring fbxFile, wstring saveFile);
	void ExportAnimations(wstring fbxPath);
	void OpenMatMeshDialog(wstring file = L"");
	void OpenAnimationDialog(wstring file = L"");
	void OpenGameModelDialog(wstring file = L"");
	void SaveGameModelDialog(wstring file = L"");


	void ShowTreeNode(void);
	void ShowTransform(void);
	void ShowAnimationInfo(void);
	void ShowCapsuleInfo(void);


private:
	wstring fileName;

	GameModel* model;
	Grid3Axis* grid;

	TRANSFORM transform;
};