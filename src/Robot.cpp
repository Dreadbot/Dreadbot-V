#include "WPILib.h"
#include "DreadbotDIO.h"

namespace dreadbot {
	class Robot: public IterativeRobot
	{
		private:

		LiveWindow *lw;
		int teleop_counter = 0;
		int auton_counter = 0;
		int test_counter = 0;
		int disabled_counter = 0;


		/*****************************************************************************
		**
		**  RobotInit() – provide for initialization at robot power-on
		**
		*****************************************************************************/
		void RobotInit()
		{
			lw = LiveWindow::GetInstance();
			SmartDashboard::init();
		}

		/*****************************************************************************
		**
		**  PutCounters
		**
		*****************************************************************************/
		void PutCounters(void)
		{
			SmartDashboard::PutNumber("TeleopPeriodic", teleop_counter);
			SmartDashboard::PutNumber("AutonPeriodic", auton_counter);
			SmartDashboard::PutNumber("TestPeriodic", test_counter);
			SmartDashboard::PutNumber("DisabledPeriodic", disabled_counter);
		}

		/*****************************************************************************
		**
		**  AutonomousInit() – called each and every time autonomous is entered from
		**      another mode
		**
		*****************************************************************************/
		void AutonomousInit()
		{
			auton_counter = 0;
			PutCounters();
		}

		void AutonomousPeriodic()
		{
			auton_counter++;
			PutCounters();
		}

		/*****************************************************************************
		**
		**  TeleopInit() – called each and every time teleop is entered from
		**      another mode
		**
		*****************************************************************************/
		void TeleopInit()
		{
			teleop_counter = 0;
			PutCounters();
		}

		void TeleopPeriodic()
		{
			teleop_counter++;
			PutCounters();
			dreadbot::GetAutonMode();
			dreadbot::isToteInTransit();
			dreadbot::isAtStepHeight();
		}

		/*****************************************************************************
		**
		**  TestInit() – called each and every time test is entered from
		**      another mode
		**
		*****************************************************************************/
		void TestInit()
		{
			test_counter = 0;
			PutCounters();
		}

		void TestPeriodic()
		{
			test_counter++;
			PutCounters();

			lw->Run();
		}

		/*****************************************************************************
		**
		**  DisabledInit() – called only when first disabled
		**
		*****************************************************************************/
		void DisabledInit()
		{
			disabled_counter = 0;
			PutCounters();
		}

		void DisabledPeriodic()
		{
			disabled_counter++;
			PutCounters();
		}

	};
}

START_ROBOT_CLASS(dreadbot::Robot);
