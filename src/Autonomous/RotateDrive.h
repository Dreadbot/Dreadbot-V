#pragma once

#include "Rotate.h"

namespace dreadbot
{
	class RotateDrive : public Rotate
	{
	public:
		RotateDrive();
		int update();
		void enter();
	};
}
