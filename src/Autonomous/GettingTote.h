#pragma once

#include "RoboState.h"

namespace dreadbot
{
	class GettingTote : public RoboState
	{
	public:
		GettingTote();
		virtual void enter();
		virtual int update();
 	private:
		bool timerActive;
		Timer getTimer;
		Timer eStopTimer;
	};
}