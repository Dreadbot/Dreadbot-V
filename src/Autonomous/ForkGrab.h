#pragma once

#include "RoboState.h"

namespace dreadbot
{
	class ForkGrab : public RoboState
	{
	public:
		ForkGrab();
		virtual void enter();
		virtual int update();
		Timer grabTimer;
	protected:
		bool timerActive;
	};
}
