#pragma once

#include <cmath>
#include "WPILib.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "Autonomous.h"
#include "Vision.h"
#include "DreadbotDIO.h"
#include "Hardware.h"

// Controller
#define LOGITECH_F310_X // Logitech F310 gamepad in X mode

// COM ports
#define COM_PRIMARY_DRIVER 0
#define COM_BACKUP_DRIVER 1

#define DRIVER_DEADZONE 0.05

#ifdef LOGITECH_F310_X
	// Primary driver controls
	#define AXS_DRIVE_X 0
	#define AXS_DRIVE_Y 1
	#define AXS_DRIVE_R 4
  	#define AXS_LIFT_DOWN 2 // LB
 	#define AXS_INTAKE_IN 3 // RB
	#define BTN_ARMS_OUT  6 // RT
 	#define BTN_OPEN_FORK 5 // LT
	#define BTN_STOP_LIFT 1 // X
	// Backup driver controls
	#define B_AXS_TOTE_OUT 3
	#define B_AXS_TOTE_IN 2
	#define B_BTN_ARMS_OUT 3
	#define B_BTN_ARMS_IN 2
	#define B_BTN_GEDDAN 5 // Robot does a short dance to recover from an intake failure
	// Shared controls
	#define BTN_SWITCH_CAM 8 // Start
#endif
