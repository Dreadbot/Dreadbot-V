#include <WPILib.h>
#include "SmartDashboard/SmartDashboard.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "Vision.h"
#include "Autonomous.h"

namespace dreadbot
{
	class Robot: public IterativeRobot
	{
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

			//Cam 2 is the rear camera
			viewingBack = false;
			StartCamera(1);
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

		void TeleopPeriodic()
		{
			drivebase->SD_RetrievePID();
			Input->updateDrivebase();
			//drivebase->SD_OutputDiagnostics();
			SmartDashboard::PutBoolean("viewingBack", viewingBack);

			//Vision switch control
			if (viewerCooldown > 0)
				viewerCooldown--;
			if (gamepad->GetRawButton(5) && viewerCooldown == 0) //Left bumper
			{
				SmartDashboard::PutBoolean("Switched camera", true);
				//Create cooldown and set the boolean thingy
				viewerCooldown = 50;
				viewingBack =! viewingBack;

				if (viewingBack)
				{
					//Rear camera: Camera 2
					StopCamera(1);
					StartCamera(2);
				}
				else
				{
					StopCamera(2);
					StartCamera(1);
				}
			}
			if (viewingBack)
			{
				IMAQdxGrab(sessionCam2, frame2, true, NULL);
				CameraServer::GetInstance()->SetImage(frame2);
			}
			else
			{
				IMAQdxGrab(sessionCam1, frame1, true, NULL);
				CameraServer::GetInstance()->SetImage(frame1);
			}

			//Output controls
			float intakeInput = gamepad->GetRawAxis(2) - gamepad->GetRawAxis(3); //Subtract left trigger from right trigger
			if (intake != NULL)
				intake->Set(intakeInput);
		
			bool liftInput = !gamepad->GetRawButton(5); //Left bumper
			if (lift != NULL)
				lift->Set(liftInput); //Keeps the lift up unless button is pressed.

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
			Input->zeroVels(); //Safety.

			//frontUltra->SetAutomaticMode(false);
			//rearUltra->SetAutomaticMode(false);
		}

		bool StopCamera(int cameraNum)
		{
			if (cameraNum == 1)
			{
				// stop image acquisition
				IMAQdxStopAcquisition(sessionCam1);
				//the camera name (ex "cam0") can be found through the roborio web interface
				imaqError = IMAQdxCloseCamera(sessionCam1);
				if (imaqError != IMAQdxErrorSuccess)
				{
					DriverStation::ReportError(
						"cam1 IMAQdxCloseCamera error: "
						+ std::to_string((long) imaqError) + "\n");
					return (false);
				}
			}
			else if (cameraNum == 2)
			{
				IMAQdxStopAcquisition(sessionCam2);
				imaqError = IMAQdxCloseCamera(sessionCam2);
				if (imaqError != IMAQdxErrorSuccess)
				{
					DriverStation::ReportError(
						"cam0 IMAQdxCloseCamera error: "
						+ std::to_string((long) imaqError) + "\n");
					return (false);
				}

			}
			return (false);
		}

		bool StartCamera(int cameraNum)
		{
			if (cameraNum == 1)
			{
				imaqError = IMAQdxOpenCamera("cam1",
					IMAQdxCameraControlModeController, &sessionCam1);
				if (imaqError != IMAQdxErrorSuccess)
				{
					DriverStation::ReportError(
						"cam1 IMAQdxOpenCamera error: "
						+ std::to_string((long) imaqError) + "\n");
					return (false);
				}
				imaqError = IMAQdxConfigureGrab(sessionCam1);
				if (imaqError != IMAQdxErrorSuccess)
				{
					DriverStation::ReportError(
						"cam0 IMAQdxConfigureGrab error: "
						+ std::to_string((long) imaqError) + "\n");
					return (false);
				}
				// acquire images
				IMAQdxStartAcquisition(sessionCam1);
			}
			else if (cameraNum == 2)
			{
				imaqError = IMAQdxOpenCamera("cam2",
					IMAQdxCameraControlModeController, &sessionCam2);
				if (imaqError != IMAQdxErrorSuccess)
				{
					DriverStation::ReportError(
						"cam0 IMAQdxOpenCamera error: "
						+ std::to_string((long) imaqError) + "\n");
					return (false);
				}
				imaqError = IMAQdxConfigureGrab(sessionCam2);
				if (imaqError != IMAQdxErrorSuccess)
				{
					DriverStation::ReportError(
						"cam0 IMAQdxConfigureGrab error: "
						+ std::to_string((long) imaqError) + "\n");
					return (false);
				}
				// acquire images
				IMAQdxStartAcquisition(sessionCam2);
			}
			return (true);
		}
	};
}

START_ROBOT_CLASS(dreadbot::Robot);
