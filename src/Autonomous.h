#pragma once

#include "WPILib.h"
#include "Timer.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include <math.h>

#define TOTE_PICKUP_TIME 2.5
#define DRIVE_TO_ZONE_TIME 7.5
#define DIST_FROM_WALL 2000 //Millimeters!
#define ULTRASONIC_SEPARATION 750 //Also millimeters!

namespace dreadbot 
{
	enum robotState{stopped, getting_tote, drive_to_zone};

	class RobotFSM
	{
	public:
		RobotFSM();
		void setHardware(MecanumDrive* base, MotorGrouping* newIntake, MotorGrouping* newTransit);
		void setUltras(Ultrasonic* newFrontUltra, Ultrasonic* newRearUltra);
		void start();
		void update();
	protected:
		void switchState(robotState newState);
		MecanumDrive* drivebase;
		Timer globalTimer;
		robotState state;

		MotorGrouping* intake;
		MotorGrouping* transit;
		Ultrasonic* frontUltra;
		Ultrasonic* rearUltra;
	};

	float getParallelTurnDir(Ultrasonic* frontUltra, Ultrasonic* rearUltra);
};
