#include "Autonomous.h"

namespace dreadbot
{
	//States
	GettingTote::GettingTote()
	{
		drivebase = nullptr;
		intake = nullptr;
		timerActive = false;
	}
	void GettingTote::setHardware(MecanumDrive* newDrivebase, MotorGrouping* newIntake)
	{
		drivebase = newDrivebase;
		intake = newIntake;
	}
	int GettingTote::update()
	{
		if (!timerActive)
		{
			getTimer.Reset();
			getTimer.Start();
			timerActive = true;
		}

		if (getTimer.HasPeriodPassed(TOTE_PICKUP_TIME))
		{
			timerActive = false;
			return HALBot::timerExpired;
		}

		if (drivebase != nullptr)
			drivebase->Drive_v(0, 0.5, 0); //Forward straight
		if (intake != nullptr)
			intake->Set(1); //Intake?
		return HALBot::no_update;
	}

	DriveToZone::DriveToZone()
	{
		drivebase = nullptr;
		timerActive = false;
	}
	void DriveToZone::setHardware(MecanumDrive* newDrivebase)
	{
		drivebase = newDrivebase;
	}
	int DriveToZone::update()
	{
		if (!timerActive)
		{
			driveTimer.Reset();
			driveTimer.Start();
			timerActive = true;
		}

		if (driveTimer.HasPeriodPassed(DRIVE_TO_ZONE_TIME))
		{
			timerActive = false;
			return HALBot::timerExpired;
		}

		if (drivebase != nullptr)
			drivebase->Drive_v(1, 0, 0); //Hard right!
		return HALBot::no_update;
	}

	int Stopped::update()
	{
		return HALBot::no_update;
	}

	void HALBot::init(MecanumDrive* newDrivebase, MotorGrouping* newIntake)
	{
		drivebase = newDrivebase;
		intake = newIntake;

		stopped = new Stopped;
		gettingTote = new GettingTote;
		gettingTote->setHardware(drivebase, intake);
		driveToZone = new DriveToZone;
		driveToZone->setHardware(drivebase);

		currentState = stopped;

		stateTable[0] = {gettingTote, HALBot::timerExpired, nullptr, driveToZone};
		stateTable[1] = {driveToZone, HALBot::timerExpired, nullptr, stopped};
		stateTable[2] = END_STATE_TABLE;
	}
	void HALBot::start()
	{
		//Starts the bot iff it's stopped
		if (currentState == stopped)
			currentState = gettingTote;
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
