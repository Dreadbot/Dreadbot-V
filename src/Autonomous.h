#pragma once

#include "WPILib.h"
#include "Timer.h"
#include "MecanumDrive.h"
#include "XMLInput.h"

#define TOTE_PICKUP_TIME 2.5
#define DRIVE_TO_ZONE_TIME 7.5

namespace dreadbot 
{
	enum robotState{start, stopped, getting_tote, drive_to_zone};

	class RobotFSM
	{
	public:
		RobotFSM();
		void SetHardware(MecanumDrive* base, MotorGrouping* newIntake, MotorGrouping* newTransit);
		void update();
	protected:
		void switchState(robotState newState);
		MecanumDrive* drivebase;
		Timer globalTimer;
		robotState state;

		MotorGrouping* intake;
		MotorGrouping* transit;
	};
};
