#pragma once

#include "WPILib.h"
#include "Timer.h"
#include "../MecanumDrive.h"
#include "../XMLInput.h"
#include "FSM.h"
#include "../DreadbotDIO.h"
#include "../../lib/Logger.h"
#include "HALBot.h" //Ugh - needed for now
using namespace Hydra;

/*******************
*CALIBRATION VALUES*
*******************/

#define ESTOP_TIME 				5.0f 	// How long the robot will wait without getting a tote until it e-stops
#define STRAFE_TO_ZONE_TIME 	3.1f 	// Used in some auton modes (!2TA !3TA) - how long does the robot strafe?
#define DRIVE_TO_ZONE_TIME 		2.0f 	// How long the robot normally drives forward

// Container evacuation parameter
#define INTAKE_PUSH_SPEED		0.9f	// Rotational velocity of the intake wheels when pushing a container out of the way
#define PUSH_TIME 				1.0f 	// 0.9 How long the robot pushes containers out of the way
#define PUSH_SPEED 				0.7f 	// How fast the robot drives forward when pushing containers
//#define DRIVE_STRAFE_CORRECTION	0.05f 	// Aport drift abbertaion correctional factor
//#define DRIVE_ROTATE_CORRECTION	0.03f 	// Rotate abberation correctional factor
#define DRIVE_STRAFE_CORRECTION	0.0f 	// Aport drift abbertaion correctional factor
#define DRIVE_ROTATE_CORRECTION	0.0f 	// Rotate abberation correctional factor

// Endgame parameters
#define RD_DRIVE_SPEED 			1.0f 	// Astern speed in RotateDrive.
#define RD_ROTATE_SPEED 		-0.4f 	// Size of the RotateDrive arc
#define BACK_AWAY_TIME 			0.6f 	// How long the robot backs up after stopping in 3TA
#define ROTATE_TIME 			0.6f 	// Also, timing is modified in RotateDrive::update - 1.0 s is subtracted
#define ROTATE_DRIVE_STRAIGHT 	0.6f 	// How long to drive straight in RotateDrive AFTER rotating

// Tote collection parameters
#define STACK_CORRECTION_TIME 	0.25f 	// How long the robot jerks backward
#define STACK_CORRECTION_SPEED 	0.85f 	// How quickly the robot jerks backward
#define LIFT_ENGAGEMENT_DELAY 	0.5f 	// How long the robot waits after starting to lower the lift while collecting the third tote before reversing.

namespace dreadbot
{
	class RoboState : public FSMState
	{
		public:
			virtual void enter() = 0;
			virtual int update() = 0;
			virtual ~RoboState() {}

		protected:
			//Hardware for access for all states
			static MecanumDrive* drivebase;
			static MotorGrouping* intake;
			static PneumaticGrouping* lift;
			static PneumaticGrouping* liftArms;
			static PneumaticGrouping* intakeArms;
			static Talon* pusher1;
			static Talon* pusher2;
			static Log* sysLog;

			friend class HALBot; //Lets HALBot set values without need for getters/setters
	};
}
