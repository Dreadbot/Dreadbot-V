#pragma once

#include <cmath>
#include "WPILib.h"
#include "Timer.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "FSM.h"
#include "DreadbotDIO.h"
#include "../lib/Logger.h"
using namespace Hydra;

#define ESTOP_TIME 				5.0f 	// How long the robot will wait without getting a tote until it e-stops
#define STRAFE_TO_ZONE_TIME 	3.1f 	// Used in some auton modes (!2TA !3TA) - how long does the robot strafe?
#define DRIVE_TO_ZONE_TIME 		2.0f 	// How long the robot normally drives forward

// Container evacuation parameter
#define INTAKE_PUSH_SPEED		1.0f	// Rotational velocity of the intake wheels when pushing a container out of the way
#define PUSH_TIME 				0.9f 	// How long the robot pushes containers out of the way
#define PUSH_SPEED 				0.75f 	// How fast the robot drives forward when pushing containers
#define DRIVE_STRAFE_CORRECTION	0.04f 	// Aport drift abbertaion correctional factor
#define DRIVE_ROTATE_CORRECTION	0.0f 	// Rotate abberation correctional factor

// Endgame parameters
#define RD_DRIVE_SPEED 			1.0f 	// Astern speed in RotateDrive.
#define RD_ROTATE_SPEED 		-0.4f 	// Size of the RotateDrive arc
#define BACK_AWAY_TIME 			0.6f 	// How long the robot backs up after stopping in 3TA
#define ROTATE_TIME 			0.8f 	// Also, timing is modified in RotateDrive::update - 1.0 s is subtracted
#define ROTATE_DRIVE_STRAIGHT 	0.2f 	// How long to drive straight in RotateDrive AFTER rotating

// Tote collection parameters
#define STACK_CORRECTION_TIME 	0.25f 	// How long the robot jerks backward
#define STACK_CORRECTION_SPEED 	0.85f 	// How quickly the robot jerks backward
#define LIFT_ENGAGEMENT_DELAY 	0.5f 	// How long the robot waits after starting to lower the lift while collecting the third tote before reversing.


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
		Timer *armTimer;
	};
	class DriveToZone : public FSMState
	{
	public:
		DriveToZone();
		virtual void enter();
		virtual int update();
		void setHardware(MecanumDrive* newDrivebase);
		Timer driveTimer;
		int dir;
		bool strafe;
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
		MecanumDrive* drivebase;
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
		MecanumDrive* drivebase;
	};
	class RotateDrive : public Rotate
	{
	public:
		RotateDrive();
		int update();
		void enter();
	};
	class StrafeLeft : public DriveToZone
	{
	public:
		StrafeLeft();
		int update();
		void enter();
	};

	//Needs to appear in https://en.wikipedia.org/wiki/Kludge#Computer_science
	class HALBot
	{
	public:
		enum fsmInputs {no_update, finish, timerExpired, nextTote, eStop};

		HALBot();
		~HALBot();
		static bool enoughTotes(); //Stupid global way of determining if the robot has acquired enough totes. Kludgish, really.
		static void incrTote(); //Increment the collected tote cout.
		static int getToteCount(); //Gets the tote count. Used in a few kludgish areas. Also stupid.
		void setMode(AutonMode newMode); //Called during AutonomousInit. Determines what autonomous mode to run.
		AutonMode getMode(); //Gets the mode. Used in AutonomousInit. Also really, really dumb/stupid.
		void init(MecanumDrive* drivebase, MotorGrouping* intake, PneumaticGrouping* lift); //Sets hardware, intializes stuff, and prepares the transition tables. Assumes that the setMode thing has been used already.
		void update(); //Basically just a cheap call to FiniteStateMachine::update. 
	private:
		static int toteCount; //How many totes have been acquired. 
		FiniteStateMachine* fsm;
		static AutonMode mode; //The mode that the robot is in. Also dumb.

		FSMTransition transitionTable[15]; //The transition table used for transitioning. Changes based on the setMode thingy.

		//State objects. These should be self-explanatory.
		GettingTote* gettingTote; 
		DriveToZone* driveToZone;
		ForkGrab* forkGrab;
		Rotate* rotate;
		Rotate* rotate2;
		Stopped* stopped;
		PushContainer* pushContainer;
		BackAway* backAway;
		RotateDrive* rotateDrive;
		StrafeLeft* strafeLeft;
	};
}
