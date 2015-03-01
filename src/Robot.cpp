#include <WPILib.h>
#include "SmartDashboard/SmartDashboard.h"
#include "DigitalInput.h"
#include "DreadbotDIO.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "Autonomous.h"

namespace dreadbot
{
	class Robot: public IterativeRobot
	{
	private:
		DriverStation *ds;
		Joystick* gamepad;
		PowerDistributionPanel *pdp;

	public:
		void RobotInit()
		{
			ds = DriverStation::GetInstance();
			SmartDashboard::init();
		//	lw = LiveWindow::GetInstance();
			pdp = new PowerDistributionPanel();
		}

		void GlobalInit()
		{
		}

		void AutonomousInit()
		{
		}

		void AutonomousPeriodic()
		{
		}

		void TeleopInit()
		{
		}

		void TeleopPeriodic()
		{

		}

		void TestInit()
		{
			GlobalInit();
		}

		void TestPeriodic()
		{
			//lw->Run();
			GetAutonMode();
			isToteInTransit();
			isAtStepHeight();
		}

		void DisabledInit()
		{
		}

		void DisabledPeriodic()
		{
		}
	};

}

START_ROBOT_CLASS(dreadbot::Robot);
