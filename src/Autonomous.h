#pragma once

#include <math.h>
#include "WPILib.h"
#include "Timer.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "FSM.h"

#define TOTE_PICKUP_TIME 1
#define DRIVE_TO_ZONE_TIME 3
#define DIST_FROM_WALL 2000 //Millimeters!
#define ULTRASONIC_SEPARATION 750 //Also millimeters!

namespace dreadbot 
{
	//States
	class GettingTote : public FSMState
	{
	public:
		int update();
		Timer getTimer;
	};
	class DriveToZone : public FSMState
	{
	public:
		int update();
	private:
		Timer driveTimer;
	};
	class Stopped : public FSMState
	{
	public:
		int update();
	};

	class HALBot
	{
	public:
		enum fsmInputs {timerExpired, sensorHit};

		HALBot(MecanumDrive* newDrivebase, MotorGroup* newIntake)
		void update();
		void start();
		static void Drive(int x, int y, int r);
		static void Intake(int speed);
		static void startDriveTimer();
		static void startGetTimer();
	private:
		FiniteStateMachine* FSM;

		MecanumDrive* drivebase;
		MotorGroup* intake;

		FSMTransition[] transitionTable;
	};

	float getParallelTurnDir(Ultrasonic* frontUltra, Ultrasonic* rearUltra);
}
