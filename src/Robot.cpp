#include <WPILib.h>
//#include "../lib/USBCamera.h"
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

		int viewerCooldown;
		bool viewingBack;

		//AxisCamera* frontCamera;
		//USBCamera* rearCamera;
		//Image* frame;

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
			viewingBack = false;
			//frontCamera = new AxisCamera("10.36.56.11"); //cam0
			//rearCamera = new USBCamera("cam1", false); //cam1
			//frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
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
			
			if (viewerCooldown > 0)
				viewerCooldown--;
			if (gamepad->GetRawButton(5) && viewerCooldown == 0) //Left bumper
			{
				viewerCooldown = 30;
				viewingBack = !viewingBack;
				//DriverStation::ReportError("Switched viewingBack to: " + viewingBack);
			}

//			//Apply image
//			if (viewingBack)
//				frontCamera->GetImage(frame);
//			else
//				rearCamera->GetImage(frame);
//			CameraServer::GetInstance()->SetImage(frame);



			SmartDashboard::PutBoolean("vieiwngBack", viewingBack);
			SmartDashboard::PutNumber("viewerCooldown", viewerCooldown);
		}

		void TestPeriodic() {
			lw->Run();
		}
	};
}

START_ROBOT_CLASS(dreadbot::Robot);
