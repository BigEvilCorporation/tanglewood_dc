///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		PhysicsObj.h
// Date:		14th January 2017
// Authors:		Matt Phillips
// Description:	Global constants
//				(all values from Mega Drive version)
///////////////////////////////////////////////////////////////

#include <ion/maths/Vector.h>

namespace Constants
{
	namespace MegaDrive
	{
		static const float frameRate = 60.0f;
		static const float subPixelsPerPixel = 256.0f;
	}

	namespace World
	{
		static const float defaultGravity = 0x0030 * MegaDrive::frameRate;
	}

	namespace Player
	{
		//Acceleration/deceleration
		static const ion::Vector2 defaultPlayerAcceleration(0x0014 * MegaDrive::frameRate, 0.0f);
		static const ion::Vector2 defaultPlayerDecelerationIdle(0x0016 * MegaDrive::frameRate, 0.0f);
		static const ion::Vector2 defaultPlayerDecelerationForced(0x0030 * MegaDrive::frameRate, 0.0f);

		//Max velocities
		static const float defaultPlayerMaxVelocityX = 0x0380 / MegaDrive::subPixelsPerPixel * MegaDrive::frameRate;
		static const float defaultPlayerMaxVelocityYUp = 0x0600 / MegaDrive::subPixelsPerPixel * MegaDrive::frameRate;
		static const float defaultPlayerMaxVelocityYDown = 0x0800 / MegaDrive::subPixelsPerPixel * MegaDrive::frameRate;

		//Jumping/falling
		static const float defaultPlayerJumpImpuse = 0x0600 / MegaDrive::subPixelsPerPixel * MegaDrive::frameRate;
		static const float defaultFallVelocity = 0x0200 / MegaDrive::subPixelsPerPixel * MegaDrive::frameRate;

		//Physics
		static const float defaultStepHeight = 0x03;
	}
}