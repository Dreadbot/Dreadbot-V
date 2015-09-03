#include "Stopped.h"

namespace dreadbot
{
	void Stopped::enter()
	{
		sysLog->log("State: Stopped");
		drive(0, 0, 0);
		if (RoboState::toteCount == 3)
			liftArmsIn(); //...should work, issues the same command?
	}
	int Stopped::update()
	{
		return RoboState::no_update; //Does nothing of significance.
	}
}
