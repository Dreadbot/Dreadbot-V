#include "WPILib.h"
#include "MecanumDrive.h"
#include "XMLInput.h"
#include "Autonomous.h"
#include "Robot.h"
#include "Vision.h"
#include "DreadbotDIO.h"


namespace dreadbot {
	enum Camera : uint8_t { // Let's buy 256 webcams so that we can laugh shallowly about it
		CAMERA_FRONT = 1,
		CAMERA_BACK = 2
	};

	class Robot: public IterativeRobot {
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
		Image* frame;
		enum Camera currentCam;
		int viewerCooldown;

	public:
		void RobotInit() {
			ds = DriverStation::GetInstance();
			SmartDashboard::init();
			pdp = new PowerDistributionPanel();
			compressor = new Compressor(0);

			drivebase = new MecanumDrive(1, 2, /*FIVE!*/ 3, 4);
			Input = XMLInput::getInstance();
			Input->setDrivebase(drivebase);
			AutonBot = nullptr;

			intake = nullptr;
			lift = nullptr;
			liftArms = nullptr;
			intakeArms = nullptr;

			frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
			StartCamera(currentCam = CAMERA_FRONT);
			DreadVision::Init();
		}

		void GlobalInit() {
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

		void AutonomousInit() {
			GlobalInit();
			if (currentCam == CAMERA_BACK) {
				StopCamera();
				StartCamera(CAMERA_FRONT);
			}

			if (AutonBot == nullptr)
				AutonBot = new HALBot;
			AutonBot->setMode(GetAutonMode());
			AutonBot->init(drivebase, intake, lift);

			if (AutonBot->getMode() == AUTON_MODE_STACK3) {
				lift->Set(1);
				Wait(0.2);
			}
		}

		void AutonomousPeriodic() {
			IMAQdxGrab(sessionCam1, frame, true, nullptr);
			DreadVision::RegisterFrame(frame);

			if (DriverStation::GetInstance()->GetMatchTime() <= 15.0f) {
				AutonBot->update();
			} else {
				drivebase->Drive(0, 0, 0);
			}

			CameraServer::GetInstance()->SetImage(DreadVision::GetFrame());
		}

		void TeleopInit() {
			GlobalInit();
		}

		void TeleopPeriodic() {
			// Drive controls. No deadzones.
			float x = gamepad->GetRawAxis(AXS_DRIVE_X);
			float y = gamepad->GetRawAxis(AXS_DRIVE_Y);
			float r = gamepad->GetRawAxis(AXS_DRIVE_R);
			x *= x;
			y *= y;
			r *= r*0.7;
			drivebase->Drive(x, y, r);

			// Intake & transit wheels
			intake->Set(((float) (gamepad->GetRawAxis(AXS_INTAKE_IN) > 0.05) * -0.8) + gamepad2->GetRawAxis(B_AXS_TOTE_OUT) - gamepad2->GetRawAxis(B_AXS_TOTE_IN));
			// Lift
			if (gamepad->GetRawButton(BTN_STOP_LIFT))
				lift->Set(0.0f);
			else
				lift->Set(gamepad->GetRawAxis(AXS_LIFT_DOWN) > 0.1 ? -1.0f : 1.0f);
			// Arms
			intakeArms->Set(-(float) gamepad->GetRawButton(BTN_ARMS_OUT) + (float) gamepad2->GetRawButton(B_BTN_ARMS_IN) - (float) gamepad2->GetRawButton(B_BTN_ARMS_OUT));
			// Forks
			liftArms->Set(-(float) gamepad->GetRawButton(BTN_OPEN_FORK));

			//Vision switch control
			if (viewerCooldown > 0)
				viewerCooldown--;
			if ((gamepad->GetRawButton(BTN_SWITCH_CAM) || gamepad2->GetRawButton(BTN_SWITCH_CAM)) && viewerCooldown == 0) {
				viewerCooldown = 10;
				StopCamera();
				if (currentCam == CAMERA_FRONT)
					StartCamera(CAMERA_BACK);
				else
					StartCamera(CAMERA_FRONT);
			}

			TransmitThresholdImage();
		}

		void TestInit() {
			int8_t liftDir = 1;
			if (GetAutonMode() == AUTON_MODE_STOP) {
				compressor->Start();
				return;
			} else if (GetAutonMode() != AUTON_MODE_STACK3) {
				// If the lift is starting from the top, go downup instead of updown
				liftDir = -1;
			}
			// This vents air from the lift cylinders, creating a sound which
			// can be used to warn pedestrians that the robot will shortly
			// be entering autonomous mode.
			float p = 1;
			float vt = 0.1;
			for (uint8_t i = 0; i < 3; ++i) {
				Wait(p - 2*vt);
				lift->Set(liftDir);
				Wait(vt);
				lift->Set(-liftDir);
				Wait(vt);
			}
			Wait(p - 2*vt);
			AutonomousInit();
		}

		void TestPeriodic() {
			AutonomousPeriodic();
		}

		void DisabledInit() {
			compressor->Stop();
			drivebase->Disengage();

			if (AutonBot != nullptr) {
				delete AutonBot;
				AutonBot = nullptr;
			}
		}

		void DisabledPeriodic() {
			TransmitThresholdImage();
		}

		void TransmitThresholdImage() {
			IMAQdxGrab(currentCam == CAMERA_FRONT ? sessionCam1 : sessionCam2, frame, true, nullptr);
			DreadVision::RegisterFrame(frame);
			CameraServer::GetInstance()->SetImage(DreadVision::GetFrame());
		}

		bool StopCamera() {
			if (currentCam == CAMERA_FRONT) {
				// stop image acquisition
				IMAQdxStopAcquisition(sessionCam1);
				//the camera name (ex "cam0") can be found through the roborio web interface
				imaqError = IMAQdxCloseCamera(sessionCam1);
				if (imaqError != IMAQdxErrorSuccess) {
					DriverStation::ReportError("cam2 IMAQdxCloseCamera error: " + std::to_string((long) imaqError) + "\n");
					return false;
				}
			} else if (currentCam == CAMERA_BACK) {
				IMAQdxStopAcquisition(sessionCam2);
				imaqError = IMAQdxCloseCamera(sessionCam2);
				if (imaqError != IMAQdxErrorSuccess) {
					DriverStation::ReportError("cam2 IMAQdxCloseCamera error: " + std::to_string((long) imaqError) + "\n");
					return false;
				}
			}
			return true;
		}

		bool StartCamera(enum Camera cameraNum) {
			currentCam = cameraNum;
			if (cameraNum == CAMERA_FRONT) {
				imaqError = IMAQdxOpenCamera("cam1", IMAQdxCameraControlModeController, &sessionCam1);
				if (imaqError != IMAQdxErrorSuccess) {
					DriverStation::ReportError("cam1 IMAQdxOpenCamera error: " + std::to_string((long) imaqError) + "\n");
					return false;
				}
				imaqError = IMAQdxConfigureGrab(sessionCam1);
				if (imaqError != IMAQdxErrorSuccess) {
					DriverStation::ReportError("cam1 IMAQdxConfigureGrab error: " + std::to_string((long) imaqError) + "\n");
					return false;
				}
				// acquire images
				IMAQdxStartAcquisition(sessionCam1);
			} else if (cameraNum == CAMERA_BACK) {
				imaqError = IMAQdxOpenCamera("cam2", IMAQdxCameraControlModeController, &sessionCam2);
				if (imaqError != IMAQdxErrorSuccess) {
					DriverStation::ReportError("cam2 IMAQdxOpenCamera error: " + std::to_string((long) imaqError) + "\n");
					return false;
				}
				imaqError = IMAQdxConfigureGrab(sessionCam2);
				if (imaqError != IMAQdxErrorSuccess) {
					DriverStation::ReportError("cam2 IMAQdxConfigureGrab error: "+ std::to_string((long) imaqError) + "\n");
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
