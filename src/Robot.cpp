#include "WPILib.h"
#include "SmartDashboard/SmartDashboard.h"
#include "MecanumDrive.h"
#include "Keycodes.h"
#include "lib/pugixml.hpp"


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
