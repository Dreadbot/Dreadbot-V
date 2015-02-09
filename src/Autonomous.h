#pragma once

#include "WPILib.h"
#include "MecanumDrive.h"

namespace dreadbot 
{
	enum robotState{};

	class RobotFSM
	{
	public:
		RobotSFM(MecanumDrive* base);
		~RobotSFM();
		void update()
	protected:
		MecanumDrive* drivebase;
	};
};
