#pragma once

#include <cmath>
#include "WPILib.h"
#include "Timer.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "FSM.h"
#include "DreadbotDIO.h"
#include "Vision.h"
#include "Hardware.h"

//All timings
#define STRAFE_TO_ZONE_TIME 3.1
#define DRIVE_TO_ZONE_TIME 2.0

// PUSH_TIME*PUSH_SPEED ~= 0.7
#define PUSH_TIME 0.71
#define PUSH_SPEED 1.0

#define BACK_AWAY_TIME 1.0
#define ROTATE_TIME 2.0
#define ESTOP_TIME 6.0
#define STACK_CORRECTION_TIME 0.35
#define CONTAINER_SPIN_SPEED 1.0


//#define AIMBOT // Enables vision-based guiding to assist in tote collection.
#define MAX_ALIGN_ERROR 0.1 // How off-target a tote can be before course correction kicks in
//#define GAIN_KP 0.024 // For proportional control, Kp = 0.5/21 would probably be in the ballpark. I think.
#define CORRECTION_GAIN 0.5 // How fast the robot should strafe regardless of error. Different approach than KP

// @todo Document each state
namespace dreadbot {
	//
	struct GettingTote : public FSMState {
		GettingTote();
		virtual void enter();
		virtual int update();
		bool timerActive;
		Timer getTimer;
		Timer eStopTimer;
	};
	//
	struct DriveToZone : public FSMState {
		DriveToZone();
		virtual void enter();
		virtual int update();
		Timer driveTimer;
		int dir;
		bool strafe;
		bool timerActive;
	};
	//
	struct ForkGrab : public FSMState {
		ForkGrab();
		virtual void enter();
		virtual int update();
		Timer grabTimer;
		bool timerActive;
	};
	//
	struct Rotate : public DriveToZone {
		Rotate();
		virtual void enter();
		virtual int update();
		int rotateConstant;
	};
	//
	struct Stopped : public FSMState {
		virtual void enter();
		virtual int update();
	};
	//
	struct PushContainer : public DriveToZone {
		PushContainer();
		virtual void enter();
		virtual int update();
		Talon* pusher1;
		Talon* pusher2;
		int pushConstant = 1;
		bool enableScaling = false;
	};
	// Used in: 3 tote stack
	struct BackAway : public ForkGrab {
		virtual void enter();
		virtual int update();
	};
	// Used in:
	struct RotateDrive : public Rotate {
		RotateDrive();
		int update();
		void enter();
	};

	class HALBot {
	public:
		enum fsmInputs {no_update, finish, timerExpired, nextTote, eStop};

		HALBot();
		~HALBot();
		static bool enoughTotes();
		static void incrTote();
		static int getToteCount();
		void setMode(AutonMode newMode);
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
