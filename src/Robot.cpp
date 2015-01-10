#include "WPILib.h"
//#include "MecanumDrive.h"


class Robot: public IterativeRobot {
private:
	LiveWindow *lw;
	//MecanumDrive *drivebase;
	Talon* testMotor = NULL;
	Joystick* controller;

public:
	void RobotInit() {
		lw = LiveWindow::GetInstance();
		testMotor = new Talon(1);
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
	}

	void TeleopPeriodic() {
		testMotor->Set(controller->GetRawAxis(1));
	}

	void TestPeriodic() {
		//lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
