///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		PhysicsObj.h
// Date:		14th January 2017
// Authors:		Matt Phillips
// Description:	Global constants
//				(all values from Mega Drive version)
///////////////////////////////////////////////////////////////

#pragma once

#include <ion/maths/Maths.h>
#include <ion/maths/Vector.h>

#include "framework/PlanePriority.h"

#if defined ION_PLATFORM_WINDOWS
#define USE_PALETTE_TEXTURES 1
#else
#define USE_PALETTE_TEXTURES 0
#endif

#define SUBPIXELS_TO_PIXELS(val) (float)((float)(val>>16)+((float)(val&0xFFFF)/Constants::MegaDrive::subPixelsPerPixel))
#define SUBPIXELS_TO_PIXELS_PER_SEC(val) SUBPIXELS_TO_PIXELS(val) * Constants::MegaDrive::frameRate
#define SUBPIXELS_TO_ACCELERATION(val) SUBPIXELS_TO_PIXELS(val) * ion::maths::Square(Constants::MegaDrive::frameRate)

#define FRAMES_TO_SECONDS(val) val / Constants::MegaDrive::frameRate
#define SUBFRAMES_TO_SECONDS(val) (val / Constants::MegaDrive::frameRate) * Constants::MegaDrive::subFramesPerFrame

namespace Constants
{
	namespace MegaDrive
	{
		static const float frameRate = 60.0f;
		static const float subPixelsPerPixel = 65536.0f;
		static const float subFramesPerFrame = 256.0f;

		static const float tileWidth = 8.0f;
		static const float tileHeight = 8.0f;

		static const float screenWidth = 320.0f;
		static const float screenHeight = 240.0f;
	}

	namespace Rendering
	{
		static const float planePriorities[(int)PlanePriority::Count] =
		{
			-0.7f, // Background
			-0.6f, // PlaneBLow
			-0.5f, // PlaneALow
			-0.4f, // SpriteLow
			-0.3f, // PlaneBHigh
			-0.2f, // PlaneAHigh
			-0.1f, // SpriteHigh
			 0.0f  // Debug
		};
	}

	namespace World
	{
		static const float defaultGravity = SUBPIXELS_TO_ACCELERATION(0x3000);
		static const float floorSearchDist = MegaDrive::tileHeight * 2;
		static const float wallSearchDist = MegaDrive::tileWidth * 2;
		static const float paletteLerpSpeed = 0.5f;
	}

	namespace Flow
	{
		static const float defaultFadeSpeed = 1.0f;
		static const float endChapterFadeTimer = 6.0f;
	}

	namespace Boulder
	{
		static const float platformYOffset = 0x0008;
		static const float minDamageYVel = SUBPIXELS_TO_PIXELS_PER_SEC(0x050000);
		static const float respawnDistance = MegaDrive::screenWidth;
	}

	namespace Character
	{
		static const ion::Vector2 accelerationWalking(SUBPIXELS_TO_ACCELERATION(0x0600), 0.0f);
		static const ion::Vector2 accelerationRunning(SUBPIXELS_TO_ACCELERATION(0x1400), 0.0f);
		static const ion::Vector2 decelerationIdle(SUBPIXELS_TO_ACCELERATION(0x1600), 0.0f);
		static const ion::Vector2 decelerationForced(SUBPIXELS_TO_ACCELERATION(0x3000), 0.0f);

		static const float maxVelocityXWalking = SUBPIXELS_TO_PIXELS_PER_SEC(0x010000);
		static const float maxVelocityXRunning = SUBPIXELS_TO_PIXELS_PER_SEC(0x040000);
		static const float maxVelocityXInAir = SUBPIXELS_TO_PIXELS_PER_SEC(0x040000);
		static const float maxVelocityYUp = SUBPIXELS_TO_PIXELS_PER_SEC(0x100000);
		static const float maxVelocityYDown = SUBPIXELS_TO_PIXELS_PER_SEC(0x080000);

		static const float walkToRunVelocity = SUBPIXELS_TO_PIXELS_PER_SEC(0x020000);

		static const float jumpImpulse = SUBPIXELS_TO_PIXELS_PER_SEC(0x060000);
		static const float jumpCancelVelMin = SUBPIXELS_TO_PIXELS_PER_SEC(0x010000);
		static const float jumpCancelVelMax = maxVelocityYUp;

		static const float fallVelocity = SUBPIXELS_TO_PIXELS_PER_SEC(0x020000);

		static const float stepHeight = 0x04;
	}

	namespace Djakk
	{
		static const ion::Vector2 accelerationWalking(SUBPIXELS_TO_ACCELERATION(0x0200), 0.0f);
		static const ion::Vector2 accelerationRunning(SUBPIXELS_TO_ACCELERATION(0x1600), 0.0f);
		static const ion::Vector2 decelerationIdle(SUBPIXELS_TO_ACCELERATION(0x2000), 0.0f);
		static const ion::Vector2 decelerationForced(SUBPIXELS_TO_ACCELERATION(0x4000), 0.0f);

		static const float maxVelocityXWalking = SUBPIXELS_TO_PIXELS_PER_SEC(0x010000);
		static const float maxVelocityXRunning = SUBPIXELS_TO_PIXELS_PER_SEC(0x042000);
		static const float maxVelocityXInAir = SUBPIXELS_TO_PIXELS_PER_SEC(0x042000);

		static const float maxVelocityXRunningTamed = SUBPIXELS_TO_PIXELS_PER_SEC(0x04E000);
		static const float maxVelocityXInAirTamed = SUBPIXELS_TO_PIXELS_PER_SEC(0x0A0000);

		static const float walkToRunVelocity = SUBPIXELS_TO_PIXELS_PER_SEC(0x030000);
		static const float jumpImpulse = SUBPIXELS_TO_PIXELS_PER_SEC(0x038000);
		static const float jumpScaleX = SUBPIXELS_TO_PIXELS(0x026000);

		static const float minWallHeight = 0x11;

		static const float minChaseDistance = 0x0090/2;
		static const float searchRandDistMax = 0x00FF;
		static const float searchRandDelayMin = FRAMES_TO_SECONDS(0x0018);
		static const float searchRandDelayMax = FRAMES_TO_SECONDS(0x0080);
		static const float searchNearTargetDist = 0x4;
		static const int biteAttackFrame = 0x2;

		static const float attackBoundsWidth = 0x0090;
		static const float attackBoundsHeight = (0x0058 - 0x40);
		static const float attackBoundsOffsetX = (0x0090 / 2) - (attackBoundsWidth / 2);
		static const float attackBoundsOffsetY = (0x0058 / 2) - (attackBoundsHeight / 2);

		static const ion::Vector2 attackBoundsMin(attackBoundsOffsetX, attackBoundsOffsetY);
		static const ion::Vector2 attackBoundsMax(attackBoundsOffsetX + attackBoundsWidth, attackBoundsOffsetY + attackBoundsHeight);

		static const ion::Vector2 saddleOffset(0x0028, -0x0008);
		static const float buckTime = 2.0f;
	}

	namespace Player
	{
		//Dimensions
		static const float boundsWidth = 0x0010;
		static const float boundsHeight = 0x0030;

		//Collision
		static const float minWallHeight = 0x11;

		//Pushing
		static const float heavyPushObjectHeight = 0x0010;

		//Colour ability
		static const float colourAbilityMaxTime = FRAMES_TO_SECONDS(0x0600);
		static const float paletteLerpSpeed = 1.0f;
		static const float timeSlowPhysicsScale = 0.1f;
		static const float timeSlowLerpSpeed = 3.0f;

		//Gliding
		static const float maxVelocityYDownGlide = SUBPIXELS_TO_PIXELS_PER_SEC(0x010000);

		//Beast taming
		static const float beastTameTimeoutBuckVelocityX = SUBPIXELS_TO_PIXELS_PER_SEC(0x040000);
		static const float beastTameTimeoutBuckVelocityY = SUBPIXELS_TO_PIXELS_PER_SEC(0x010000);
	}

	namespace Flue
	{
		static const float defaultEjectTime = FRAMES_TO_SECONDS(0x0020);
		static const float defaultEjectForce = SUBPIXELS_TO_PIXELS_PER_SEC(0x0E0000);
	}

	namespace Fuzzl
	{
		//Deceleration
		static const ion::Vector2 deceleration(SUBPIXELS_TO_ACCELERATION(0x0200), 0.0f);

		//Max velocities
		static const float maxVelocityX = SUBPIXELS_TO_PIXELS_PER_SEC(0x012000);
		static const float maxVelocityYUp = SUBPIXELS_TO_PIXELS_PER_SEC(0x100000);
		static const float maxVelocityYDown = SUBPIXELS_TO_PIXELS_PER_SEC(0x080000);

		//Physics
		static const float stepHeight = 0x03;

		static const float alertDistance = 0x0050;
		static const float lostDistance = 0x0080;
		static const float startledImpulse = SUBPIXELS_TO_PIXELS_PER_SEC(0x010000);
		static const float animSpeedVelocityMul = 0.2f;
		static const int maxEyeWatchFrames = 8;
		static const int eyeWatchCentreFrame = 8;

		//Bounce
		static const float bounceDelay = FRAMES_TO_SECONDS(0x40);
		static const float bounceImpulse = SUBPIXELS_TO_PIXELS_PER_SEC(0x018000);
		static const float bounceAnimSpeed = SUBFRAMES_TO_SECONDS(0x40);
	}

	namespace Hogg
	{
		//Movement
		static const ion::Vector2 accelerationWalking(SUBPIXELS_TO_ACCELERATION(0x000800), 0.0f);
		static const ion::Vector2 accelerationRunning(SUBPIXELS_TO_ACCELERATION(0x000800), 0.0f);
		static const ion::Vector2 decelerationIdle(SUBPIXELS_TO_ACCELERATION(0x000800), 0.0f);
		static const ion::Vector2 decelerationForced(SUBPIXELS_TO_ACCELERATION(0x001000), 0.0f);

		static const float maxVelocityXWalking = SUBPIXELS_TO_PIXELS_PER_SEC(0x060000);
		static const float maxVelocityXRunning = SUBPIXELS_TO_PIXELS_PER_SEC(0x060000);

		//Chase
		static const float alertDistance = 0x0090;
		static const float sleepDistance = 0x0400;
		static const float minChaseDistance = 0x0010;

		//Stuck in wall
		static const float stuckVelocity = SUBPIXELS_TO_PIXELS_PER_SEC(0x020000);
		static const float stuckTime = FRAMES_TO_SECONDS(0x0100);
		static const float stuckExitImpulse = SUBPIXELS_TO_PIXELS_PER_SEC(0x100000);
	}

	namespace Mushroom
	{
		static const float impulse = SUBPIXELS_TO_PIXELS_PER_SEC(0x0A0000);
		static const float impulseDiag = SUBPIXELS_TO_PIXELS_PER_SEC(0x071100);
	}
}
