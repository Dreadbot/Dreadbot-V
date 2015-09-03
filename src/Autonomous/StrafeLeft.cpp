#include "StrafeLeft.h"

namespace dreadbot
{
	StrafeLeft::StrafeLeft()
	{
		timerActive = false;
	}
	void StrafeLeft::enter()
	{
		sysLog->log("State: StrafeLeft");
		driveTimer.Start();
		timerActive = true;
		drivebase->GoFast(); //Gotta go faaaaaaaasssst.
		drive(-1, 0, 0); //Left
	}
	int StrafeLeft::update()
	{
		if (driveTimer.Get() >= 0.5f) 
		{
			driveTimer.Stop();
			driveTimer.Reset();
			timerActive = false;
			drive(0, 0, 0); //stop
			return RoboState::timerExpired;
		}
		return RoboState::no_update;
	}
}
