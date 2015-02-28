#include "SmartDashboard/SmartDashboard.h"
#include "DigitalInput.h"
#include "AutonSetup.h"

namespace dreadbot
{
	int GetAutonMode(void)
	{
		enum AutonMode mode = AUTON_MODE_STOP;

		if (!DigitalInput(9).Get()) {
			mode = AUTON_MODE_STOP;
		}
		else if (!DigitalInput(8).Get()) {
			mode = AUTON_MODE_MOVE;
		}
		else if (!DigitalInput(7).Get()) {
			mode = AUTON_MODE_TOTE;
		}
		else if (!DigitalInput(6).Get()) {
			mode = AUTON_MODE_CONTAINER;
		}

		SmartDashboard::PutNumber("Auton Mode", mode);
		return mode;
	}
};
