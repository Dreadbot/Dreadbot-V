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
		intakeArms->Set(1); //Intake arms in
		if (!timerActive)
		{
			driveTimer.Reset();
			driveTimer.Start();
			timerActive = true;
		}

		if (driveTimer.Get() >= pushTime)
		{
			timerActive = false;
			drivebase->Drive_v(0, 0, 0);
			return RoboState::timerExpired;
		}
		if (RoboState::toteCount >= 2) {
			drivebase->Drive_v(DRIVE_STRAFE_CORRECTION, -PUSH_SPEED, DRIVE_ROTATE_CORRECTION); //Straight forward
		} else {
			drivebase->Drive_v(0.0f, -PUSH_SPEED, 0.0f);
		}
		if (pusher1 != nullptr)
			pusher1->Set(INTAKE_PUSH_SPEED); //Push the container?
		if (pusher2 != nullptr)
			pusher2->Set(INTAKE_PUSH_SPEED);
		return RoboState::no_update;
	}
}
