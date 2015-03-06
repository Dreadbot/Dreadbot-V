#pragma once

#include <math.h>
#include "WPILib.h"
#include "Timer.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "FSM.h"
#include "DreadbotDIO.h"

#define TOTE_GRAB_DELAY 1.0
#define DRIVE_TO_ZONE_TIME 4.1
#define PUSH_TIME 1.5
#define BACK_AWAY_TIME 1
#define ROTATE_TIME 0.66
#define LOWER_STACK_TIME 2
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
	protected:
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
	class PushContainer : public DriveToZone
	{
	public:
		int update();
		SimpleMotor* pusher1;
		SimpleMotor* pusher2;
	};
	class BackAway : public ForkGrab
	{
	public:
		int update();
		MecanumDrive* drivebase;
	};

	class HALBot
	{
	public:
		enum fsmInputs {no_update, finish, timerExpired, nextTote, sensorHit};

		HALBot();
		~HALBot();
		static bool enoughTotes();
		static void incrTote();
		void setMode(AutonMode newMode);
		void init(MecanumDrive* drivebase, MotorGrouping* intake, PneumaticGrouping* lift);
		void update();
	private:
		static int toteCount;
		FiniteStateMachine* fsm;
		static AutonMode mode;

		FSMTransition transitionTable[15];
		GettingTote* gettingTote;
		DriveToZone* driveToZone;
		ForkGrab* forkGrab;
		Rotate* rotate;
		Rotate* rotate2;
		Stopped* stopped;
		PushContainer* pushContainer;
		BackAway* backAway;
	};

	float getParallelTurnDir(Ultrasonic* frontUltra, Ultrasonic* rearUltra);
}
