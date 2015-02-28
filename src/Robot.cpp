#include <WPILib.h>
#include "SmartDashboard/SmartDashboard.h"
#include "DigitalInput.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "Autonomous.h"
#include "MPU6050.h"
//#include "Robot.h"


namespace dreadbot
{
	class Robot: public IterativeRobot
	{
	private:
		DriverStation *ds;
		Joystick* gamepad;
		Joystick* gamepad_duncan;
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
		bool Cam1Enabled;
		bool Cam2Enabled;

		MPU6050* IMU;

		DigitalInput* lift_switch; // 0
		DigitalInput* transit_switch_l; // 1
		DigitalInput* transit_switch_r; // 2

	public:
		void RobotInit()
		{
			ds = DriverStation::GetInstance();
			SmartDashboard::init();
		//	lw = LiveWindow::GetInstance();
			pdp = new PowerDistributionPanel();
			compressor = new Compressor(0);

			lift_switch = new DigitalInput(0); // 0
			transit_switch_l = new DigitalInput(1); // 1
			transit_switch_r = new DigitalInput(2); // 2


			//frontUltra = new Ultrasonic(6, 7); //Dummy values for the ultrasonics
			//rearUltra = new Ultrasonic(4, 5);

			drivebase = new MecanumDrive(1, 2, 3, 4);
			Input = XMLInput::getInstance();
			Input->setDrivebase(drivebase);
			AutonBot = new RobotFSM;

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
			gamepad_duncan = new Joystick(1);
			drivebase->Engage();

			intake = Input->getMGroup("intake");
			lift = Input->getPGroup("lift");
			liftArms = Input->getPGroup("liftArms");
			intakeArms = Input->getPGroup("intakeArms");
// badd
			IMU = new MPU6050();
		}

		void AutonomousInit()
		{
			GlobalInit();
			AutonBot->setHardware(drivebase, intake);
			AutonBot->setUltras(0, 0); //Basically disables the ultrasonics
			AutonBot->start();

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
			/*
			int16_t ax, ay, az, gx, gy, gz, mx, my, mz;
			IMU->getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);// ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz, int16_t* mx, int16_t* my, int16_t* mz
			SmartDashboard::PutNumber("a-x", ax);
			//SmartDashboard::PutNumber("a-y", ay);
			//SmartDashboard::PutNumber("a-z", az);
			*/
			drivebase->SD_RetrievePID();
			Input->updateDrivebase();
			//drivebase->SD_OutputDiagnostics();
			SmartDashboard::PutBoolean("viewingBack", viewingBack);

			//Vision switch control
			if (viewerCooldown > 0)
				viewerCooldown--;
			if ((gamepad->GetRawAxis(3) > 0.8) && viewerCooldown == 0) //Start button
			{
				SmartDashboard::PutBoolean("Switched camera", true);
				//Create cooldown and set the boolean thingy
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
/* Lift down: lt axis 2 > 0.8
 * Actuate fork: lb button 5
 * Intake arms: rt axis 3 > 0.5
 * Intake arms pneumatics: rb button 6
 */
			//Output controls
			float intakeInput = gamepad->GetRawAxis(3);
			//float intakeInput_duncan = gamepad_duncan->GetRawAxis(3);
			float intakeInput_duncan = 0.0f;
			if (intake != NULL)
				intake->Set(((float) (intakeInput > 0.15) * -1) + (float) (intakeInput_duncan > 0.15));

			float liftInput = gamepad->GetRawAxis(2);
			float liftInput_duncan = gamepad_duncan->GetRawAxis(2);
			if (liftInput_duncan > 0.15) {
				// Lower the lift arms until they collide with the lift switch
				if (!lift_switch->Get()) {

				}
			} else {
				if (lift != NULL)
					lift->Set(liftInput > 0.15 ? -1.0f : 1.0f);
			}
			float armInput = (float) gamepad->GetRawButton(6);
			if (intakeArms != NULL)
				intakeArms->Set(-armInput);

			float liftArmInput = (float) gamepad->GetRawButton(5);
			if (liftArms != NULL)
					liftArms->Set(-liftArmInput);

			SmartDashboard::PutBoolean("R transit switch", transit_switch_r->Get());
			SmartDashboard::PutBoolean("L transit switch", transit_switch_l->Get());
			SmartDashboard::PutBoolean("Lift switch", lift_switch->Get());
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

