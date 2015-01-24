#include "WPILib.h"
#include "SmartDashboard/SmartDashboard.h"
#include "MecanumDrive.h"
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
			drivebase = new MecanumDrive(1, 2, 3, 4);
		}

		void AutonomousInit() {
			drivebase->Engage();

		}

		void AutonomousPeriodic() {}

		void TeleopInit() {
			drivebase->Engage();
		}

		void TeleopPeriodic() {
			drivebase->SD_RetrievePID();
			drivebase->SD_OutputDiagnostics();
			drivebase->Drive_v(gamepad->GetRawAxis(0), gamepad->GetRawAxis(1), gamepad->GetRawAxis(2));
		}

		void TestPeriodic() {
			lw->Run();
		}
	};
}

START_ROBOT_CLASS(dreadbot::Robot);
