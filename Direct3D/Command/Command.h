#pragma once

#define gCmdManager CommandManager::Get()

class Command
{
public:
	friend class CommandManager;

protected:
	Command() {}
	virtual ~Command() {}

	virtual void Execute(void) = 0;
	virtual void Undo(void) = 0;
	virtual void Redo(void) = 0;
};


class CommandManager
{
public:
	inline static CommandManager* Get(void) { return instance; }
	inline static void Create(void) { assert(instance == NULL); instance = new CommandManager(); }
	inline static void Delete(void) { SAFE_DELETE(instance); }

public:
	CommandManager();
	~CommandManager();

	void AddCommand(Command* cmd);
	void UndoCommand(void);
	void RedoCommand(void);
	void ClearCommands(void);

	Command* GetUndoCommand(void);
	Command* GetRedoCommand(void);
	
private:
	static CommandManager* instance;

	vector<Command*> oldCommands;
	UINT maxCommand;
	int curCommand;
};

