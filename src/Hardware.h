#pragma once

#include "WPILib.h"
#include "XMLInput.h"


/* Static class containing robot hardware.
 * Initialization is done within Robot.cpp. This class only serves as a freely-accessible container.
 */


#define drivebase 	RobotHardware::_drivebase
#define intake		RobotHardware::_intake
#define lift		RobotHardware::_lift
#define fork		RobotHardware::_fork
#define intakeArms	RobotHardware::_intakeArms

namespace dreadbot {
	struct RobotHardware {
		static MecanumDrive* _drivebase;
		static MotorGrouping* _intake;
		static PneumaticGrouping* _lift;
		static PneumaticGrouping* _fork;
		static PneumaticGrouping* _intakeArms;
	};
}
