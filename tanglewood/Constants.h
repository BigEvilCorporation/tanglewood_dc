///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		PhysicsObj.h
// Date:		14th January 2017
// Authors:		Matt Phillips
// Description:	Global constants
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include <ion/maths/Vector.h>

namespace Constants
{
	namespace MegaDrive
	{
		static const float megaDriveFrameRate = 24.0f;
		static const float megaDrivePixelsPerFrame = megaDriveFrameRate;
		static const float megaDriveSubPixelsPerFrame = megaDriveFrameRate / 100.0f;
	}

	namespace World
	{
		static const float defaultGravity = 48.0f * MegaDrive::megaDriveSubPixelsPerFrame;
	}

	namespace Player
	{
		static const ion::Vector2 defaultPlayerMaxVelocity(896.0f * MegaDrive::megaDriveSubPixelsPerFrame, 1536.0f * MegaDrive::megaDriveSubPixelsPerFrame);
		static const ion::Vector2 defaultPlayerAcceleration(20.0f * MegaDrive::megaDriveSubPixelsPerFrame, 0.0f);
		static const ion::Vector2 defaultPlayerDeceleration(22.0f * MegaDrive::megaDriveSubPixelsPerFrame, 0.0f);
	}
}