#pragma once

class GModelReadWriter
{
public:
	enum class GModelReadClass
	{
		GameModel, GamePlayer, GameEnemyRobot, GameEnemyBoss,
	};
	static void OpenGModel(wstring file, GModelReadClass readClass, void** model);
};
