#include <WPILib.h>
#include "SmartDashboard/SmartDashboard.h"
#include "DigitalInput.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
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

		MotorGrouping* intake;
		PneumaticGrouping* lift;
		PneumaticGrouping* liftArms;
		PneumaticGrouping* intakeArms;

		HALBot* AutonBot;

		//Vision stuff - credit to team 116 for this!
		IMAQdxSession sessionCam1;
		IMAQdxSession sessionCam2;
		IMAQdxError imaqError;
		Image* frame1;
		Image* frame2;
		bool Cam1Enabled;
		bool Cam2Enabled;
		bool viewingBack;
		int viewerCooldown;

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
			AutonBot = new HALBot;

			intake = nullptr;
			lift = nullptr;
			liftArms = nullptr;
			intakeArms = nullptr;

			//Vision stuff
			frame1 = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
			frame2 = imaqCreateImage(IMAQ_IMAGE_RGB, 0);

			//Cam 2 is the rear camera
			viewingBack = false;
			Cam2Enabled = false;
			Cam1Enabled = StartCamera(1);
		}

		void GlobalInit()
		{
			compressor->Start();
			drivebase->Engage();

			//frontUltra->SetAutomaticMode(true);
			//rearUltra->SetAutomaticMode(true);

			Input->loadXMLConfig();
			gamepad = Input->getController(0);
			drivebase->Engage();

			intake = Input->getMGroup("intake");
			lift = Input->getPGroup("lift");
			liftArms = Input->getPGroup("liftArms");
			intakeArms = Input->getPGroup("intakeArms");

			AutonBot->init(drivebase, intake);
		}

		void AutonomousInit()
		{
			GlobalInit();
			//AutonBot->start();
		}

		void AutonomousPeriodic()
		{
			AutonBot->update();

//			//Vision during auton
//			if (viewingBack && Cam2Enabled)
//			{
//				IMAQdxGrab(sessionCam2, frame2, true, nullptr);
//				CameraServer::GetInstance()->SetImage(frame2);
//			}
//			if (!viewingBack && Cam1Enabled)
//			{
//				IMAQdxGrab(sessionCam1, frame1, true, nullptr);
//				CameraServer::GetInstance()->SetImage(frame1);
//			}
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
//			if (viewerCooldown > 0)
//				viewerCooldown--;
//			if (gamepad->GetRawButton(8) && viewerCooldown == 0) //Start button
//			{
//				SmartDashboard::PutBoolean("Switched camera", true);
//				//Create cooldown and set the boolean thingy
//				viewerCooldown = 10;
//				viewingBack =! viewingBack;
//
//				if (viewingBack)
//				{
//					//Rear camera: Camera 2
//					StopCamera(1);
//					Cam1Enabled = false;
//					Cam2Enabled = StartCamera(2);
//				}
//				else
//				{
//					StopCamera(2);
//					Cam1Enabled = StartCamera(1);
//					Cam2Enabled = false;
//				}
//			}
//			if (viewingBack && Cam2Enabled)
//			{
//				IMAQdxGrab(sessionCam2, frame2, true, nullptr);
//				CameraServer::GetInstance()->SetImage(frame2);
//			}
//			if (!viewingBack && Cam1Enabled)
//			{
//				IMAQdxGrab(sessionCam1, frame1, true, n4ullptr);
//				CameraServer::GetInstance()->SetImage(frame1);
//			}

			//Output controls
			float intakeInput = gamepad->GetRawAxis(2) - gamepad->GetRawAxis(3); //Subtract left trigger from right trigger
			if (intake != nullptr)
				intake->Set(intakeInput);

			bool liftInput = !gamepad->GetRawButton(5); //Left bumper
			if (lift != nullptr)
			{
				if (liftInput)
					lift->Set(1); //Keeps the lift up unless button is pressed.
				else
					lift->Set(-1);
			}
			
			float armInput = 0; /*(int)gamepad->GetRawButton(3);*/ //X button
			armInput += (int)gamepad->GetRawButton(2) * -1; //B button
			if (intakeArms != nullptr)
				intakeArms->Set(armInput);

			float liftArmInput = gamepad->GetRawButton(6); //Right bumper
			if (liftArms != nullptr)
					liftArms->Set(liftArmInput);
		}

		void TestInit()
		{
			GlobalInit();
		}

		void TestPeriodic()
		{
			//lw->Run();
		}

		void DisabledInit()
		{
			compressor->Stop();
			drivebase->Disengage();
			Input->zeroVels(); //Safety.

			//frontUltra->SetAutomaticMode(false);
			//rearUltra->SetAutomaticMode(false);
		}

		void DisabledPeriodic()
		{
//			if (viewingBack && Cam2Enabled)
//			{
//				IMAQdxGrab(sessionCam2, frame2, true, nullptr);
//				CameraServer::GetInstance()->SetImage(frame2);
//			}
//			if (!viewingBack && Cam1Enabled)
//			{
//				IMAQdxGrab(sessionCam1, frame1, true, nullptr);
//				CameraServer::GetInstance()->SetImage(frame1);
//			}
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
					return false;
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
					return false;
				}
			}
			return true;
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
					return false;
				}
				imaqError = IMAQdxConfigureGrab(sessionCam1);
				if (imaqError != IMAQdxErrorSuccess)
				{
					DriverStation::ReportError(
						"cam0 IMAQdxConfigureGrab error: "
						+ std::to_string((long) imaqError) + "\n");
					return false;
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
					return false;
				}
				imaqError = IMAQdxConfigureGrab(sessionCam2);
				if (imaqError != IMAQdxErrorSuccess)
				{
					DriverStation::ReportError(
						"cam0 IMAQdxConfigureGrab error: "
						+ std::to_string((long) imaqError) + "\n");
					return false;
				}
				// acquire images
				IMAQdxStartAcquisition(sessionCam2);
			}
			return true;
		}
	};
}

START_ROBOT_CLASS(dreadbot::Robot);
