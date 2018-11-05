///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		DjakkReveal.h
// Date:		4th November 2018
// Authors:		Matt Phillips
// Description:	Djakk reveal cutscene
//
// 1. Camera lerp to Djakk
// 2. Roar
// 3. Camera lerp back
// 4. Begin chase
//
///////////////////////////////////////////////////////////////

#include "framework/Cutscene.h"
#include "cutscenes/Common.h"
#include "tanglewood/Player.h"
#include "tanglewood/Djakk.h"

#include "Animations.h"
#include "Globals.h"

namespace Cutscenes
{
	class DjakkReveal : public Cutscene
	{
	public:
		DjakkReveal(Player& player, Djakk& djakk)
			: m_player(player)
			, m_djakk(djakk)
		{

		}

		virtual void Setup()
		{
			Player& player = m_player;
			Djakk& djakk = m_djakk;

			//Stop movement and restrict controls
			AddStep(new Cutscene::Step("StopMove",
				[&player]()
				{
					player.m_acceleration.x = 0.0f;
					player.m_velocity.x = 0.0f;
					player.m_controlEnabled = false;
				},
				[](float dt) -> bool { return false; } ));

			//Lerp camera to Djakk (blocking)
			AddStep(new Common::CameraLerp(*Globals::Game::camera, m_djakk, 1.0f, true));

			//Wait
			AddStep(new Common::Delay(0.5f));

			//Roar and begin chase
			AddStep(new Cutscene::Step("Roar",
				[&djakk]()
				{
					djakk.BeginChase(true);
				},
				[&djakk](float dt) -> bool { return djakk.GetCurrentAnimType() && (*djakk.GetCurrentAnimType() == Animations::Djakk::roar); }));

			//Lerp camera to player (non-blocking)
			AddStep(new Common::CameraLerp(*Globals::Game::camera, m_player, 1.0f, false));

			//Restore controls
			AddStep(new Cutscene::Step("RestoreMove",
				[&player]()
				{
					player.m_controlEnabled = true;
				},
				[](float dt) -> bool { return false; }));
		}

	private:
		Player& m_player;
		Djakk& m_djakk;
	};
}