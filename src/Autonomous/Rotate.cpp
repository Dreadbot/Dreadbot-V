#include "Rotate.h"

namespace Dreadbot
{
	Rotate::Rotate()
	{
		timerActive = false;
		rotateConstant = 1; //Changes the direction that the robot turns.
	}
	void Rotate::enter()
	{
		driveTimer.Reset();
		driveTimer.Start();
		timerActive = true;
		sysLog->log("State: Rotate");
	}
	int Rotate::update()
	{
		if (driveTimer.Get() >= ROTATE_TIME)
		{ //Rotated far enough; break
			timerActive = false;
			drivebase->Drive_v(0, 0, 0);
			if (HALBot::getToteCount() == 3)
				lift->Set(-1); //Lower lift
			return HALBot::timerExpired;
		}
		if (drivebase != nullptr)
			drivebase->Drive_v(0, 0, 0.5 * rotateConstant);
		return HALBot::no_update;
	}
}
