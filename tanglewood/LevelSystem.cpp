#include "LevelSystem.h"

#include "Constants.h"
#include "Globals.h"
#include "levels/LevelList.h"

namespace LevelSystem
{
	void AdvanceLevel()
	{
		if (Globals::Game::level)
		{
			//Delete current
			delete Globals::Game::level;

			//Advance to next
			Globals::Game::levelIdx++;
		}

		//Create next level
		Globals::Game::level = Constants::levels[Globals::Game::levelIdx].levelfactory();
	}
}