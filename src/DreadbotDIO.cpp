#include "SmartDashboard/SmartDashboard.h"
#include "DigitalInput.h"
#include "DreadbotDIO.h"

namespace dreadbot
{
	enum AutonMode GetAutonMode(void)
	{
		DigitalInput *bit0 = new DigitalInput(7);
		DigitalInput *bit1 = new DigitalInput(8);
		DigitalInput *bit2 = new DigitalInput(9);
		enum AutonMode mode = AUTON_MODE_STOP;
		int sw = 0;

		if (!bit0->Get()) { sw |= (0x01 << 0); }
		if (!bit1->Get()) { sw |= (0x01 << 1); }
		if (!bit2->Get()) { sw |= (0x01 << 2); }

		delete bit0;
		delete bit1;
		delete bit2;

		if      (sw == 0) { mode = AUTON_MODE_STOP;      }
		else if (sw == 1) { mode = AUTON_MODE_DRIVE;     }
		else if (sw == 2) { mode = AUTON_MODE_TOTE;      }
		else if (sw == 3) { mode = AUTON_MODE_CONTAINER; }
		else if (sw == 4) { mode = AUTON_MODE_BOTH;      }
		else if (sw == 5) { mode = AUTON_MODE_STACK2;    }
		else if (sw == 6) { mode = AUTON_MODE_STACK3;    }

		SmartDashboard::PutNumber("Auton Mode", mode);
		return mode;
	}

	bool isToteInTransit(void)
	{
		DigitalInput *trans_l = new DigitalInput(1);
		DigitalInput *trans_r = new DigitalInput(2);
		bool transit_left  = !trans_l->Get();
		bool transit_right = !trans_r->Get();
		bool in_transit = transit_left || transit_right;

		delete trans_l;
		delete trans_r;

		SmartDashboard::PutBoolean("Transit Left",    transit_left);
		SmartDashboard::PutBoolean("Transit Right",   transit_right);
		SmartDashboard::PutBoolean("Tote In Transit", in_transit);
		return in_transit;
	}

	bool isLiftDown(void)
	{
		DigitalInput *height = new DigitalInput(0);
		bool at_height = !height->Get();
		delete height;

		SmartDashboard::PutBoolean("Lift down", at_height);
		return at_height;
	}

	bool isPracticeBot(void)
	{
		DigitalInput *io = new DigitalInput(5);
		bool practice = !io->Get();
		delete io;

		SmartDashboard::PutBoolean("Practice Bot", practice);
		return practice;
	}

	bool isCompetitionBot(void)
	{
		return !isPracticeBot();
	}
};
