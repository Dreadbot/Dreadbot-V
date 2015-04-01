#pragma once

#include <cmath>
#include "WPILib.h"
#include "Timer.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "FSM.h"
#include "DreadbotDIO.h"
#include "../lib/Logger.h"
#include "Hardware.h"

using namespace Hydra;

//All timings
#define STRAFE_TO_ZONE_TIME 3.1f
#define DRIVE_TO_ZONE_TIME 2.0f

#define PUSH_TIME 0.9
#define PUSH_SPEED 0.75

#define BACK_AWAY_TIME 4.0f
#define ROTATE_TIME 2.5f //Also, timing is modified in RotateDrive::update - 1.0 s is subtracted
#define ESTOP_TIME 6.0f
#define STACK_CORRECTION_TIME 0.35f

namespace dreadbot 
{
	//States
	class GettingTote : public FSMState
	{
	public:
		GettingTote();
		virtual void enter();
		virtual int update();
	private:
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
		Timer driveTimer;
		int dir;
		bool strafe;
	protected:
		bool timerActive;
	};
	class ForkGrab : public FSMState
	{
	public:
		ForkGrab();
		virtual void enter();
		virtual int update();
		Timer grabTimer;
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
	};
	class PushContainer : public DriveToZone
	{
	public:
		PushContainer();
		virtual void enter();
		virtual int update();
		Talon* pusher1;
		Talon* pusher2;
		int pushConstant;
		bool enableScaling;
	};
	class BackAway : public ForkGrab
	{
	public:
		virtual void enter();
		virtual int update();
	};
	class RotateDrive : public Rotate
	{
	public:
		RotateDrive();
		int update();
		void enter();
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
		AutonMode getMode();
		void init();
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
		RotateDrive* rotateDrive;
	};
}
