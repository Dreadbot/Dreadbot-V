#pragma once

#include <math.h>
#include "WPILib.h"
#include "Timer.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "FSM.h"

#define TOTE_PICKUP_TIME 1
#define DRIVE_TO_ZONE_TIME 3
#define DIST_FROM_WALL 2000 //Millimeters!
#define ULTRASONIC_SEPARATION 750 //Also millimeters!

namespace dreadbot 
{
	float getParallelTurnDir(Ultrasonic* frontUltra, Ultrasonic* rearUltra);
}
