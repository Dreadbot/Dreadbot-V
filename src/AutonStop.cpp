#include "Autonomous.h"
#include "Robot.h"

namespace dreadbot
{

	void AutonStop::Stopped::enter()
	{
		sysLog->log("State: Stopped");
		if (drivebase != nullptr) {
			drivebase->Drive_v(0, 0, 0);
		}
	}

	int AutonStop::Stopped::update()
	{
		return HALBot::no_update; //Does nothing of significance.
	}

	AutonStop::AutonStop()
	{
		stopped = new AutonStop::Stopped;
		fsm = new FiniteStateMachine;
	}

	AutonStop::~AutonStop()
	{
		delete stopped;
		delete fsm;
	}

	void AutonStop::init(MecanumDrive* drivebase, MotorGrouping* intake, PneumaticGrouping* lift)
	{
		int i = 0;
		FSMState* defState = nullptr;

		transitionTable[i++] = {stopped, HALBot::no_update, nullptr, stopped};
		transitionTable[i++] = END_STATE_TABLE;
		defState = stopped;

		fsm->init(transitionTable, defState);
	}
}
