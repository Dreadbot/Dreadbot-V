#include "RotateDrive.h"

namespace dreadbot
{
	RotateDrive::RotateDrive()
	{
		timerActive = false;
		rotateConstant = 1;
	}
	void RotateDrive::enter()
	{
		sysLog->log("State: RotateDrive");
		drivebase->GoFast(); //Gotta go faaaaaaaasssst.
		driveTimer.Start();
		//liftArms->Set(0);
	}
	int RotateDrive::update()
	{
		if (driveTimer.Get() >= (ROTATE_TIME - 0.5f))
		{ //Rotated far enough; break
			timerActive = false;
			drivebase->GoSpeed(1.0);
			drivebase->Drive_v(0, 1, 0);
			
			// The stack is lowered prior to stopping in order to decelerate properly
			if (RoboState::toteCount == 3)
				lift->Set(-1); //Lower lift

			Wait(ROTATE_DRIVE_STRAIGHT);

			return RoboState::timerExpired;
		}
		if (drivebase != nullptr)
			drivebase->Drive_v(0, RD_DRIVE_SPEED, RD_ROTATE_SPEED);
		return RoboState::no_update;
	}
}
