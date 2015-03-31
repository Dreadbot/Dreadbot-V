#include "Hardware.h"


namespace dreadbot {
	MecanumDrive* RobotHardware::_drivebase;
	MotorGrouping* RobotHardware::_intake;
	PneumaticGrouping* RobotHardware::_lift;
	PneumaticGrouping* RobotHardware::_fork;
	PneumaticGrouping* RobotHardware::_intakeArms;
}
