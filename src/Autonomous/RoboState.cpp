#include "RoboState.h"

namespace dreadbot
{
	//These are needed for static members... ugh. 1.5 hours.
	MecanumDrive* RoboState::drivebase;
	MotorGrouping* RoboState::intake;
	PneumaticGrouping* RoboState::lift;
	PneumaticGrouping* RoboState::liftArms;
	PneumaticGrouping* RoboState::intakeArms;
	Talon* RoboState::pusher1;
	Talon* RoboState::pusher2;
	Log* RoboState::sysLog;
}
