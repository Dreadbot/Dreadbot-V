#include "DriveToZone.h"

namespace dreadbot
{
	DriveToZone::DriveToZone() 
	{
		timerActive = false;
		strafe = false;
		dir = 1; //Multiplier that changes the direction the robot moves.
	}
	void DriveToZone::enter() 
	{
		driveTimer.Reset();
		driveTimer.Start();
		timerActive = true;
		raiseLift(); //Raise the lift for tote transit - it's more stable that way.
		sysLog->log("State: DriveToZone");
	}
	int DriveToZone::update()
	{
		float drvZoneTime = DRIVE_TO_ZONE_TIME;
		if (RoboState::toteCount == 0)
			drvZoneTime += 0.4f; //The robot starts farther back when no tote is collected. This adds on additional time to make sure the robot still drives far enough.

		//Break once the robot has moved far enough - timing based.
		if ((driveTimer.Get() >= drvZoneTime && !strafe) || (driveTimer.Get() >= STRAFE_TO_ZONE_TIME && strafe))
		{
			driveTimer.Stop();
			driveTimer.Reset();
			timerActive = false;
			drive(0, 0, 0);
			return RoboState::timerExpired;
		}

		//Apply actual velocity changes
		if (drivebase != nullptr)
		{
			if (strafe)
				drive(1, 0, 0); //Right
			else
				drive(0, 0.8 * dir, 0); // Do a short dance
		}

		return RoboState::no_update;
	}
}
