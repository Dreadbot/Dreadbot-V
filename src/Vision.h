#include <WPILib.h>

#define Y_IMAGE_RES 480
#define VIEW_ANGLE 49 //Axis M1013
#define PI 3.14159265358975

//Something about score limits for target identification
#define RECTANGULARITY 40 //These might be for 2014's code...
#define APECT_RATIO_LIMIT 55

#define AREA_MINIMIM 150 //Minimum area of particles to be considered
#define MAX_PARTICLES 6 //Max particles? What?!

namespace dreadbot
{
	void visionTest(AxisCamera* camera = NULL);
}
