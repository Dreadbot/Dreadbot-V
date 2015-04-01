#include "Hardware.h"

namespace dreadbot {
  MecanumDrive* Hardware::drivebase;
  MotorGrouping* Hardware::intake;
  PneumaticGrouping* Hardware::lift;
  PneumaticGrouping* Hardware::fork;
  PneumaticGrouping* Hardware::intakeArms;
}
