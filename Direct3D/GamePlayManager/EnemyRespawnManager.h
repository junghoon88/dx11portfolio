#pragma once

struct EnemyArea
{
	string		Name  = "";
	D3DXVECTOR2 Start = D3DXVECTOR2(0, 0);
	D3DXVECTOR2 Size  = D3DXVECTOR2(0, 0);
	D3DXCOLOR   Color = D3DXCOLOR(1, 1, 1, 1);
	bool		Show  = true;
};


enum class EnemyRespawnPhase
{
	Ready,
	Phase1,
	Phase2, 
	Boss
};

class DrawStage;

class EnemyRespawnManager
{
public:
	EnemyRespawnManager(DrawStage* stage);
	~EnemyRespawnManager();

	void Update(void);
	void AddArea(EnemyArea area);

private:
	bool FindArea(string name, OUT EnemyArea* area);
	void RespawnRobot(EnemyArea& area, UINT num);

public:

	inline void SetBattleStart(bool b) { battleStart = b; }
	inline void SetBattleEnd(bool b) { battleEnd = b; }
	inline bool GetBattleStart(void) { return battleStart; }
	inline bool GetBattleEnd(void) { return battleEnd; }
	inline bool IsBattle(void) { return (battleStart && !battleEnd); }


private:
	DrawStage*			stage;
	EnemyRespawnPhase	battlePhase;
	bool				battleStart;
	bool				battleEnd;
	vector<EnemyArea>	areas;
};

