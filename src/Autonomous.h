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
		GettingTote();
		int update();
		void setHardware(MecanumDrive* newDrivebase, MotorGrouping* newIntake);
		Timer getTimer;
	private:
		MecanumDrive* drivebase;
		MotorGrouping* intake;
		bool timerActive;
	};
	class DriveToZone : public FSMState
	{
	public:
		DriveToZone();
		int update();
		void setHardware(MecanumDrive* newDrivebase);
		Timer driveTimer;
	private:
		MecanumDrive* drivebase;
		bool timerActive;
	};
	class Stopped : public FSMState
	{
	public:
		int update();
	};

	class HALBot
	{
	public:
		enum fsmInputs {no_update, timerExpired, sensorHit};

		HALBot();
		~HALBot();
		void init(MecanumDrive* newDrivebase, MotorGrouping* newIntake);
		void update();
	private:
		FiniteStateMachine* fsm;
		MecanumDrive* drivebase;
		MotorGrouping* intake;

		FSMTransition transitionTable[3];
		GettingTote* gettingTote;
		DriveToZone* driveToZone;
		Stopped* stopped;
	};

	float getParallelTurnDir(Ultrasonic* frontUltra, Ultrasonic* rearUltra);
}
