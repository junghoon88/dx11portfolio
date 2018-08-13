#include "stdafx.h"
#include "Command.h"

CommandManager* CommandManager::instance = NULL;

CommandManager::CommandManager() 
	: maxCommand(100), curCommand(-1)
{

}
CommandManager::~CommandManager()
{
	ClearCommands(); 
}

void CommandManager::AddCommand(Command* cmd)
{
	//curCommand �� �� ���� �ƴҶ�
	//��, Redo �� ������ �����϶� Redo ������ �����.
	while (curCommand < (int)oldCommands.size() - 1)
	{
		//oldCommands.erase(oldCommands.begin() + (int)oldCommands.size() - 1);
		oldCommands.pop_back();
	}

	//������ ������ ���� �տ� �����͸� �����.
	if (oldCommands.size() == maxCommand)
	{
		SAFE_DELETE(oldCommands[0]);
		oldCommands.erase(oldCommands.begin());
	}

	//���� �߰�
	oldCommands.push_back(cmd);

	//������ġ�� ���� �ڷ�
	curCommand = oldCommands.size() - 1;

	cmd->Execute();
}

void CommandManager::UndoCommand(void)
{
	if (oldCommands.size() == 0) return;
	if (curCommand == -1) return;

	oldCommands[curCommand--]->Undo();
}

void CommandManager::RedoCommand(void)
{
	if (oldCommands.size() == 0) return;
	if (curCommand == oldCommands.size() - 1) return;

	oldCommands[++curCommand]->Redo();
}

void CommandManager::ClearCommands(void)
{
	while (oldCommands.size() > 0)
	{
		SAFE_DELETE(oldCommands[0]);
		oldCommands.erase(oldCommands.begin());
	}
	curCommand = -1;
}

Command* CommandManager::GetUndoCommand(void)
{
	if (curCommand > -1)
		return oldCommands[curCommand];

	return NULL;
}
Command* CommandManager::GetRedoCommand(void)
{
	if (curCommand < (int)oldCommands.size() - 1)
		return oldCommands[curCommand + 1];

	return NULL;
}
