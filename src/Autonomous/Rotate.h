#pragma once

#include "DriveToZone.h"

namespace dreadbot
{
	class Rotate : public DriveToZone
	{
	public:
		Rotate();
		virtual void enter();
		virtual int update();

		int rotateConstant;
	};
}
