#pragma once
#include "stdafx.h"
#include "Command.h"

class CommandTerrainUpDown : public Command
{
public:
	CommandTerrainUpDown(function<void(VertexTypePTNC2*)> func, VertexTypePTNC2* vertexDataOld, VertexTypePTNC2* vertexDataNew)
		: func(func), vertexDataOld(vertexDataOld), vertexDataNew(vertexDataNew)
	{
	}
	~CommandTerrainUpDown()
	{
		SAFE_DELETE_ARRAY(vertexDataOld);
		SAFE_DELETE_ARRAY(vertexDataNew);
	}

	// Command을(를) 통해 상속됨
	virtual void Execute(void) override
	{
		func(vertexDataNew);
	}

	virtual void Undo(void) override
	{
		func(vertexDataOld);
	}

	virtual void Redo(void) override
	{
		Execute();
	}


private:
	function<void(VertexTypePTNC2*)> func;
	VertexTypePTNC2* vertexDataOld;
	VertexTypePTNC2* vertexDataNew;

};
