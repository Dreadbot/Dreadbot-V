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
	void GettingTote::enter()
	{
		XMLInput::getInstance()->getPGroup("lift")->Set(1);
	}
	int GettingTote::update()
	{

		if (isToteInTransit() && !timerActive)
		{
			//Stay still while a tote is loaded
			timerActive = true;
			getTimer.Reset();
			getTimer.Start();
			drivebase->Drive_v(0, 0, 0);
			SmartDashboard::PutBoolean("Tote collected", true);
		}
		if (getTimer.HasPeriodPassed(TOTE_GRAB_DELAY) && timerActive)
		{
			//Tote successfully collected.
			intake->Set(0);
			timerActive = false;
			return HALBot::timerExpired;
		}
		if (timerActive)
		{
			//Keep sucking a tote in
			intake->Set(-0.5);
			return HALBot::no_update;
		}

		drivebase->Drive_v(0, -0.5, 0);
		intake->Set(-0.5);
		SmartDashboard::PutString("State", "gettingTote");
		SmartDashboard::PutBoolean("Tote collected", false);
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
	void DriveToZone::enter()
	{
		driveTimer.Reset();
		driveTimer.Start();
		timerActive = true;
		XMLInput::getInstance()->getPGroup("lift")->Set(1);
	}
	int DriveToZone::update()
	{
		if (driveTimer.HasPeriodPassed(DRIVE_TO_ZONE_TIME))
		{
			timerActive = false;
			drivebase->Drive_v(0, 0, 0);
			return HALBot::timerExpired;

		}

		if (drivebase != nullptr)
			drivebase->Drive_v(0, -0.75, 0); //Straight forward
		SmartDashboard::PutString("State", "driveToZone");
		return HALBot::no_update;
	}

	ForkGrab::ForkGrab()
	{
		timerActive = false;
		lift = nullptr;
	}
	void ForkGrab::enter()
	{
		timerActive = true;
		grabTimer.Start();
	}
	int ForkGrab::update()
	{
		DigitalInput* lowSwitch = new DigitalInput(0); //DI slot?

		if (!lowSwitch->Get())
		{
			timerActive = false;
			grabTimer.Stop();
			//Raise the lift
			lift->Set(1);
			Wait(0.5); //Cheat
			HALBot::incrTote();
			delete lowSwitch;
			if (HALBot::enoughTotes())
				return HALBot::finish;
			else
				return HALBot::nextTote;
		}
		SmartDashboard::PutString("State", "ForkGrab");
		if (lift != nullptr)
			lift->Set(-1); //Lower the lift for grabbing?
		delete lowSwitch;
		return HALBot::no_update;
	}

	RaiseForks::RaiseForks()
	{
		timerActive = false;
		lift = nullptr;
	}
	void RaiseForks::enter()
	{
		timerActive = false;
		grabTimer.Stop();
		grabTimer.Reset();
		grabTimer.Start();
	}
	int RaiseForks::update()
	{
		if (grabTimer.HasPeriodPassed(RAISE_STACK_TIME))
		{
			timerActive = false;
			grabTimer.Stop();
			lift->Set(0);
			return HALBot::timerExpired;
		}
		SmartDashboard::PutString("State", "raiseForks");
		if (lift != nullptr)
			lift->Set(1);
		return HALBot::no_update;
	}

	void Stopped::enter()
	{
		// do nothing
	}
	int Stopped::update()
	{
		lift->Set(-1);
		SmartDashboard::PutString("State", "stopped");
		return HALBot::no_update;
	}

	Rotate::Rotate()
	{
		timerActive = false;
		rotateConstant = 1;
	}
	void Rotate::enter()
	{
		driveTimer.Reset();
		driveTimer.Start();
		timerActive = true;
	}
	int Rotate::update()
	{
		if (driveTimer.HasPeriodPassed(ROTATE_TIME))
		{
			timerActive = false;
			drivebase->Drive_v(0, 0, 0);
			return HALBot::timerExpired;
		}
		if (drivebase != nullptr)
			drivebase->Drive_v(0, 0, 0.5 * rotateConstant);

		SmartDashboard::PutNumber("rotateTimer", driveTimer.Get());
		SmartDashboard::PutString("State", "rotate");
		return HALBot::no_update;
	}
	void BackAway::enter()
	{
		// do nothing
	}
	int BackAway::update()
	{
		if (!timerActive)
		{
			grabTimer.Reset();
			grabTimer.Start();
			timerActive = true;
			if (lift != nullptr)
				lift->Set(0);
			Wait(0.66); //Another cheat
		}

		if (grabTimer.HasPeriodPassed(BACK_AWAY_TIME))
		{
			timerActive = false;
			drivebase->Drive_v(0, 0, 0);
			lift->Set(0);
			return HALBot::timerExpired;
		}

		if (drivebase != nullptr)
			drivebase->Drive_v(0, -0.75, 0); //Back up
		if (lift != nullptr)
			lift->Set(-1); //Lower the lift so the tote goes free
		SmartDashboard::PutString("State", "backAway");
		return HALBot::no_update;
	}
	void PushContainer::enter()
	{
		pushConstant *= 1;
	}
	int PushContainer::update()
	{
		if (!timerActive)
		{
			driveTimer.Reset();
			driveTimer.Start();
			timerActive = true;
		}

		if (driveTimer.HasPeriodPassed(PUSH_TIME))
		{
			timerActive = false;
			drivebase->Drive_v(0, 0, 0);
			return HALBot::timerExpired;
		}

		if (drivebase != nullptr)
			drivebase->Drive_v(0, -0.75, 0); //Straight forward
		if (pusher1 != nullptr)
			pusher1->Set(1 * pushConstant); //Push the container?
		if (pusher2 != nullptr)
			pusher2->Set(1 * pushConstant);
		SmartDashboard::PutString("State", "pushContainer");
		return HALBot::no_update;
	}


	int HALBot::toteCount = 0;
	AutonMode HALBot::mode = AUTON_MODE_STOP;
	bool HALBot::enoughTotes()
	{
		SmartDashboard::PutNumber("toteCount", toteCount);
		switch (mode)
		{
		case AUTON_MODE_TOTE:
			return toteCount >= 1;
			break;
		case AUTON_MODE_STACK3:
			if (toteCount >= 3) //Probably works now. Probably.
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
		raiseForks = new RaiseForks;
		pushContainer = new PushContainer;
		backAway = new BackAway;
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
		delete raiseForks;
		delete pushContainer;
		delete backAway;
		delete fsm;
		toteCount = 0;
	}
	void HALBot::init(MecanumDrive* drivebase, MotorGrouping* intake, PneumaticGrouping* lift)
	{
		int i;
		gettingTote->setHardware(drivebase, intake);
		driveToZone->setHardware(drivebase);
		rotate->setHardware(drivebase);
		rotate2->setHardware(drivebase);
		pushContainer->setHardware(drivebase);
		pushContainer->pusher1 = XMLInput::getInstance()->getPWMMotor(0);
		pushContainer->pusher2 = XMLInput::getInstance()->getPWMMotor(1);
		pushContainer->pushConstant = 1;
		stopped->lift = lift; //Don't know if I like these...
		forkGrab->lift = lift;
		backAway->lift = lift;
		raiseForks->lift = lift;
		backAway->drivebase = drivebase;


		//Apply state tables and set the starting state
		FSMState* defState = nullptr;
		if (mode == AUTON_MODE_STOP)
		{
			i = 0;
			transitionTable[i++] = {stopped, HALBot::no_update, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
			defState = stopped;
		}
		if (mode == AUTON_MODE_DRIVE)
		{
			i = 0;
			transitionTable[i++] = {driveToZone, HALBot::timerExpired, nullptr, rotate};
			transitionTable[i++] = {rotate, HALBot::timerExpired, nullptr, stopped};
			transitionTable[i++] = {stopped, HALBot::no_update, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
			defState = driveToZone;
		}
		if (mode == AUTON_MODE_TOTE)
		{
			i = 0;
			rotate->rotateConstant = -1;
			transitionTable[i++] = {gettingTote, HALBot::timerExpired, nullptr, forkGrab};
			transitionTable[i++] = {forkGrab, HALBot::finish, nullptr, rotate};
			transitionTable[i++] = {rotate, HALBot::timerExpired, nullptr, driveToZone};
			transitionTable[i++] = {driveToZone, HALBot::timerExpired, nullptr, rotate2};
			transitionTable[i++] = {rotate2, HALBot::timerExpired, nullptr, backAway};
			transitionTable[i++] = {backAway, HALBot::timerExpired, nullptr, stopped};
			transitionTable[i++] = {stopped, HALBot::no_update, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
			defState = gettingTote;
		}
		if (mode == AUTON_MODE_CONTAINER)
		{
			i = 0;
			transitionTable[i++] = {driveToZone, HALBot::timerExpired, nullptr, rotate};
			transitionTable[i++] = {rotate, HALBot::timerExpired, nullptr, stopped};
			transitionTable[i++] = {stopped, HALBot::no_update, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
			defState = driveToZone;
		}
		if (mode == AUTON_MODE_BOTH)
		{
			i = 0;
			transitionTable[i++] = {stopped, HALBot::no_update, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
			defState = stopped;
		}
		if (mode == AUTON_MODE_STACK3 || mode == AUTON_MODE_STACK2)
		{
			i = 0;
			rotate->rotateConstant = -1;
			transitionTable[i++] = {gettingTote, HALBot::timerExpired, nullptr, forkGrab};
			transitionTable[i++] = {forkGrab, HALBot::nextTote, nullptr, pushContainer};
			transitionTable[i++] = {forkGrab, HALBot::finish, nullptr, rotate};
			transitionTable[i++] = {pushContainer, HALBot::timerExpired, nullptr, gettingTote};
			transitionTable[i++] = {rotate, HALBot::timerExpired, nullptr, driveToZone};
			transitionTable[i++] = {driveToZone, HALBot::timerExpired, nullptr, rotate2};
			transitionTable[i++] = {rotate2, HALBot::timerExpired, nullptr, backAway};
			transitionTable[i++] = {backAway, HALBot::timerExpired, nullptr, stopped};
			transitionTable[i++] = {stopped, HALBot::no_update, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
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
