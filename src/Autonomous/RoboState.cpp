#include "RoboState.h"

namespace dreadbot
{
	//These are needed for static members... ugh. 1.5 hours.
	MecanumDrive* RoboState::drivebase = nullptr;
	MotorGrouping* RoboState::intake = nullptr;
	PneumaticGrouping* RoboState::lift = nullptr;
	PneumaticGrouping* RoboState::liftArms = nullptr;
	PneumaticGrouping* RoboState::intakeArms = nullptr;
	Talon* RoboState::pusher1 = nullptr;
	Talon* RoboState::pusher2 = nullptr;
	Log* RoboState::sysLog = nullptr;

	int RoboState::toteCount = 0;
	int RoboState::neededTCount = 0;
}
