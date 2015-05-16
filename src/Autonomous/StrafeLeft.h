#pragma once

#include "DriveToZone.h"

namespace dreadbot
{
	class StrafeLeft : public DriveToZone
	{
	public:
		StrafeLeft();
		int update();
		void enter();
	};
}
