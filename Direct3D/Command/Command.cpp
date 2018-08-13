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
	//curCommand 가 맨 끝이 아닐때
	//즉, Redo 가 가능한 상태일때 Redo 영역을 지운다.
	while (curCommand < (int)oldCommands.size() - 1)
	{
		//oldCommands.erase(oldCommands.begin() + (int)oldCommands.size() - 1);
		oldCommands.pop_back();
	}

	//스택이 꽉차면 제일 앞에 데이터를 지운다.
	if (oldCommands.size() == maxCommand)
	{
		SAFE_DELETE(oldCommands[0]);
		oldCommands.erase(oldCommands.begin());
	}

	//스택 추가
	oldCommands.push_back(cmd);

	//현재위치를 제일 뒤로
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
