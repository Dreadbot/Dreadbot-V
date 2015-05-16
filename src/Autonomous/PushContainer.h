#pragma once

#include "DriveToZone.h"

namespace dreadbot
{
	class PushContainer : public DriveToZone
	{
	public:
		PushContainer();
		virtual void enter();
		virtual int update();
		int pushConstant;
		bool enableScaling;
	};
}
