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
		if (RoboState::toteCount >= 2 && grabTimer.Get() >= LIFT_ENGAGEMENT_DELAY)
		{
			intakeArmsIn();
			drivebase->GoFast();
			drive(0, RD_DRIVE_SPEED, RD_ROTATE_SPEED);
			if (isLiftDown()) 
			{
				//XMLInput::getInstance()->getPGroup("liftArms")->Set(0);
				raiseLift();
				Wait(0.3); //Totes must engage first
				stopLift();
				return RoboState::finish;
			}
			else
				return RoboState::no_update;
		}
		else if (RoboState::toteCount >= 2 && grabTimer.Get() >= 0.2 && thing)
			//XMLInput::getInstance()->getPGroup("liftArms")->Set(-1);
			thing = false; //No-one but Parker, who left the team, knows what this is.

		if (isLiftDown())
		{
			//XMLInput::getInstance()->getPGroup("liftArms")->Set(0);
			RoboState::toteCount++; //Once the lift is down, it is assumed that the tote is actually collected, i.e. in the fork.

			//special 3-tote auton condition. Really kludgy. Causes the robot to NOT lift before rotating/driving
			if (RoboState::toteCount >= 3 && RoboState::toteCount >= RoboState::neededTCount)
			{
				raiseLift();
				Wait(0.25); //Totes must engage first
				stopLift();
				return RoboState::finish;
			}
			//Raise the lift and cheat to alight the tote
			drivebase->GoFast();
			drive(0, STACK_CORRECTION_SPEED, 0); // @todo Calibrate
			Wait(STACK_CORRECTION_TIME);
			drive(0, 0, 0);
			drivebase->GoSlow();
			raiseLift();
			Wait(0.3);
			if (RoboState::toteCount >= RoboState::neededTCount)
				return RoboState::finish;
			else
				return RoboState::nextTote;
		}
		drive(0, 0, 0);
		lowerLift(); //Lower the lift for grabbing
		return RoboState::no_update;
	}
}
