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

		SmartDashboard::PutBoolean("GettingTote timerActive", timerActive);
		SmartDashboard::PutNumber("getTimer", getTimer.Get());

		if (getTimer.HasPeriodPassed(TOTE_PICKUP_TIME))
		{
			SmartDashboard::PutBoolean("getTimer has passed", true);
			timerActive = false;
			intake->Set(0);
			drivebase->Drive_v(0, 0, 0);
			return HALBot::timerExpired;
		}

		if (drivebase != nullptr)
			drivebase->Drive_v(0, 0.5, 0);
		SmartDashboard::PutString("State", "gettingTote");
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
			drivebase->Drive_v(0, 0, 0);
			return HALBot::timerExpired;

		}

		if (drivebase != nullptr)
			drivebase->Drive_v(1, 0, 0); //Hard right!
		SmartDashboard::PutString("State", "driveToZone");
		return HALBot::no_update;
	}

	int Stopped::update()
	{

		SmartDashboard::PutString("State", "stopped");
		return HALBot::no_update;
	}
	int Rotate::update()
	{
		if (!timerActive)
		{
			driveTimer.Reset();
			driveTimer.Start();
			timerActive = true;
		}
		if (driveTimer.HasPeriodPassed(ROTATE_TIME))
		{
			timerActive = false;
			drivebase->Drive_v(0, 0, 0);
			return HALBot::timerExpired;
		}
		if (drivebase != nullptr)
			drivebase->Drive_v(0, 0, 1);
		SmartDashboard::PutString("State", "rotate");
		return HALBot::no_update;

	}


	HALBot::HALBot()
	{
		stopped = new Stopped;
		gettingTote = new GettingTote;
		driveToZone = new DriveToZone;
		rotate = new Rotate;
		drivebase = nullptr;
		intake = nullptr;
		fsm = new FiniteStateMachine;
	}
	HALBot::~HALBot()
	{
		delete stopped;
		delete gettingTote;
		delete driveToZone;
		delete rotate;
		delete fsm;
	}
	void HALBot::init(MecanumDrive* newDrivebase, MotorGrouping* newIntake)
	{
		drivebase = newDrivebase;
		intake = newIntake;

		gettingTote->setHardware(drivebase, intake);
		driveToZone->setHardware(drivebase);
		rotate->setHardware(drivebase);


		transitionTable[0] = {gettingTote, HALBot::timerExpired, nullptr, driveToZone},
		transitionTable[1] = {driveToZone, HALBot::timerExpired, nullptr, rotate},
		transitionTable[2] = {rotate, HALBot::timerExpired, nullptr, stopped},
		transitionTable[3] = END_STATE_TABLE;

		fsm->init(transitionTable, gettingTote);
	}
	void HALBot::update()
	{
		fsm->update();
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
