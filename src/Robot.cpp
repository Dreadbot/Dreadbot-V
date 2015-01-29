#include <WPILib.h>
#include "SmartDashboard/SmartDashboard.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "Vision.h"

namespace dreadbot {
	class Robot: public IterativeRobot {
	private:
		DriverStation *ds;
		LiveWindow *lw;
		Joystick* gamepad;
		Input::XMLInput* Input;
		AxisCamera* camera;
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
			Input = Input::XMLInput::getInstance();
			Input->setDrivebase(drivebase);
			camera = new AxisCamera;
		}

		void AutonomousInit() {
			drivebase->Engage();

		}

		void AutonomousPeriodic() {
			visionTest();
		}

		void TeleopInit() {
			Input->loadXMLConfig("/XML Bot Config.xml");
			drivebase->Engage();
		}

		void TeleopPeriodic() {
			drivebase->SD_RetrievePID();
			drivebase->SD_OutputDiagnostics();
		//	drivebase->Drive_v(gamepad->GetRawAxis(0), gamepad->GetRawAxis(1), gamepad->GetRawAxis(2));
			Input->updateDrivebase();
		}

		void TestPeriodic() {
			lw->Run();
		}
	};
}

START_ROBOT_CLASS(dreadbot::Robot);
