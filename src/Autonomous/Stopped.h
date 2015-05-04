#pragma once

#include "RoboState.h"

namespace dreadbot
{
	class Stopped : public RoboState
	{
	public:
		virtual void enter();
		virtual int update();
	};
}