#include "BackAway.h"

namespace dreadbot
{
	void BackAway::enter()
	{
		sysLog->log("State: BackAway");
		drive(0, 0, 0);
		timerActive = false; //Cheat way of figuring out if the lift is down. Used elsewhere
	}
	int BackAway::update()
	{
		//While the lift isn't down
		if (!timerActive)
		{
			lowerLift();

			//On first lift down
			if (isLiftDown())
			{
				timerActive = true;
				liftArmsOut();
				Wait(0.13); //Uber cheap way of getting the totes to disengage
				grabTimer.Reset();
				grabTimer.Start();
			}
			return RoboState::no_update;
		}

		if (grabTimer.Get() >= BACK_AWAY_TIME)
		{
			drive(0, 0, 0);
			return RoboState::timerExpired;
		}

		if (drivebase != nullptr) 
		{
			drivebase->GoFast();
			drive(0, -1, 0);
		}
		//liftArms->Set(-1);
		return RoboState::no_update;
	}
}
