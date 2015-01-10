#include "WPIlib.h"
#pragma once

#define DIRECTMODE_DMODE
#define DIRECTMODE_XMODE

#ifdef DIRECTMODE_DMODE
	#define D_BUTTON_A 2
	#define D_BUTTON_B 3
	#define D_BUTTON_X 1
	#define D_BUTTON_Y 4
	#define D_BUTTON_LB 5
	#define D_BUTTON_RB 6
	#define D_BUTTON_LT 7
	#define D_BUTTON_RT 8
	#define D_BUTTON_START 10
	#define D_BUTTON_BACK 9
	#define D_BUTTON_LJOY 11
	#define D_BUTTON_RJOY 12
	#define D_AXIS_LJOY_X 1
	#define D_AXIS_LJOY_Y 2
	#define D_AXIS_RJOY_X 3
	#define D_AXIS_RJOY_Y 4
	#define D_AXIS_DPAD_X 5
	#define D_AXIS_DPAD_Y 6
#endif

#ifdef DIRECTMODE_XMODE
	#define X_BUTTON_A 1
	#define X_BUTTON_B 2
	#define X_BUTTON_X 3
	#define X_BUTTON_Y 4
	#define X_BUTTON_LB 5
	#define X_BUTTON_RB 6
	#define X_BUTTON_START 8
	#define X_BUTTON_BACK 7
	#define X_BUTTON_LJOY 9	
	#define X_BUTTON_RJOY 10
	#define X_AXIS_TRIGGERS 3
	#define X_AXIS_LJOY_X 1
	#define X_AXIS_LJOY_Y 2
	#define X_AXIS_RJOY_X 4
	#define X_AXIS_RJOY_Y 5
	#define X_AXIS_DPAD_X 6
#endif
