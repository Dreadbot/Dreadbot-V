#include "WPILib.h"
#include "DreadbotDIO.h"

namespace dreadbot {
	class Robot: public IterativeRobot
	{
		private:

		LiveWindow *lw;
		int teleop_counter = 0;
		int auton_counter = 0;
		int practice_counter = 0;
		int test_counter = 0;
		int disabled_counter = 0;

		void RobotInit()
		{
			//lw = LiveWindow::GetInstance();
			SmartDashboard::init();
		}

		void PutCounters(void)
		{
			SmartDashboard::PutNumber("TeleopPeriodic", teleop_counter);
			SmartDashboard::PutNumber("AutonPeriodic", auton_counter);
			SmartDashboard::PutNumber("PracticePeriodic", practice_counter);
			SmartDashboard::PutNumber("TestPeriodic", test_counter);
			SmartDashboard::PutNumber("DisabledPeriodic", disabled_counter);
		}

		void GlobalInit()
		{
		}

		void AutonomousInit()
		{
		}

		void AutonomousPeriodic()
		{
			auton_counter++;
			PutCounters();
		}

		void TeleopInit()
		{
		}

		void TeleopPeriodic()
		{
			teleop_counter++;
			PutCounters();
			dreadbot::GetAutonMode();
			dreadbot::isToteInTransit();
			dreadbot::isAtStepHeight();
		}

		void TestInit()
		{
		}

		void TestPeriodic()
		{
			test_counter++;
			PutCounters();

			//lw->Run();
		}

		void PracticeInit()
		{
		}

		void PracticePeriodic()
		{
			practice_counter++;
			PutCounters();
		}

		void DisabledInit()
		{
		}

		void DisabledPeriodic()
		{
			disabled_counter++;
			PutCounters();
		}

	};
}

START_ROBOT_CLASS(dreadbot::Robot);
