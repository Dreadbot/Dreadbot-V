#include "WPILib.h"
#include "SmartDashboard/SmartDashboard.h"
#include "MecanumDrive.h"
#include "XMLInput.h"

namespace dreadbot {
	class Robot: public IterativeRobot {
	private:
		DriverStation *ds;
		LiveWindow *lw;
		Joystick* gamepad;
		Input::XMLInput* input;

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
			input = Input::XMLInput::getInstance();
			input->setDrivebase(drivebase);
		}

		void AutonomousInit() {}

		void AutonomousPeriodic() {}

		void TeleopInit() {
			input->loadXMLConfig("/XML Bot Config.xml");
			drivebase->Engage();
		}

		void TeleopPeriodic() {
//			drivebase->SDGetPID();
//			drivebase->SDOutputDiagnostics();
//			drivebase->Drive(
//				gamepad->GetRawAxis(1),
//				gamepad->GetRawAxis(2),
//				gamepad->GetRawAxis(3));
			input->updateDrivebase();

		}

		void TestPeriodic() {
			lw->Run();
		}
	};
}
START_ROBOT_CLASS(dreadbot::Robot);
