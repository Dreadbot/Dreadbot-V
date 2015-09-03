#include "RoboState.h"

namespace dreadbot
{
	//These are needed for static members... ugh. 1.5 hours.
	MecanumDrive* RoboState::drivebase = nullptr;
	MotorGrouping* RoboState::intakeWheels = nullptr;
	PneumaticGrouping* RoboState::lift = nullptr;
	PneumaticGrouping* RoboState::liftArms = nullptr;
	PneumaticGrouping* RoboState::intakeArms = nullptr;
	Talon* RoboState::pusher1 = nullptr;
	Talon* RoboState::pusher2 = nullptr;
	Log* RoboState::sysLog = nullptr;

	int RoboState::toteCount = 0;
	int RoboState::neededTCount = 0;

	void RoboState::drive(double x, double y, double r)
	{
		if (drivebase != nullptr)
			drivebase->Drive_v(x, y, r);
	}
	void RoboState::intake(double magnitude)
	{
		if (intakeWheels != nullptr)
			intakeWheels->Set(magnitude);
	}
	void RoboState::push(double magnitude)
	{
		if (pusher1 != nullptr)
			pusher1->Set(magnitude);
		if (pusher2 != nullptr)
			pusher2->Set(magnitude);
	}
	void RoboState::raiseLift()
	{
		if (lift != nullptr)
			lift->Set(1);
	}
	void RoboState::lowerLift()
	{
		if (lift != nullptr)
			lift->Set(-1);
	}
	void RoboState::stopLift()
	{
		if (lift != nullptr)
			lift->Set(0);
	}
	void RoboState::intakeArmsIn()
	{
		if (intakeArms != nullptr)
			intakeArms->Set(1);
	}
	void RoboState::intakeArmsOut()
	{
		if (intakeArms != nullptr)
			intakeArms->Set(-1);
	}
	void RoboState::intakeArmsLax()
	{
		if (intakeArms != nullptr)
			intakeArms->Set(0);
	}
	void RoboState::liftArmsIn()
	{
		if (liftArms != nullptr)
			liftArms->Set(0); //This should also be a single solenoid. Double (hehe) check.
	}
	void RoboState::liftArmsOut()
	{
		if (liftArms != nullptr)
			liftArms->Set(-1);
	}
}
