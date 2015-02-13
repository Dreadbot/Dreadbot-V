#pragma once

#include "WPILib.h"
#include "Timer.h"
#include "MecanumDrive.h"
#include "XMLInput.h"

#define TOTE_PICKUP_TIME 2500
#define DRIVE_TO_ZONE_TIME 3000
#define null 0

namespace dreadbot 
{
	enum robotState{start, stopped, getting_tote, drive_to_zone};

	class RobotFSM
	{
	public:
		RobotFSM();
		~RobotFSM();
		void SetHardware(MecanumDrive* base, MotorGrouping* newIntake, MotorGrouping* newTransit);
		void update();
		void switchState(robotState newState);
	protected:
		MecanumDrive* drivebase;
		Timer globalTimer;
		robotState state;

		MotorGrouping* intake;
		MotorGrouping* transit;
	};
};
