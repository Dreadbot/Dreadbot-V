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

	}

	void AutonomousPeriodic() {

	}

	void TeleopInit() {

	}

	void TeleopPeriodic() {

	}

	void TestPeriodic() {
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
