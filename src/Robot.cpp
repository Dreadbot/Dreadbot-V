#include <WPILib.h>
#include "SmartDashboard/SmartDashboard.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "Vision.h"
#include "Autonomous.h"

namespace dreadbot {
	class Robot: public IterativeRobot {
	private:
		DriverStation *ds;
		Joystick* gamepad;
		PowerDistributionPanel *pdp;
		Compressor* compressor;

		//Ultrasonic *frontUltra;
		//Ultrasonic *rearUltra;

		XMLInput* Input;
		MecanumDrive *drivebase;
		RobotFSM* AutonBot;

		MotorGrouping* intake;
		PneumaticGrouping* lift;
		PneumaticGrouping* liftArms;
		PneumaticGrouping* intakeArms;

		int viewerCooldown;
		bool viewingBack;

		//Vision stuff - credit to team 116 for this!
		IMAQdxSession sessionCam1;
		IMAQdxSession sessionCam2;
		IMAQdxError imaqError;
		Image* frame1;
		Image* frame2;

	public:
		void RobotInit()
		{
			ds = DriverStation::GetInstance();
			SmartDashboard::init();
		//	lw = LiveWindow::GetInstance();
			pdp = new PowerDistributionPanel();
			compressor = new Compressor(0);

			//frontUltra = new Ultrasonic(6, 7); //Dummy values for the ultrasonics
			//rearUltra = new Ultrasonic(4, 5);

			drivebase = new MecanumDrive(1, 2, 3, 4);
			Input = XMLInput::getInstance();
			Input->setDrivebase(drivebase);
			AutonBot = new RobotFSM;

			intake = NULL;
			lift = NULL;
			liftArms = NULL;
			intakeArms = NULL;

			//Vision stuff
			frame1 = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
			frame2 = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
			viewingBack = false;
		}

		void GlobalInit()
		{
			//compressor->Start();
			drivebase->Engage();

			//frontUltra->SetAutomaticMode(true);
			//rearUltra->SetAutomaticMode(true);

			Input->loadXMLConfig("/XML Bot Config.xml");
			gamepad = Input->getController(0);
			drivebase->Engage();

			intake = Input->getMGroup("intake");
			lift = Input->getPGroup("lift");
			liftArms = Input->getPGroup("liftArms");
			intakeArms = Input->getPGroup("intakeArms");

			viewingBack = false;
		}

		void AutonomousInit()
		{
			GlobalInit();
			AutonBot->setHardware(drivebase, intake);
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
			SmartDashboard::PutBoolean("Button 5", gamepad->GetRawButton(5));
			SmartDashboard::PutBoolean("Button 3", gamepad->GetRawButton(3));
			SmartDashboard::PutBoolean("viewingBack", viewingBack);

			//Vision switch control
			if (viewerCooldown > 0)
				viewerCooldown--;
			if (gamepad->GetRawButton(3) && viewerCooldown == 0)
			{
				SmartDashboard::PutBoolean("Switched camera", true);
				//Create cooldown and set the boolean thingy
				viewerCooldown = 30;
				viewingBack =! viewingBack;
			}

			//Output controls
			float intakeInput = gamepad->GetRawAxis(2) - gamepad->GetRawAxis(3); //Subtract left trigger from right trigger
			if (intake != NULL)
				intake->Set(intakeInput);
		
			float liftInput = (int)gamepad->GetRawButton(4); //Y Button
			liftInput += (int)gamepad->GetRawButton(1) * -1; //A button
			if (lift != NULL)
				lift->Set(liftInput);

			float armInput = 0; /*(int)gamepad->GetRawButton(3);*/ //X button
			armInput += (int)gamepad->GetRawButton(2) * -1; //B button
			if (intakeArms != NULL)
				intakeArms->Set(armInput);

			float liftArmInput = gamepad->GetRawButton(6); //Right bumper
			if (liftArms != NULL)
					liftArms->Set(liftArmInput);
		}

		void TestInit()
		{
			GlobalInit();
		}

		void TestPeriodic()
		{
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
