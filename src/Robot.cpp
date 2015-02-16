#include <WPILib.h>
#include "../lib/USBCamera.h"
#include "SmartDashboard/SmartDashboard.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "Vision.h"
#include "Autonomous.h"

namespace dreadbot {

#define CAMSWITCH_AXIS 6

	class Robot: public IterativeRobot {
	private:
		DriverStation *ds;
		LiveWindow *lw;
		Joystick* gamepad;
		PowerDistributionPanel *pdp;
		Compressor* compressor;

		//Ultrasonic *frontUltra;
		//Ultrasonic *rearUltra;

		XMLInput* Input;
		MecanumDrive *drivebase;
		RobotFSM* AutonBot;

		MotorGrouping* intake;
		MotorGrouping* transit;
		PneumaticGrouping* lift;
		PneumaticGrouping* intakeArms;

		int viewerCooldown;
		bool viewingBack;

//		//Vision stuff
//		Image* frame;
//		USBCamera* frontCam;
//		USBCamera* rearCam;

	public:
		void RobotInit()
		{
			ds = DriverStation::GetInstance();
			SmartDashboard::init();
			lw = LiveWindow::GetInstance();
			pdp = new PowerDistributionPanel();
			compressor = new Compressor(0);

			//frontUltra = new Ultrasonic(6, 7); //Dummy values for the ultrasonics
			//rearUltra = new Ultrasonic(4, 5);

			drivebase = new MecanumDrive(1, 2, 3, 4);
			Input = XMLInput::getInstance();
			Input->setDrivebase(drivebase);
			AutonBot = new RobotFSM;

			intake = NULL;
			transit = NULL;
			lift = NULL;
			intakeArms = NULL;

			//Vision stuff
//			viewingBack = false;
//			frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
//			frontCam = new USBCamera("cam0", false);
//			rearCam = new USBCamera("cam1", false);
		}

		void GlobalInit()
		{
			compressor->Start();
			drivebase->Engage();

			//frontUltra->SetAutomaticMode(true);
			//rearUltra->SetAutomaticMode(true);

			Input->loadXMLConfig("/XML Bot Config.xml");
			gamepad = Input->getController(0);
			drivebase->Engage();

			intake = Input->getMGroup("intake");
			transit = Input->getMGroup("transit");
			lift = Input->getPGroup("lift");
			intakeArms = Input->getPGroup("intakeArms");
		}

		void AutonomousInit()
		{
			GlobalInit();
			AutonBot->setHardware(drivebase, intake, transit);
			AutonBot->setUltras(0, 0); //Basically disables the ultrasonics
			AutonBot->start();
		}

		void AutonomousPeriodic()
		{
			AutonBot->update();
		}

		void TeleopInit()
		{
			GlobalInit();
		}

		void TeleopPeriodic() {
			drivebase->SD_RetrievePID();
			Input->updateDrivebase();
			//drivebase->SD_OutputDiagnostics();

//			//Vision Controls
//			if (viewerCooldown > 0)
//				viewerCooldown--;
//			if (gamepad->GetRawButton(5) && viewerCooldown == 0) //Left bumper
//			{
//				viewerCooldown = 30;
//				viewingBack = !viewingBack;
//			}
//
//			//Do actual vision switching stuff
//			if (viewingBack)
//				rearCam->GetImage(frame);
//			else
//				frontCam->GetImage(frame);
//			CameraServer::GetInstance()->SetImage(frame);

			//Output controls 34
			float intakeInput = gamepad->GetRawAxis(2) - gamepad->GetRawAxis(3); //Subtract left trigger from right trigger
			if (intake != NULL)
				intake->Set(intakeInput);
		
			float transitInput = (int)gamepad->GetRawButton(5); //Left bumper, transit intake
			transitInput += (int) gamepad->GetRawButton(6) * -1; //Right bumper, transit outtake
			if (transit != NULL)
				transit->Set(transitInput);

			float liftInput = (int)gamepad->GetRawButton(4); //Y Button
			liftInput += (int)gamepad->GetRawButton(1) * -1; //A button
			if (lift != NULL)
				lift->Set(liftInput);

			float armInput = (int)gamepad->GetRawButton(3); //X button
			armInput += (int)gamepad->GetRawButton(2) * -1; //B button
			if (intakeArms != NULL)
				intakeArms->Set(armInput);
		}

		void TestPeriodic() {
			lw->Run();
		}

		void DisabledInit()
		{
			compressor->Stop();
			drivebase->Disengage();

			//frontUltra->SetAutomaticMode(false);
			//rearUltra->SetAutomaticMode(false);
		}
	};
}

START_ROBOT_CLASS(dreadbot::Robot);
