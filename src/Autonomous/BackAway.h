#pragma once

#include "ForkGrab.h"

namespace dreadbot
{
	class BackAway : public ForkGrab
	{
	public:
		virtual void enter();
		virtual int update();
	};
}
