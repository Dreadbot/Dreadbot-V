#include <WPILib.h>
//#include "../lib/USBCamera.h"
#include "SmartDashboard/SmartDashboard.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "Vision.h"
#include "rps.h"

namespace dreadbot {

#define CAMSWITCH_AXIS 6

	class Robot: public IterativeRobot {
	private:
		DriverStation *ds;
		LiveWindow *lw;
		Joystick* gamepad;
		PowerDistributionPanel *pdp;
		Compressor* compressor;

		XMLInput* Input;
		MecanumDrive *drivebase;
		rps *positioner;

		MotorGrouping* intake;
		MotorGrouping* transit;
		PneumaticGrouping* lift;
		PneumaticGrouping* intakeArms;

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
			compressor = new Compressor(); //Uses default of 0? Or something? What?

			drivebase = new MecanumDrive(1, 2, 3, 4);
			Input = XMLInput::getInstance();
			Input->setDrivebase(drivebase);

			intake = NULL;
			transit = NULL;
			lift = NULL;
			intakeArms = NULL;
			positioner = new rps(0);

			//Vision stuff
			viewingBack = false;
			//frontCamera = new AxisCamera("10.36.56.11"); //cam0
			//rearCamera = new USBCamera("cam1", false); //cam1
			//frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
		}

		void GlobalInit()
		{
			compressor->Start();
			drivebase->Engage();

			Input->loadXMLConfig("/XML Bot Config.xml");
			gamepad = Input->getController(0);
			drivebase->Engage();

			intake = Input->getMGroup("intake");
			transit = Input->getMGroup("transit");
			lift = Input->getPGroup("lift");
			intakeArms = Input->getPGroup("intakeArms");
		}

		void AutonomousInit() {
			GlobalInit();
		}

		void AutonomousPeriodic() {
		}

		void TeleopInit() 
		{
			GlobalInit();
			Input->loadXMLConfig("/XML Bot Config.xml");
			drivebase->Engage();
			positioner->Start();
		}

		void TeleopPeriodic() {
			drivebase->SD_RetrievePID();
			Input->updateDrivebase();
			drivebase->SD_OutputDiagnostics();

//			if (viewerCooldown > 0)
//				viewerCooldown--;
//			if (gamepad->GetRawButton(5) && viewerCooldown == 0) //Left bumper
//			{
//				viewerCooldown = 30;
//				viewingBack = !viewingBack;
//			}
//

			//Output controls
			//Intake arm motors
			float intakeOutput = gamepad->GetRawAxis(2) - gamepad->GetRawAxis(3); //Subtract left trigger from right trigger
			intake->Set(intakeOutput);
			
			float transitInput = (int)gamepad->GetRawButton(5); //Left bumper, transit intake?
			transitInput += (int) gamepad->GetRawButton(6) * -1; //Right bumper, transit outtake?
			transit->Set(transitInput);

			float liftInput = (int)gamepad->GetRawButton(4); //Y Button
			liftInput += (int)gamepad->GetRawButton(1) * -1; //A button
			lift->Set(liftInput);

			float armInput = (int)gamepad->GetRawButton(3); //X button
			armInput += (int)gamepad->GetRawButton(2) * -1; //B button
			intakeArms->Set(armInput);

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

		void DisabledInit()
		{
			compressor->Stop();
			drivebase->Disengage();
		}
	};
}

START_ROBOT_CLASS(dreadbot::Robot);
