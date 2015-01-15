#include "WPILib.h"
#include "SmartDashboard/SmartDashboard.h"
#include "MecanumDrive.h"
<<<<<<< HEAD
//#include "XMLInput.h"

namespace dreadbot {
	class Robot: public IterativeRobot {
	private:
		DriverStation *ds;
		LiveWindow *lw;
		Joystick* gamepad;

		PowerDistributionPanel *pdp;
		MecanumDrive *drivebase;

	public:
		void RobotInit() {
			ds = DriverStation::GetInstance();
			SmartDashboard::init();
			lw = LiveWindow::GetInstance();
			gamepad = new Joystick(0);

			pdp = new PowerDistributionPanel();
			drivebase = new MecanumDrive(0, 1, 2, 3);
		}

		void AutonomousInit() {}

		void AutonomousPeriodic() {}

		void TeleopInit() {
			drivebase->Engage();
		}

		void TeleopPeriodic() {
			drivebase->SDGetPID();
			drivebase->SDOutputDiagnostics();
			drivebase->Drive(
				gamepad->GetRawAxis(1),
				gamepad->GetRawAxis(2),
				gamepad->GetRawAxis(3));

		}

		void TestPeriodic() {
			lw->Run();
		}
	};
}
START_ROBOT_CLASS(dreadbot::Robot);
=======
#include "XMLInput.h"

class Robot: public IterativeRobot {
private:
	LiveWindow *lw;
	//MecanumDrive *drivebase;
	Talon* testMotor = NULL;
	Joystick* controller;

public:
	void RobotInit() {
		lw = LiveWindow::GetInstance();
		testMotor = new Talon(0);
		controller = new Joystick(1);
		//&GetWatchdog()->SetEnabled(false);
	}

	void AutonomousInit() {
		//drivebase->EnableControl();
	}

	void AutonomousPeriodic() {

	}

	void TeleopInit() {
		//drivebase->DisableControl();
		SmartDashboard::PutBoolean("Enabled: ", true);
	}

	void TeleopPeriodic() {
		SmartDashboard::PutNumber("GetRawAxis(1)", controller->GetRawAxis(1));
		testMotor->Set(controller->GetRawAxis(1));
	}

	void TestPeriodic() {
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
>>>>>>> origin/master
