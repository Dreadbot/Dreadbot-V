#include "WPILib.h"
#include "SmartDashboard/SmartDashboard.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include <fstream>

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
			drivebase = new MecanumDrive(0, 1, 2, 3);
			input = Input::XMLInput::getInstance();
			input->setDrivebase(drivebase);
			//input->loadXMLConfig("/XML Bot Config.xml");
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
			//input->updateDrivebase();

		}

		void TestPeriodic() {
			lw->Run();
		}
	};
}
START_ROBOT_CLASS(dreadbot::Robot);
