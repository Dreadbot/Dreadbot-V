#include <WPILib.h>
//#include "../lib/USBCamera.h"
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

		XMLInput* Input;
		MecanumDrive *drivebase;
		RobotFSM* AutonBot;

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
		void RobotInit()
		{
			ds = DriverStation::GetInstance();
			SmartDashboard::init();
			lw = LiveWindow::GetInstance();
			pdp = new PowerDistributionPanel();
			compressor = new Compressor(0);

			drivebase = new MecanumDrive(1, 2, 3, 4);
			Input = XMLInput::getInstance();
			Input->setDrivebase(drivebase);
			AutonBot = new RobotFSM;

			intake = NULL;
			transit = NULL;
			lift = NULL;
			intakeArms = NULL;

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

		void AutonomousInit()
		{
			GlobalInit();
			AutonBot->SetHardware(drivebase, intake, transit);
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
			float intakeInput = gamepad->GetRawAxis(2) - gamepad->GetRawAxis(3); //Subtract left trigger from right trigger
			intake->Set(intakeInput);
			
//			float transitInput = (int)gamepad->GetRawButton(6); //Right bumper, transit intake
//			transitInput += (int) gamepad->GetRawButton(5) * -1; //Left bumper, transit outtake
			transit->Set(-1 * intakeInput);

			float liftInput = (int)gamepad->GetRawButton(4); //Y Button
			liftInput += (int)gamepad->GetRawButton(1) * -1; //A button
			lift->Set(liftInput);

			float armInput = (int)gamepad->GetRawButton(3); //X button
			armInput += (int)gamepad->GetRawButton(2) * -1; //B button
			intakeArms->Set(armInput);
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
