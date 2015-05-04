#pragma once

#include "RoboState.h"

namespace dreadbot
{
	class StrafeLeft : public RoboState
	{
	public:
		StrafeLeft();
		int update();
		void enter();
	};
}
