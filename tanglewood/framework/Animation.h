#pragma once

namespace AnimFlags
{
	enum _AnimFlags
	{
		Loop =				1 << 0,
		FreezeMovementX =	1 << 1,
		FreezeMovementY =	1 << 2,
	};
}

struct AnimType
{
	std::string sheetName;
	std::string animName;
	int flags;
};
