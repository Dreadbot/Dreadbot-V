#include <WPILib.h>
#include "SmartDashboard/SmartDashboard.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "Vision.h"
#include "rps.h"

namespace dreadbot {
	class Robot: public IterativeRobot {
	private:
		DriverStation *ds;
		LiveWindow *lw;
		Joystick* gamepad;
		Input::XMLInput* Input;
		PowerDistributionPanel *pdp;
		MecanumDrive *drivebase;
		rps *positioner;

		AxisCamera* camera;
		Image* image;

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
			positioner = new rps(0);
			//Vision stuff
			camera = new AxisCamera("10.36.56.11");
			image = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
		}

		void AutonomousInit() {
			drivebase->Engage();

		}

		void AutonomousPeriodic() {
			camera->GetImage(image);
			imaqDrawShapeOnImage(image, image, { 10, 10, 100, 100 }, DrawMode::IMAQ_DRAW_VALUE, ShapeMode::IMAQ_SHAPE_OVAL, 0.0f);
			CameraServer::GetInstance()->SetImage(image);
		}

		void TeleopInit() {
			Input->loadXMLConfig("/XML Bot Config.xml");
			drivebase->Engage();
			positioner->Start();
		}

		void TeleopPeriodic() {
			drivebase->SD_RetrievePID();
			drivebase->SD_OutputDiagnostics();
		//	drivebase->Drive_v(gamepad->GetRawAxis(0), gamepad->GetRawAxis(1), gamepad->GetRawAxis(2));
			Input->updateDrivebase();
			Input->updateInds();
			inertial_frame thisCoord = positioner->GetTrackingData();
			SmartDashboard::PutNumber("Pos x", thisCoord.position.x);
			SmartDashboard::PutNumber("Pos y", thisCoord.position.y);
			SmartDashboard::PutNumber("Vel x", thisCoord.velocity.x);
			SmartDashboard::PutNumber("Vel y", thisCoord.velocity.y);
			SmartDashboard::PutNumber("Rot p", thisCoord.rp);
			SmartDashboard::PutNumber("Rot v", thisCoord.rv);
		}

		void TestPeriodic() {
			lw->Run();
		}
	};
}

START_ROBOT_CLASS(dreadbot::Robot);
