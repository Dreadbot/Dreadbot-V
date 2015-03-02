// Controller macro definitions

#pragma once


#define SAITEK_X51 0 // Joystick on COM0
#define LOGITECH_F310_X 1 // F310 in X-mode on COM1
#define LOGITECH_F310_D 2 // F310 in D-mode on COM2


// Define the type of controller used for each role
#define COM_PRIMARY_DRIVER SAITEK_X51
#define COM_BACKUP_DRIVER LOGITECH_F310_X


// Primary driver uses joysticks
#if COM_PRIMARY_DRIVER == SAITEK_X51
	#define AXS_DRIVE_X 1 // Forward-backward control on joystick X
	#define AXS_DRIVE_Y 2 // Right-left control on joystick Y
	#define AXS_DRIVE_R 5 // Rotation on joystick's twist axis
  	#define AXS_ARMS_IN 6 // POV hat y-axis up makes arms open
 	#define AXS_ARMS_OUT 7 // POV hat y-axis down makes arms close
	#define BTN_SWITCH_CAM 3 // Camera change is fire button
	#define BTN_INTAKE_FWD 1 // Intake is first stage of top trigger
	#define BTN_INTAKE_REV 2 // Outtake is bottom trigger
 	#define BTN_OPEN_SKI 4 // Lift arm out is the flat-black button on the joystick
 	#define BTN_LIFT_PICKUP 5 // Manual life override
 	#define BTN_LIFT_COOP 6 // Lift transits to co-op position
 	#define BTN_DEBUG 7 // Toggles debug output moded
	// The Saitek doesn't require input squaring

// Primary driver uses a gamepad in X-mode
#elif COM_PRIMARY_DRIVER == LOGITECH_F310_X
	#define AXS_DRIVE_X 1 // Forward-backward control on left thumbstick
	#define AXS_DRIVE_Y 2 // Right-left control on left thumbstick
	#define AXS_DRIVE_R 5 // Rotation on joystick's twist axis
  	#define AXS_ARMS_IN 6 // POV hat y-axis up makes arms open
 	#define AXS_ARMS_OUT 7 // POV hat y-axis down makes arms close
	#define BTN_SWITCH_CAM 3 // Camera change is fire button
	#define BTN_INTAKE_FWD 1 // Intake is first stage of top trigger
	#define BTN_INTAKE_REV 2 // Outtake is bottom trigger
 	#define BTN_OPEN_SKI 4 // Lift arm out is the flat-black button on the joystick
 	#define BTN_LIFT_PICKUP 5 // Manually overrides the lift to travel to collection position
 	#define BTN_LIFT_COOP 6 // Lift transits to co-op position
 	#define BTN_DEBUG 7 // Toggles debug output mode

	#define SQUARE_INPUTS
#endif

// Backup driver uses a gamepad in X-mode
#if COM_BACKUP_DRIVER == LOGITECH_F310_X
	#define BTN_AUTOBOT_ON 1 // Turns override mode on
	#define BTN_AUTOBOT_OFF 2 // Turns override mode off
	#define BTN_TOTE_OUT 3 // In override mode, spin intake wheels backward
	#define BTN_TOTE_IN 4 // In override mode, spin intake wheels forward.
	#define BTN_VIBRATE 5 // Robot performs a quick maneuver to shake off loose totes
	#define BTN_SWITCH_CAM_DRV2 3 // Cam switch can be done by the backup driver
#endif
