#include <WPILib.h>
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "Autonomous.h"
#include "Robot.h"
#include "DreadbotDIO.h"


namespace dreadbot 
{
	class Robot: public IterativeRobot 
	{
		DriverStation *ds;
		Joystick* gamepad;
		Joystick* gamepad2;
		PowerDistributionPanel *pdp;
		Compressor* compressor;

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
			pdp = new PowerDistributionPanel();
			compressor = new Compressor(0);

			drivebase = new MecanumDrive(1, 2, 3, 4);
			Input = XMLInput::getInstance();
			Input->setDrivebase(drivebase);
			AutonBot = nullptr;

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

			Input->loadXMLConfig();
			gamepad = Input->getController(COM_PRIMARY_DRIVER);
			gamepad2 = Input->getController(COM_BACKUP_DRIVER);

			intake = Input->getMGroup("intake");
			lift = Input->getPGroup("lift");
			liftArms = Input->getPGroup("liftArms");
			intakeArms = Input->getPGroup("intakeArms");
		}

		void AutonomousInit()
		{
			GlobalInit();
			if (AutonBot == nullptr)
				AutonBot = new HALBot;
			AutonBot->setMode(GetAutonMode());
			AutonBot->init(drivebase, intake, lift);

			if (AutonBot->getMode() == AUTON_MODE_STACK3)
			{
				lift->Set(1);
				Wait(0.2);
			}
		}

		void AutonomousPeriodic()
		{
			drivebase->SD_RetrievePID();
			if (DriverStation::GetInstance()->GetMatchTime() <= 15.0f)
				AutonBot->update();
			else
				drivebase->Drive_v(0, 0, 0);

			//Vision during auton
			if (viewingBack && Cam2Enabled)
			{
				IMAQdxGrab(sessionCam2, frame2, true, nullptr);
				CameraServer::GetInstance()->SetImage(frame2);
			}
			if (!viewingBack && Cam1Enabled)
			{
				IMAQdxGrab(sessionCam1, frame1, true, nullptr);
				CameraServer::GetInstance()->SetImage(frame1);
			}
		}

		void TeleopInit()
		{
			GlobalInit();
		}

		void TeleopPeriodic()
		{
			drivebase->SD_RetrievePID();
			Input->updateDrivebase();

			//Output controls
			float intakeInput = gamepad->GetRawAxis(3);
			intake->Set(((float) (intakeInput > 0.15) * -0.8) + gamepad2->GetRawAxis(3) - gamepad2->GetRawAxis(2));


			if (gamepad->GetRawButton(1)) {
				lift->Set(0.0f);
			} else {
				lift->Set(gamepad->GetRawAxis(2) > 0.1 ? -1.0f : 1.0f);
			}
			intakeArms->Set(-(float) gamepad->GetRawButton(6) + (float) gamepad2->GetRawButton(2) - (float) gamepad2->GetRawButton(3));

			liftArms->Set(-(float) gamepad->GetRawButton(5));

			//Vision switch control
			if (viewerCooldown > 0)
				viewerCooldown--;
			if ((gamepad->GetRawButton(8) || gamepad2->GetRawButton(8)) && viewerCooldown == 0) //Start button
			{
				SmartDashboard::PutBoolean("Switched camera", true);
				viewerCooldown = 10;
				viewingBack =! viewingBack;
				if (viewingBack)
				{
					//Rear camera: Camera 2
					StopCamera(1);
					Cam1Enabled = false;
					Cam2Enabled = StartCamera(2);
				}
				else
				{
					StopCamera(2);
					Cam1Enabled = StartCamera(1);
					Cam2Enabled = false;
				}
			}
			if (viewingBack && Cam2Enabled)
			{
				IMAQdxGrab(sessionCam2, frame2, true, nullptr);
				CameraServer::GetInstance()->SetImage(frame2);
			}
			if (!viewingBack && Cam1Enabled)
			{
				IMAQdxGrab(sessionCam1, frame1, true, nullptr);
				CameraServer::GetInstance()->SetImage(frame1);
			}
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

			if (AutonBot != nullptr)
			{
				delete AutonBot;
				AutonBot = nullptr;
			}
		}

		void DisabledPeriodic()
		{
			if (viewingBack && Cam2Enabled)
			{
				IMAQdxGrab(sessionCam2, frame2, true, nullptr);
				CameraServer::GetInstance()->SetImage(frame2);
			}
			if (!viewingBack && Cam1Enabled)
			{
				IMAQdxGrab(sessionCam1, frame1, true, nullptr);
				CameraServer::GetInstance()->SetImage(frame1);
			}
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
