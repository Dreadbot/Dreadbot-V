#pragma once

#include <math.h>
#include "WPILib.h"
#include "Timer.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "FSM.h"
#include "DreadbotDIO.h"


//All timings
#define TOTE_GRAB_DELAY 1.05
#define STRAFE_TO_ZONE_TIME 3.3
#define DRIVE_TO_ZONE_TIME 2.7
#define PUSH_TIME 1
#define BACK_AWAY_TIME 1
#define ROTATE_TIME 1.8
#define LOWER_STACK_TIME 2.5
#define RAISE_STACK_TIME 2

#define DIST_FROM_WALL 2000 //Millimeters!
#define ULTRASONIC_SEPARATION 750 //Also millimeters!

namespace dreadbot 
{
	//States
	class GettingTote : public FSMState
	{
	public:
		GettingTote();
		virtual void enter();
		virtual int update();
		void setHardware(MecanumDrive* newDrivebase, MotorGrouping* newIntake);
	private:
		MecanumDrive* drivebase;
		MotorGrouping* intake;
		bool timerActive;
		Timer getTimer;
		Timer eStopTimer;
	};
	class DriveToZone : public FSMState
	{
	public:
		DriveToZone();
		virtual void enter();
		virtual int update();
		void setHardware(MecanumDrive* newDrivebase);
		bool strafe;
		Timer driveTimer;
	protected:
		MecanumDrive* drivebase;
		bool timerActive;
	};
	class ForkGrab : public FSMState
	{
	public:
		ForkGrab();
		virtual void enter();
		virtual int update();
		Timer grabTimer;
		PneumaticGrouping* lift;
		MecanumDrive* drivebase;
	protected:
		bool timerActive;
	};
	class Rotate : public DriveToZone
	{
	public:
		Rotate();
		virtual void enter();
		virtual int update();

		int rotateConstant;
	};
	class Stopped : public FSMState
	{
	public:
		virtual void enter();
		virtual int update();
		PneumaticGrouping* lift;
	};
	class PushContainer : public DriveToZone
	{
	public:
		virtual void enter();
		virtual int update();
		Talon* pusher1;
		Talon* pusher2;
		int pushConstant;
	};
	class BackAway : public ForkGrab
	{
	public:
		virtual void enter();
		virtual int update();
		MecanumDrive* drivebase;
	};

	class HALBot
	{
	public:
		enum fsmInputs {no_update, finish, timerExpired, nextTote, eStop};

		HALBot();
		~HALBot();
		static bool enoughTotes();
		static void incrTote();
		static int getToteCount();
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
