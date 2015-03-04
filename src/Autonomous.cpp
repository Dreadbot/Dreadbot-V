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

		if (isToteInTransit() && !timerActive)
		{
			//Stay still while a tote is loaded
			timerActive = true;
			getTimer.Start();
			drivebase->Drive_v(0, 0, 0);
		}
		if (timerActive)
		{
			//Keep sucking a tote in
			intake->Set(-1);
			return HALBot::no_update;
		}
		if (getTimer.HasPeriodPassed(TOTE_GRAB_DELAY) && timerActive)
		{
			//Tote successfully collected.
			intake->Set(0);
			timerActive = false;
			HALBot::incrTote();
			return HALBot::timerExpired;
		}

		drivebase->Drive_v(0, -0.5, 0);
		intake->Set(-1);
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
			drivebase->Drive_v(0, -0.75, 0); //Straight forward
		SmartDashboard::PutNumber("DriveToZone timer", driveTimer.Get());
		SmartDashboard::PutString("State", "driveToZone");
		return HALBot::no_update;
	}

	ForkGrab::ForkGrab()
	{
		timerActive = false;
		lift = nullptr;
	}
	int ForkGrab::update()
	{
		if (!timerActive)
		{
			timerActive = true;
			grabTimer.Start();
		}

		if (grabTimer.HasPeriodPassed(LOWER_STACK_TIME))
		{
			timerActive = false;
			grabTimer.Stop();
			//Raise the lift
			lift->Set(1);
			if (HALBot::enoughTotes())
			{
				HALBot::incrTote();
				return HALBot::nextTote;
			}
			else
				return HALBot::finish;
		}
		SmartDashboard::PutString("State", "grabTote");
		if (lift != nullptr)
			lift->Set(-1); //Lower the lift for grabbing?
		return HALBot::no_update;
	}

	int Stopped::update()
	{
		lift->Set(-1);
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

		SmartDashboard::PutNumber("rotateTimer", driveTimer.Get());
		SmartDashboard::PutString("State", "rotate");
		return HALBot::no_update;
	}
	int BackAway::update()
	{
		return HALBot::no_update; //Temporary
	}
	int PushContainer::update()
	{
		return HALBot::no_update; //Temporary
	}

	int HALBot::toteCount = 0;
	AutonMode HALBot::mode = AUTON_MODE_STOP;
	bool HALBot::enoughTotes()
	{
		switch (mode)
		{
		case AUTON_MODE_TOTE:
			if (toteCount == 1)
				return true;
			else return false;
			break;
		case AUTON_MODE_STACK3:
			if (toteCount == 3) //Probably works now. Probably.
				return true;
			else return false;
		default:
			return true;
		}
	}
	void HALBot::incrTote()
	{
		toteCount++;
	}
	HALBot::HALBot()
	{
		stopped = new Stopped;
		gettingTote = new GettingTote;
		driveToZone = new DriveToZone;
		rotate = new Rotate;
		rotate2 = new Rotate;
		forkGrab = new ForkGrab;
		pushContainer = new PushContainer;
		backAway = new BackAway;
		drivebase = nullptr;
		intake = nullptr;
		fsm = new FiniteStateMachine;
		mode = AUTON_MODE_DRIVE;
	}
	HALBot::~HALBot()
	{
		delete stopped;
		delete gettingTote;
		delete driveToZone;
		delete rotate;
		delete forkGrab;
		delete pushContainer;
		delete backAway;
		delete fsm;
		toteCount = 0;
	}
	void HALBot::init(MecanumDrive* newDrivebase, MotorGrouping* newIntake, PneumaticGrouping* lift)
	{
		drivebase = newDrivebase;
		intake = newIntake;

		gettingTote->setHardware(drivebase, intake);
		driveToZone->setHardware(drivebase);
		rotate->setHardware(drivebase);
		rotate2->setHardware(drivebase);
		pushContainer->setHardware(drivebase, intake);
		backAway->setHardware(drivebase);
		stopped->lift = lift; //Don't know if I like these...
		forkGrab->lift = lift;


		//Apply state tables and set the starting state
		FSMState* defState = nullptr;
		if (mode == AUTON_MODE_STOP)
		{
			transitionTable[0] = END_STATE_TABLE;
			defState = stopped;
		}
		if (mode == AUTON_MODE_DRIVE)
		{
			transitionTable[0] = {driveToZone, HALBot::timerExpired, nullptr, rotate};
			transitionTable[1] = {rotate, HALBot::timerExpired, nullptr, rotate2};
			transitionTable[2] = {rotate2, HALBot::timerExpired, nullptr, stopped};
			transitionTable[3] = END_STATE_TABLE;
			defState = driveToZone;
		}
		if (mode == AUTON_MODE_TOTE)
		{
			transitionTable[0] = {gettingTote, HALBot::timerExpired, nullptr, rotate};
			transitionTable[1] = {rotate, HALBot::timerExpired, nullptr, driveToZone};
			transitionTable[2] = {driveToZone, HALBot::timerExpired, nullptr, rotate2};
			transitionTable[3] = {rotate2, HALBot::timerExpired, nullptr, stopped};
			transitionTable[4] = END_STATE_TABLE;
			defState = gettingTote;
		}
		if (mode == AUTON_MODE_CONTAINER)
		{
			transitionTable[0] = END_STATE_TABLE;
			defState = stopped;
		}
		if (mode == AUTON_MODE_BOTH)
		{
			transitionTable[0] = END_STATE_TABLE;
			defState = stopped;
		}
		if (mode == AUTON_MODE_STACK2)
		{
			transitionTable[0] = END_STATE_TABLE;
			defState = stopped;
		}
		if (mode == AUTON_MODE_STACK3)
		{
			transitionTable[0] = {gettingTote, HALBot::timerExpired, nullptr, forkGrab};
			transitionTable[1] = {forkGrab, HALBot::nextTote, nullptr, gettingTote};
			transitionTable[2] = {forkGrab, HALBot::finish, nullptr, rotate};
			transitionTable[3] = {rotate, HALBot::timerExpired, nullptr, driveToZone};
			transitionTable[4] = {driveToZone, HALBot::timerExpired, nullptr, rotate2};
			transitionTable[5] = {rotate2, HALBot::timerExpired, nullptr, stopped};
			transitionTable[6] = END_STATE_TABLE;
			defState = gettingTote;
		}

		fsm->init(transitionTable, defState);
	}
	void HALBot::update()
	{
		fsm->update();
		SmartDashboard::PutNumber("toteCount", toteCount);
	}
	void HALBot::setMode(AutonMode newMode)
	{
		mode = newMode;
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
