///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Platform.h
// Date:		30th September 2018
// Authors:		Matt Phillips
// Description:	Physics barrier
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include <ion/maths/Vector.h>

struct Barrier
{
	float width;
	float height;
	ion::Vector2 position;
};