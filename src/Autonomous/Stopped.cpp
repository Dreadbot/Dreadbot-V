#include "Stopped.h"

namespace dreadbot
{
	void Stopped::enter()
	{
		sysLog->log("State: Stopped");
		if (drivebase != nullptr)
			drivebase->Drive_v(0, 0, 0);
		if (RoboState::toteCount == 3)
			liftArms->Set(0); //Only lower the forks when the robot is in 3TA - it's not needed elsewhere
	}
	int Stopped::update()
	{
		return 0;//HALBot::no_update; //Does nothing of significance.
	}
}
