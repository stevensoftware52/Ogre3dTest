#ifndef __Util_h_
#define __Util_h_

#include "stdafx.h"

namespace Util
{
	static bool hasStringAbbr(const char* name, const char* part)
	{
		// non "" command
		if (*name)
		{
			// "" part from non-"" command
			if (!*part)
				return false;

			for (;;)
			{
				if (!*part)
					return true;
				else if (!*name)
					return false;
				else if (tolower(*name) != tolower(*part))
					return false;
				++name; ++part;
			}
		}
		// allow with any for ""

		return true;
	}

	static Ogre::ColourValue vectorToColor(Ogre::Vector3 vec)
	{
		return Ogre::ColourValue(vec.x, vec.y, vec.z);
	}
}

#endif