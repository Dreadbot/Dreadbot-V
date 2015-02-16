#pragma once

#include "WPILib.h"
#include "Timer.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include <math.h>

#define TOTE_PICKUP_TIME 2.5
#define DRIVE_TO_ZONE_TIME 7.5
#define DIST_FROM_WALL 2000 //Millimeters!
#define ULTRASONIC_SEPARATION 750 //Also millimeters!

namespace dreadbot 
{
	enum robotState{stopped, getting_tote, drive_to_zone};

	float getParallelTurnDir(Ultrasonic* frontUltra, Ultrasonic* rearUltra);
};
