/*******************************************************************************
** 
**  Auton Modes
**
**  0.  Do Nothing
**      Just sit there. Ultimate fall-back if we're broken or alliance is bad.
**
**  1.  Drive
**      Just drive to auton zone.
**
**  2.  Tote
**      Grab tote, drive to auton zone.
**
**  3.  Container
**      Grab container, drive to auton zone.
**
**  4.  Tote and Container
**      Grab both tote and container, drive to auton zone.
**
**  5.  Tote 2-Stack
**      Grab 2 totes, stack them, drive to auton zone.
**
**  6.  Tote 3-Stack
**      Grab all 3 totes, stack them, drive to auton zone.
**
*******************************************************************************/

namespace dreadbot
{
	enum AutonMode {
		AUTON_MODE_STOP,	// 0.  Do Nothing
		AUTON_MODE_DRIVE,	// 1.  Drive
		AUTON_MODE_TOTE,	// 2.  Tote
		AUTON_MODE_CONTAINER,	// 3.  Container
		AUTON_MODE_BOTH,	// 4.  Tote and Container
		AUTON_MODE_STACK2,	// 5.  Tote 2-stack
		AUTON_MODE_STACK3,	// 6.  Tote 3-stack
	};
	enum AutonMode GetAutonMode(void);
	bool isToteInTransit(void);
	bool isLiftDown(void);
};

