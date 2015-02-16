#include "Autonomous.h"

namespace dreadbot
{
	RobotFSM::RobotFSM()
	{
		drivebase = 0;
		transit = 0;
		intake = 0;

		state = stopped;
	}
	void RobotFSM::update()
	{
		SmartDashboard::PutNumber("Time", globalTimer.Get());
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
	void RobotFSM::setHardware(MecanumDrive* base, MotorGrouping* newIntake, MotorGrouping* newTransit)
	{
		drivebase = base;
		intake = newIntake;
		transit = newTransit;
	}
	void RobotFSM::switchState(robotState newState)
	{
		//This might be needed for special state-switching behavior.
		//At the moment, it does nothing.
		state = newState;
	}
	void RobotFSM::start()
	{
		globalTimer.Stop(); //TODO: Determine if 'stop' is needed
		globalTimer.Reset();
		switchState(getting_tote);
		globalTimer.Start();
	}
};
