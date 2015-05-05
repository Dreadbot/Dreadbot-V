#include "HALBot.h"

namespace dreadbot
{
	HALBot::HALBot()
	{
		sysLog = nullptr;
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
		strafeLeft = new StrafeLeft;
		mode = AUTON_MODE_DRIVE; //Just drive straight forward. Assumes a spherical cow.
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
		delete strafeLeft;
		RoboState::toteCount = 0;
		RoboState::neededTCount = 0;
	}
	void HALBot::init(MecanumDrive* drivebase, MotorGrouping* intake, PneumaticGrouping* lift)
	{
		int i;
		sysLog = Logger::getInstance()->getLog("sysLog");
		RoboState::drivebase = drivebase;
		RoboState::intake = intake;
		RoboState::lift = lift;
		RoboState::liftArms = XMLInput::getInstance()->getPGroup("liftArms");
		RoboState::intakeArms = XMLInput::getInstance()->getPGroup("intakeArms");
		RoboState::pusher1 = XMLInput::getInstance()->getPWMMotor(0);
		RoboState::pusher2 = XMLInput::getInstance()->getPWMMotor(1);
		RoboState::sysLog = sysLog;
		pushContainer->pushConstant = 1;

		//Apply state tables and set the starting state. Note that RoboState::neededTCount is 0 before this.
		FSMState* defState = nullptr;
		if (mode == AUTON_MODE_STOP)
		{
			i = 0;
			transitionTable[i++] = {stopped, RoboState::no_update, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
			defState = stopped;
		}
		if (mode == AUTON_MODE_DRIVE)
		{
			i = 0;
			driveToZone->strafe = false;
			driveToZone->dir = -1;
			transitionTable[i++] = {driveToZone, RoboState::timerExpired, nullptr, rotate};
			transitionTable[i++] = {rotate, RoboState::timerExpired, nullptr, stopped};
			transitionTable[i++] = {stopped, RoboState::no_update, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
			defState = driveToZone;
		}
		if (mode == AUTON_MODE_TOTE)
		{
			i = 0;
			RoboState::neededTCount = 1;
			rotate->rotateConstant = 1;
			driveToZone->strafe = false;
			transitionTable[i++] = {gettingTote, RoboState::timerExpired, nullptr, forkGrab};
			transitionTable[i++] = {forkGrab, RoboState::finish, nullptr, rotate};
			transitionTable[i++] = {rotate, RoboState::timerExpired, nullptr, driveToZone};
			transitionTable[i++] = {driveToZone, RoboState::timerExpired, nullptr, rotate2};
			transitionTable[i++] = {rotate2, RoboState::timerExpired, nullptr, stopped};
			transitionTable[i++] = {gettingTote, RoboState::eStop, nullptr, stopped};
			transitionTable[i++] = {stopped, RoboState::no_update, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
			defState = gettingTote;
		}
		if (mode == AUTON_MODE_CONTAINER)
		{
			driveToZone->strafe = true;
			i = 0;
			rotate->rotateConstant = -1;
			transitionTable[i++] = {rotate, RoboState::timerExpired, nullptr, driveToZone};
			transitionTable[i++] = {driveToZone, RoboState::timerExpired, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
		}
		if (mode == AUTON_MODE_BOTH)
		{
			//Not really used.
			i = 0;
			RoboState::neededTCount = 1;
			transitionTable[i++] = {stopped, RoboState::no_update, nullptr, stopped};
			transitionTable[i++] = END_STATE_TABLE;
			defState = stopped;
		}
		if (mode == AUTON_MODE_STACK2)
		{
			i = 0;
			rotateDrive->rotateConstant = -1;
			pushContainer->pushConstant = -1;
			pushContainer->enableScaling = true;
			driveToZone->strafe = false;
			RoboState::toteCount++; //We already have a tote
			RoboState::neededTCount = 2;

			transitionTable[i++] = {pushContainer, RoboState::timerExpired, nullptr, gettingTote};
			transitionTable[i++] = {gettingTote, RoboState::timerExpired, nullptr, forkGrab};
			transitionTable[i++] = {gettingTote, RoboState::eStop, nullptr, stopped};
			transitionTable[i++] = {forkGrab, RoboState::finish, nullptr, strafeLeft};
			transitionTable[i++] = {forkGrab, RoboState::nextTote, nullptr, pushContainer};
			transitionTable[i++] = {strafeLeft, RoboState::timerExpired, nullptr, rotateDrive};
			transitionTable[i++] = {rotateDrive, RoboState::timerExpired, nullptr, stopped};
			transitionTable[i++] = {stopped, RoboState::no_update, nullptr, stopped};
			defState = pushContainer;
		}
		if (mode == AUTON_MODE_STACK3)
		{
			i = 0;
			rotateDrive->rotateConstant = -1;
			pushContainer->pushConstant = -1;
			pushContainer->enableScaling = true;
			driveToZone->strafe = false;
			RoboState::toteCount++; //We already have a tote
			RoboState::neededTCount = 3;

			transitionTable[i++] = {pushContainer, RoboState::timerExpired, nullptr, gettingTote};
			transitionTable[i++] = {gettingTote, RoboState::timerExpired, nullptr, forkGrab};
			transitionTable[i++] = {forkGrab, RoboState::nextTote, nullptr, pushContainer};
			transitionTable[i++] = {forkGrab, RoboState::finish, nullptr, rotateDrive};
			transitionTable[i++] = {rotateDrive, RoboState::timerExpired, nullptr, backAway};
			transitionTable[i++] = {backAway, RoboState::timerExpired, nullptr, stopped};
			transitionTable[i++] = {gettingTote, RoboState::eStop, nullptr, stopped};
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
