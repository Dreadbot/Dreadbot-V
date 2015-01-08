#include "WPILib.h"
#include "MecanumDrive.h"


class Robot: public IterativeRobot {
private:
	LiveWindow *lw;
	MecanumDrive *drivebase;

public:
	void RobotInit() {
		lw = LiveWindow::GetInstance();
	}

	void AutonomousInit() {
		drivebase->EnableControl();
	}

	void AutonomousPeriodic() {

	}

	void TeleopInit() {
		drivebase->DisableControl();
	}

	void TeleopPeriodic() {

	}

	void TestPeriodic() {
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
