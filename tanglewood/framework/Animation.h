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

	bool operator == (const AnimType& rhs) const
	{
		return sheetName == rhs.sheetName
			&& animName == rhs.animName
			&& flags == rhs.flags;
	}

	bool operator != (const AnimType& rhs) const
	{
		return !(*this == rhs);
	}
};
