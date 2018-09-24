///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		PhysicsObj.h
// Date:		14th January 2017
// Authors:		Matt Phillips
// Description:	Global constants
//				(all values from Mega Drive version)
///////////////////////////////////////////////////////////////

#include <ion/maths/Maths.h>
#include <ion/maths/Vector.h>

#define SUBPIXELS_TO_PIXELS(val) (float)((float)(val>>16)+((float)(val&0xFFFF)/Constants::MegaDrive::subPixelsPerPixel))
#define SUBPIXELS_TO_PIXELS_PER_SEC(val) SUBPIXELS_TO_PIXELS(val) * Constants::MegaDrive::frameRate
#define SUBPIXELS_TO_ACCELERATION(val) SUBPIXELS_TO_PIXELS(val) * ion::maths::Square(Constants::MegaDrive::frameRate)

namespace Constants
{
	namespace MegaDrive
	{
		static const float frameRate = 60.0f;
		static const float subPixelsPerPixel = 65536.0f;
		static const float subFramesPerFrame = 256.0f;

		static const float tileWidth = 8.0f;
		static const float tileHeight = 8.0f;
	}

	namespace World
	{
		static const float defaultGravity = SUBPIXELS_TO_ACCELERATION(0x3000);
		static const float floorSearchDist = MegaDrive::tileHeight * 2;
	}

	namespace Player
	{
		//Acceleration/deceleration
		static const ion::Vector2 defaultPlayerAcceleration(SUBPIXELS_TO_ACCELERATION(0x0600), 0.0f);
		static const ion::Vector2 defaultPlayerDecelerationIdle(SUBPIXELS_TO_ACCELERATION(0x1600), 0.0f);
		static const ion::Vector2 defaultPlayerDecelerationForced(SUBPIXELS_TO_ACCELERATION(0x3000), 0.0f);

		//Max velocities
		static const float defaultPlayerMaxVelocityX = SUBPIXELS_TO_PIXELS_PER_SEC(0x040000);
		static const float defaultPlayerMaxVelocityYUp = SUBPIXELS_TO_PIXELS_PER_SEC(0x100000);
		static const float defaultPlayerMaxVelocityYDown = SUBPIXELS_TO_PIXELS_PER_SEC(0x080000);

		//Jumping/falling
		static const float defaultPlayerJumpImpulse = SUBPIXELS_TO_PIXELS_PER_SEC(0x060000);
		static const float defaultFallVelocity = SUBPIXELS_TO_PIXELS_PER_SEC(0x020000);

		//Physics
		static const float defaultStepHeight = 0x04;
	}
}
