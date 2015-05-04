#include "ForkGrab.h"

namespace dreadbot
{
	ForkGrab::ForkGrab()
	{
		timerActive = false;
	}
	void ForkGrab::enter()
	{
		sysLog->log("State: ForkGrab");
		grabTimer.Reset();
		grabTimer.Start();
	}
	bool thing = true; //PARKER, NO!
	int ForkGrab::update()
	{
		if (HALBot::getToteCount() >= 2 && grabTimer.Get() >= LIFT_ENGAGEMENT_DELAY) 
		{
			intakeArms->Set(1);
			drivebase->GoFast();
			drivebase->Drive_v(0, RD_DRIVE_SPEED, RD_ROTATE_SPEED);
			if (isLiftDown()) 
			{
				//XMLInput::getInstance()->getPGroup("liftArms")->Set(0);
				lift->Set(1); //Raise lift
				Wait(0.3); //Totes must engage first
				lift->Set(0);
				return HALBot::finish;
			} else {
				return HALBot::no_update;
			}
		} else if (HALBot::getToteCount() >= 2 && grabTimer.Get() >= 0.2 && thing) {
			//XMLInput::getInstance()->getPGroup("liftArms")->Set(-1);
			thing = false;
		}

		if (isLiftDown())
		{
			//XMLInput::getInstance()->getPGroup("liftArms")->Set(0);
			HALBot::incrTote(); //Once the lift is down, it is assumed that the tote is actually collected, i.e. in the fork.

			//special 3-tote auton condition. Really kludgy. Causes the robot to NOT lift before rotating/driving
			if (HALBot::getToteCount() >= 3 && HALBot::enoughTotes())
			{
				lift->Set(1); //Raise lift
				Wait(0.25); //Totes must engage first
				lift->Set(0);
				return HALBot::finish;
			}
			//Raise the lift and cheat to alight the tote
			drivebase->GoFast();
			drivebase->Drive_v(0, STACK_CORRECTION_SPEED, 0); // @todo Calibrate
			Wait(STACK_CORRECTION_TIME);
			drivebase->Drive_v(0, 0, 0);
			drivebase->GoSlow();
			lift->Set(1);
			Wait(0.3);
			if (HALBot::enoughTotes())
				return HALBot::finish;
			else
				return HALBot::nextTote;
		}
		drivebase->Drive_v(0, 0, 0);
		if (lift != nullptr)
			lift->Set(-1); //Lower the lift for grabbing
		return HALBot::no_update;
	}
}
