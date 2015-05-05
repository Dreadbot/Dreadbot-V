#pragma once

#include "../../lib/Logger.h"

//All states
#include "BackAway.h"
#include "DriveToZone.h"
#include "ForkGrab.h"
#include "GettingTote.h"
#include "PushContainer.h"
#include "Rotate.h"
#include "RotateDrive.h"
#include "Stopped.h"
#include "StrafeLeft.h"
#include "RoboState.h"

using namespace Hydra;

namespace dreadbot 
{
	//Needs to appear in https://en.wikipedia.org/wiki/Kludge#Computer_science
	class HALBot
	{
	public:
		HALBot();
		~HALBot();
		void setMode(AutonMode newMode); //Called during AutonomousInit. Determines what autonomous mode to run.
		AutonMode getMode(); //Gets the mode. Used in AutonomousInit. Also really, really dumb/stupid.
		void init(MecanumDrive* drivebase, MotorGrouping* intake, PneumaticGrouping* lift); //Sets hardware, intializes stuff, and prepares the transition tables. Assumes that the setMode thing has been used already.
		void update(); //Basically just a cheap call to FiniteStateMachine::update. 
	private:
		FiniteStateMachine* fsm;
		AutonMode mode; //The mode that the robot is in. Also dumb.
		Log* sysLog;

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
