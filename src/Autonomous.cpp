#include "Autonomous.h"

namespace dreadbot
{
	RobotFSM::RobotFSM()
	{
		drivebase = null;
	}
	void RobotFSM::update()
	{
		if (state == stopped)
		{
			globalTimer.Start();
			state = getting_tote;
		}

		if (state == getting_tote)
		{
			//Has the tote already been picked up?
			//if (globalTimer.HasPeriodPassed(TOTE_PICKUP_TIME))
				//switchState(drive_to_zone);

			drivebase->Drive_v(0, .75, 0); //Drive forward at 3/4 speed

		}

		else if (state == drive_to_zone)
		{

		}
	}
};
