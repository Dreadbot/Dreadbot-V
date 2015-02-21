#include "Autonomous.h"

namespace dreadbot
{
	RobotFSM::RobotFSM()
	{
		drivebase = 0;
		intake = 0;

		state = stopped;

		frontUltra = 0;
		rearUltra = 0;
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

			drivebase->Drive_v(0, 0.75, getParallelTurnDir(frontUltra, rearUltra)); //Drive forward at 3/4 speed with parallel ultrasonics enabled
			intake->Set(1.0); //These need to be opposite of each other
		}

		else if (state == drive_to_zone)
		{
			if (globalTimer.HasPeriodPassed(DRIVE_TO_ZONE_TIME))
				switchState(stopped);

			//Hard right!
			drivebase->Drive_v(0, 1, 0);
		}
	}
	void RobotFSM::setHardware(MecanumDrive* base, MotorGrouping* newIntake)
	{
		drivebase = base;
		intake = newIntake;
	}
	void RobotFSM::setUltras(Ultrasonic* newFrontUltra, Ultrasonic* newRearUltra)
	{
		frontUltra = newFrontUltra;
		rearUltra = newRearUltra;
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

	float getParallelTurnDir(Ultrasonic* frontUltra, Ultrasonic* rearUltra)
	{
		if (frontUltra == 0 || rearUltra == 0)
			return 0;

		//Get the approximate difference in distances - used for angle calculation?
		float frontDelta = frontUltra->GetRangeMM() - DIST_FROM_WALL;
		float rearDelta = rearUltra->GetRangeMM() - DIST_FROM_WALL;
		float totalDelta = frontDelta + (rearDelta * -1);

		//Get the actual angle, then return the cosine of it (for steering?)
		float angle = atan(totalDelta / ULTRASONIC_SEPARATION); //This might need testing
		return cos(angle);
	}
}
