#include <WPILib.h>
#include "SmartDashboard/SmartDashboard.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "Vision.h"

namespace dreadbot {

#define CAMSWITCH_AXIS 6

	class Robot: public IterativeRobot {
	private:
		DriverStation *ds;
		LiveWindow *lw;
		Joystick* gamepad;
		XMLInput* Input;
		PowerDistributionPanel *pdp;
		MecanumDrive *drivebase;

		MotorGrouping* intake;

		//AxisCamera* frontCamera;
		//AxisCamera* backCamera;
		//bool viewingBack;

	public:
		void RobotInit() {
			ds = DriverStation::GetInstance();
			SmartDashboard::init();
			lw = LiveWindow::GetInstance();
			pdp = new PowerDistributionPanel();
			drivebase = new MecanumDrive(1, 2, 3, 4);
			Input = XMLInput::getInstance();
			Input->setDrivebase(drivebase);

			intake = NULL;

			//Vision stuff
			//frontCamera = new AxisCamera("10.36.56.11");
			//backCamera = new AxisCamera("10.0.0.11");
			//viewingBack = false;

		}

		void AutonomousInit() {
			drivebase->Engage();
		}

		void AutonomousPeriodic() {
		}

		void TeleopInit() {
			Input->loadXMLConfig("/XML Bot Config.xml");
			intake = Input->getMGroup("intake");
			gamepad = Input->getController(0);
			drivebase->Engage();
		}

		void TeleopPeriodic() {
			drivebase->SD_RetrievePID();
			Input->updateDrivebase();

			float intakeOutput = gamepad->GetRawAxis(2) - gamepad->GetRawAxis(3); //Subtract left trigger from right trigger
			intake->Set(intakeOutput);
			
//			if (gamepad->GetRawAxis(CAMSWITCH_AXIS) > 0) //Switch to front
//				viewingBack = false;
//			else if (gamepad->GetRawAxis(CAMSWITCH_AXIS) < 0)
//				viewingBack = false;
//
//			Apply the image to the camera server
//			Image* image = NULL;
//			image = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
//			if (viewingBack) //If the back camera is the currently active view...
//				backCamera->GetImage(image);
//			else
//				frontCamera->GetImage(image);
//			CameraServer::GetInstance()->SetImage(image);
//			delete image;
		}

		void TestPeriodic() {
			lw->Run();
		}
	};
}

START_ROBOT_CLASS(dreadbot::Robot);
