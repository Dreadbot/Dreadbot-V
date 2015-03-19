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
		XMLInput::getInstance()->getPGroup("lift")->Set(1); //Raise the lift
		eStopTimer.Start(); //estop timer - if limit is passed, automatically estops the robot.
	}
	int GettingTote::update()
	{
		if (isToteInTransit() && !timerActive)
		{
			//Stay still while a tote is loaded
			timerActive = true; //This doesn't really control a timer anymore...
			drivebase->Drive_v(0, 0, 0);
		}
		if (!isToteInTransit() && timerActive)
		{
			//Tote successfully collected.
			intake->Set(0);
			timerActive = false;
			eStopTimer.Stop();
			eStopTimer.Reset();
			return HALBot::timerExpired;
		}
		if (timerActive)
		{
			//Keep sucking a tote in
			intake->Set(-0.5);
			return HALBot::no_update;
		}
		if (HALBot::getToteCount() != 0) //Open the intake arms for grabbing totes after the first tote is collected
			XMLInput::getInstance()->getPGroup("intakeArms")->Set(-1);
		drivebase->Drive_v(0, -0.75, 0);
		intake->Set(-0.6);

		//E-stop in case the tote is missed. Zeros the velocity and intake/transit motors, and puts the robot into Stopped
		if (eStopTimer.Get() >= ESTOP_TIME)
		{
			eStopTimer.Stop();
			eStopTimer.Reset();
			if (drivebase != nullptr) drivebase->Drive_v(0, 0, 0);
			if (intake != nullptr) intake->Set(0);
			return HALBot::eStop;
		}

		return HALBot::no_update;
	}

	DriveToZone::DriveToZone()
	{
		drivebase = nullptr;
		timerActive = false;
		strafe = false;
		dir = 1; //Multiplier that changes the direction the robot moves.
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
		if (HALBot::getToteCount() < 3)
			XMLInput::getInstance()->getPGroup("lift")->Set(1); //Raise the lift for tote transit - it's more stable that way.
	}
	int DriveToZone::update()
	{
		float drvZoneTime = DRIVE_TO_ZONE_TIME;
		if (HALBot::getToteCount() == 0)
			drvZoneTime += 0.4f; //The robot starts farther back when no tote is collected. This adds on additional time to make sure the robot still drives far enough.

		//Break once the robot has moved far enough - timing based.
		if ((driveTimer.Get() >= drvZoneTime && !strafe) || (driveTimer.Get() >= STRAFE_TO_ZONE_TIME && strafe))
		{
			driveTimer.Stop();
			driveTimer.Reset();
			timerActive = false;
			drivebase->Drive_v(0, 0, 0);
			return HALBot::timerExpired;
		}

		//Apply actual velocity changes
		if (drivebase != nullptr)
		{
			if (strafe)
				drivebase->Drive_v(1, 0, 0); //Right
			else
				drivebase->Drive_v(0, 0.8 * dir, 0); // Do a short dance
		}

		return HALBot::no_update;
	}

	ForkGrab::ForkGrab()
	{
		timerActive = false;
		drivebase = nullptr;
		lift = nullptr;
	}
	void ForkGrab::enter()
	{
	}
	int ForkGrab::update()
	{
		if (isLiftDown())
		{
			HALBot::incrTote();

			//special 3-tote auton condition. Really sketchy. Causes the robot to NOT lift before rotating/driving
			if (HALBot::getToteCount() >= 3 && HALBot::enoughTotes())
			{
				lift->Set(1); //Raise lift
				Wait(0.25); //Totes must engage first
				lift->Set(0);
				return HALBot::finish;
			}

			//Raise the lift and cheat to alight the tote
			drivebase->Drive_v(0, 1, 0); // @todo Calibrate
			Wait(STACK_CORRECTION_TIME);
			drivebase->Drive_v(0, 0, 0);
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

	void Stopped::enter()
	{
		// do nothing
	}
	int Stopped::update()
	{
		return HALBot::no_update; //Does nothing of significance.
	}

	Rotate::Rotate()
	{
		timerActive = false;
		rotateConstant = 1; //Changes the direction that the robot turns.
	}
	void Rotate::enter()
	{
		driveTimer.Reset();
		driveTimer.Start();
		timerActive = true;
	}
	int Rotate::update()
	{
		if (driveTimer.Get() >= ROTATE_TIME)
		{ //Rotated far enough; break
			timerActive = false;
			drivebase->Drive_v(0, 0, 0);
			if (HALBot::getToteCount() == 3)
				XMLInput::getInstance()->getPGroup("lift")->Set(-1); //Lower lift
			return HALBot::timerExpired;
		}
		if (drivebase != nullptr)
			drivebase->Drive_v(0, 0, 0.5 * rotateConstant);
		return HALBot::no_update;
	}

	void BackAway::enter()
	{
		// do nothing
	}
	int BackAway::update()
	{
		//This entire function may not work properly.
		if (!timerActive)
		{
			grabTimer.Reset();
			grabTimer.Start();
			timerActive = true;
			if (lift != nullptr)
				lift->Set(-1); //Lower the lift
		}

		if (grabTimer.Get() >= BACK_AWAY_TIME)
		{
			timerActive = false;
			drivebase->Drive_v(0, 0, 0);
			XMLInput::getInstance()->getPGroup("liftArms")->Set(-1);
			return HALBot::timerExpired;
		}

		if (lift != nullptr)
			lift->Set(-1); //Lower the lift so the tote goes free
		return HALBot::no_update;
	}

	PushContainer::PushContainer()
	{
		enableScaling = false;
	}
	void PushContainer::enter()
	{
		pushConstant *= -1; //Not used, but it can change the direction the robot pushes containers
	}
	int PushContainer::update()
	{
		float pushTime = PUSH_TIME;
		if (enableScaling)
			pushTime += ((float)HALBot::getToteCount() - 1) / 3; //Scaling for three-tote autonomous, since the second container is farther away than the first
		XMLInput::getInstance()->getPGroup("intakeArms")->Set(1); //Intake arms in
		if (!timerActive)
		{
			driveTimer.Reset();
			driveTimer.Start();
			timerActive = true;
		}

		if (driveTimer.Get() >= pushTime)
		{
			timerActive = false;
			drivebase->Drive_v(0, 0, 0);
			return HALBot::timerExpired;
		}

		if (drivebase != nullptr)
			drivebase->Drive_v(0, -PUSH_SPEED, 0); //Straight forward
		if (pusher1 != nullptr)
			pusher1->Set(1); //Push the container?
		if (pusher2 != nullptr)
			pusher2->Set(1);
		return HALBot::no_update;
	}

	RotateDrive::RotateDrive()
	{
		timerActive = false;
		rotateConstant = 1;
	}
	void RotateDrive::enter()
	{
		//Does nothing
	}
	int RotateDrive::update()
	{
		if (driveTimer.Get() >= (ROTATE_TIME - 1.0f))
		{ //Rotated far enough; break
			timerActive = false;
			drivebase->Drive_v(0, 0, 0);
			if (HALBot::getToteCount() == 3) {
				XMLInput::getInstance()->getPGroup("lift")->Set(-1); //Lower lift
				XMLInput::getInstance()->getPGroup("lift")->Set(-1);
			}
			return HALBot::timerExpired;
		}
		if (drivebase != nullptr)
			drivebase->Drive_v(0, 1.0 * dir, 0.5 * rotateConstant); // @todo Add RotateDrive coefficients to preprocessor definitions
		return HALBot::no_update;
	}


	int HALBot::toteCount = 0;
	AutonMode HALBot::mode = AUTON_MODE_STOP;
	int HALBot::getToteCount()
	{
		return toteCount;
	}
	bool HALBot::enoughTotes()
	{
		switch (mode)
		{
		case AUTON_MODE_TOTE:
			return toteCount >= 1;
			break;
		case AUTON_MODE_STACK3:
			return toteCount >= 3;
			break;
		case AUTON_MODE_STACK2:
			return toteCount >= 2;
			break;
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
		fsm = new FiniteStateMachine;
		rotateDrive = new RotateDrive;
		mode = AUTON_MODE_DRIVE;
	}
	HALBot::~HALBot()
	{
		delete stopped;
		delete gettingTote;
		delete driveToZone;
		delete rotate;
		delete rotateDrive;
		delete forkGrab;
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
		rotateDrive->setHardware(drivebase);
		rotate2->setHardware(drivebase);
		pushContainer->setHardware(drivebase);
		pushContainer->pusher1 = XMLInput::getInstance()->getPWMMotor(0);
		pushContainer->pusher2 = XMLInput::getInstance()->getPWMMotor(1);
		pushContainer->pushConstant = 1;
		stopped->lift = lift; //Don't know if I like these...
		forkGrab->lift = lift;
		forkGrab->drivebase = drivebase;
		backAway->lift = lift;
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
			driveToZone->strafe = false;
			driveToZone->dir = -1;
			transitionTable[i++] = {driveToZone, HALBot::timerExpired, nullptr, rotate};
			transitionTable[i++] = {rotate, HALBot::timerExpired, nullptr, stopped};
			transitionTable[i++] = {stopped, HALBot::no_update, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
			defState = driveToZone;
		}
		if (mode == AUTON_MODE_TOTE)
		{
			i = 0;
			rotate->rotateConstant = 1;
			driveToZone->strafe = false;
			transitionTable[i++] = {gettingTote, HALBot::timerExpired, nullptr, forkGrab};
			transitionTable[i++] = {forkGrab, HALBot::finish, nullptr, rotate};
			transitionTable[i++] = {rotate, HALBot::timerExpired, nullptr, driveToZone};
			transitionTable[i++] = {driveToZone, HALBot::timerExpired, nullptr, rotate2};
			transitionTable[i++] = {rotate2, HALBot::timerExpired, nullptr, stopped};
			transitionTable[i++] = {gettingTote, HALBot::eStop, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
			defState = gettingTote;
		}
		if (mode == AUTON_MODE_CONTAINER)
		{
			driveToZone->strafe = true;
			i = 0;
			rotate->rotateConstant = -1;
			transitionTable[i++] = {rotate, HALBot::timerExpired, nullptr, driveToZone};
			transitionTable[i++] = {driveToZone, HALBot::timerExpired, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
		}
		if (mode == AUTON_MODE_BOTH)
		{
			i = 0;
			transitionTable[i++] = {stopped, HALBot::no_update, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
			defState = stopped;
		}
		if (mode == AUTON_MODE_STACK2)
		{
			i = 0;
			rotate->rotateConstant = 1;
			rotate2->rotateConstant = -1;
			pushContainer->pushConstant = -1;
			driveToZone->strafe = false;

			transitionTable[i++] = {gettingTote, HALBot::timerExpired, nullptr, forkGrab};
			transitionTable[i++] = {forkGrab, HALBot::nextTote, nullptr, pushContainer};
			transitionTable[i++] = {forkGrab, HALBot::finish, nullptr, rotate};
			transitionTable[i++] = {pushContainer, HALBot::timerExpired, nullptr, gettingTote};
			transitionTable[i++] = {rotate, HALBot::timerExpired, nullptr, driveToZone};
			transitionTable[i++] = {driveToZone, HALBot::timerExpired, nullptr, rotate2};
			transitionTable[i++] = {rotate2, HALBot::timerExpired, nullptr, stopped};
			//transitionTable[i++] = {backAway, HALBot::timerExpired, nullptr, stopped};
			transitionTable[i++] = {gettingTote, HALBot::eStop, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
			defState = gettingTote;
		}
		if (mode == AUTON_MODE_STACK3)
		{
			i = 0;
			rotateDrive->rotateConstant = -1;
			pushContainer->pushConstant = -1;
			pushContainer->enableScaling = true;
			driveToZone->strafe = false;
			incrTote(); //We already have a tote

			transitionTable[i++] = {pushContainer, HALBot::timerExpired, nullptr, gettingTote};
			transitionTable[i++] = {gettingTote, HALBot::timerExpired, nullptr, forkGrab};
			transitionTable[i++] = {forkGrab, HALBot::nextTote, nullptr, pushContainer};
			transitionTable[i++] = {forkGrab, HALBot::finish, nullptr, rotateDrive};
			transitionTable[i++] = {rotateDrive, HALBot::timerExpired, nullptr, driveToZone};
			transitionTable[i++] = {driveToZone, HALBot::timerExpired, nullptr, backAway};
			transitionTable[i++] = {backAway, HALBot::timerExpired, nullptr, stopped};
			transitionTable[i++] = {gettingTote, HALBot::eStop, nullptr, stopped};
			defState = pushContainer;
		}

		fsm->init(transitionTable, defState);
	}
	void HALBot::update()
	{
		fsm->update();
	}
	void HALBot::setMode(AutonMode newMode)
	{
		mode = newMode;
	}
	AutonMode HALBot::getMode()
	{
		return mode; //Used exactly once, in AutonInit(). This must be killed with fire when possible.
	}
}
