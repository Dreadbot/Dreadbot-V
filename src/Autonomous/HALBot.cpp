#include "HALBot.h"

namespace dreadbot
{
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
		toteCount = 0;
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
			transitionTable[i++] = {stopped, HALBot::no_update, nullptr, stopped};
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
			rotateDrive->rotateConstant = -1;
			pushContainer->pushConstant = -1;
			pushContainer->enableScaling = true;
			driveToZone->strafe = false;
			incrTote(); //We already have a tote

			transitionTable[i++] = {pushContainer, HALBot::timerExpired, nullptr, gettingTote};
			transitionTable[i++] = {gettingTote, HALBot::timerExpired, nullptr, forkGrab};
			transitionTable[i++] = {gettingTote, HALBot::eStop, nullptr, stopped};
			transitionTable[i++] = {forkGrab, HALBot::finish, nullptr, strafeLeft};
			transitionTable[i++] = {forkGrab, HALBot::nextTote, nullptr, pushContainer};
			transitionTable[i++] = {strafeLeft, HALBot::timerExpired, nullptr, rotateDrive};
			transitionTable[i++] = {rotateDrive, HALBot::timerExpired, nullptr, stopped};
			transitionTable[i++] = {stopped, HALBot::no_update, nullptr, stopped};
			defState = pushContainer;
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
			transitionTable[i++] = {rotateDrive, HALBot::timerExpired, nullptr, backAway};
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
