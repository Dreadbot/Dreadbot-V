#include "Autonomous.h"

namespace dreadbot
{
	RobotFSM::RobotFSM()
	{
		drivebase = NULL;
		transit = NULL;
		intake = NULL;

		state = start;
	}
	void RobotFSM::update()
	{
		if (state == start)
		{
			//Start the timer, then switch to getting_tote
			globalTimer.Start();
			switchState(getting_tote);
		}
		if (state == stopped)
		{
			globalTimer.Stop();
			SmartDashboard::PutBoolean("Auton Complete: ", true);
		}
		if (state == getting_tote)
		{
			//Has the tote already been picked up?
			if (globalTimer.HasPeriodPassed(TOTE_PICKUP_TIME))
				switchState(drive_to_zone);

			drivebase->Drive_v(0, .75, 0); //Drive forward at 3/4 speed
			intake->Set(1.0); //These need to be opposite of each other
			transit->Set(-1.0);
		}

		else if (state == drive_to_zone)
		{
			if (globalTimer.HasPeriodPassed(DRIVE_TO_ZONE_TIME))
				switchState(stopped);

			//Hard right!
			drivebase->Drive_v(0, 1, 0);
		}
	}
	void RobotFSM::SetHardware(MecanumDrive* base, MotorGrouping* newIntake, MotorGrouping* newTransit)
	{
		drivebase = base;
		intake = newIntake;
		transit = newTransit;
	}
	void RobotFSM::switchState(robotState newState)
	{
		//This might be needed for special state-switching behavior.
		//At the moment, it does nothing.
	}
};
