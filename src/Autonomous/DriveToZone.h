#pragma once

#include "RoboState.h"

namespace dreadbot
{
	class DriveToZone : public RoboState
	{
	public:
		DriveToZone();
		virtual void enter();
		virtual int update();
		Timer driveTimer;
		int dir;
		bool strafe;
	protected:
		bool timerActive;
	};
}
