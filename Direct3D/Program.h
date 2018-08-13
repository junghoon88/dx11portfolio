#pragma once

class Execute;
struct ExecuteValues;

class Program
{
public:
	Program();
	~Program();

	void Update(void);

	void PreRender(void);
	void Render(void);
	void PostRender(void);

	void ResizeScreen(void);

private:
	void InitializeValues(void);
	void InitializeExecutes(void);

	void UpdateCommand(void);

	void SetGlobalBuffers(void);

private:
	void LoadJsonFile(void);
	void SaveJsonFile(void);

private:
	ExecuteValues* values;
	vector<Execute*> executes;

};