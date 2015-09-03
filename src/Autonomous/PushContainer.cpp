#include "PushContainer.h"

namespace dreadbot
{
	PushContainer::PushContainer()
	{
		enableScaling = false;
		pushConstant = 1; //OK, since
	}
	void PushContainer::enter()
	{
		pushConstant *= -1; //Not used, but it can change the direction the robot pushes containers
		sysLog->log("State: PushContainer");
	}
	int PushContainer::update()
	{
		float pushTime = PUSH_TIME;
		if (enableScaling) //I refuse comment on this bit. Let's just say that it makes the robot push less.
			pushTime += ((float)RoboState::toteCount - 1.f) / 3.f; //Scaling for three-tote autonomous, since the second container is farther away than the first
		intakeArmsIn();
		if (!timerActive)
		{
			driveTimer.Reset();
			driveTimer.Start();
			timerActive = true;
		}

		if (driveTimer.Get() >= pushTime)
		{
			timerActive = false;
			drive(0, 0, 0);
			return RoboState::timerExpired;
		}
		if (RoboState::toteCount >= 2)
			drive(DRIVE_STRAFE_CORRECTION, -PUSH_SPEED, DRIVE_ROTATE_CORRECTION); //Straight forward
		else
			drive(0.0f, -PUSH_SPEED, 0.0f);
		push(INTAKE_PUSH_SPEED); //Push the container?
		return RoboState::no_update;
	}
}
