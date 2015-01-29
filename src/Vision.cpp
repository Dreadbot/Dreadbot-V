#include "Vision.h"

namespace dreadbot
{
	void visionTest(AxisCamera* camera)
	{
		ColorImage* image = NULL;
		camera->GetImage(image);
		SmartDashboard::PutData(image);
	}
}
