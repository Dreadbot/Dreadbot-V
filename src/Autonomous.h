#pragma once

#include <math.h>
#include "WPILib.h"
#include "Timer.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "FSM.h"

#define TOTE_PICKUP_TIME 2
#define DRIVE_TO_ZONE_TIME 4.1
#define ROTATE_TIME 0.66
#define LOWER_STACK_TIME 2
#define TOTE_COUNT 3
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
		virtual int update();
		void setHardware(MecanumDrive* newDrivebase);
		Timer driveTimer;
	protected:
		MecanumDrive* drivebase;
		bool timerActive;
	};
	class ForkGrab : public FSMState
	{
	public:
		ForkGrab();
		virtual int update();
		Timer grabTimer;
		PneumaticGrouping* lift;
	private:;
		bool timerActive;
	};
	class Rotate : public DriveToZone
	{
	public:
		int update();
	};
	class Stopped : public FSMState
	{
	public:
		int update();
		PneumaticGrouping* lift;
	};

	class HALBot
	{
	public:
		enum fsmInputs {no_update, finish, timerExpired, nextTote, sensorHit};

		HALBot();
		~HALBot();
		static int getToteCount();
		static void incrTote();
		void init(MecanumDrive* newDrivebase, MotorGrouping* newIntake, PneumaticGrouping* lift);
		void update();
	private:
		static int toteCount;
		FiniteStateMachine* fsm;
		MecanumDrive* drivebase;
		MotorGrouping* intake;

		FSMTransition transitionTable[6];
		GettingTote* gettingTote;
		DriveToZone* driveToZone;
		ForkGrab* forkGrab;
		Rotate* rotate;
		Stopped* stopped;
	};

	float getParallelTurnDir(Ultrasonic* frontUltra, Ultrasonic* rearUltra);
}
