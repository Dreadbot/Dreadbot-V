#include "BackAway.h"

namespace dreadbot
{
	void BackAway::enter()
	{
		sysLog->log("State: BackAway");
		drivebase->Drive_v(0, 0, 0);
		timerActive = false; //Cheat way of figuring out if the lift is down. Used elsewhere
	}
	int BackAway::update()
	{
		//While the lift isn't down
		if (!timerActive)
		{
			lift->Set(-1);

			//On first lift down
			if (isLiftDown())
			{
				timerActive = true;
				liftArms->Set(-1);
				Wait(0.13); //Uber cheap way of getting the totes to disengage
				grabTimer.Reset();
				grabTimer.Start();
			}
			return HALBot::no_update;
		}

		if (grabTimer.Get() >= BACK_AWAY_TIME)
		{
			drivebase->Drive_v(0, 0, 0);
			return HALBot::timerExpired;
		}

		if (drivebase != nullptr) 
		{
			drivebase->GoFast();
			drivebase->Drive_v(0, -1, 0);
		}
		//liftArms->Set(-1);
		return HALBot::no_update;
	}
}
