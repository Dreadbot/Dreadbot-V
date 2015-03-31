#include "Robot.h"

namespace dreadbot {
	class Robot: public IterativeRobot {
		DriverStation *ds;
		Joystick* gamepad1;
		Joystick* gamepad2;
		PowerDistributionPanel *pdp;
		Compressor* compressor;
		XMLInput* Input;
		HALBot* AutonBot;

		USBCamera* camera;
		Image* frame;

	public:
		void RobotInit() {
			ds = DriverStation::GetInstance();
			SmartDashboard::init();
			pdp = new PowerDistributionPanel();
			compressor = new Compressor(0);

			drivebase = new MecanumDrive(1, 2, 3, 4);
			Input = XMLInput::getInstance();
			Input->setDrivebase(drivebase);
			Input->loadXMLConfig();
			intake = Input->getMGroup("intake");
			lift = Input->getPGroup("lift");
			fork = Input->getPGroup("liftArms");
			intakeArms = Input->getPGroup("intakeArms");
			gamepad1 = Input->getController(COM_PRIMARY_DRIVER);
			gamepad2 = Input->getController(COM_BACKUP_DRIVER);

			AutonBot = nullptr;

			frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
			camera = new USBCamera("cam1", false);
			camera->OpenCamera();
			camera->StartCapture();
			DreadVision::Init();
		}

		void GlobalInit() {
			compressor->Start();
			drivebase->Engage();
		}

		void AutonomousInit() {
			GlobalInit();
			enum AutonMode mode = GetAutonMode();

			if (AutonBot != nullptr)
				delete AutonBot;

			AutonBot = new HALBot;
			AutonBot->setMode(mode);
			AutonBot->init();

			if (mode == AUTON_MODE_STACK3) {
				lift->Set(1);
				Wait(0.25);
			}
		}

		void AutonomousPeriodic() {
			if (DriverStation::GetInstance()->GetMatchTime() <= 15.0f)
				AutonBot->update();
			else
				drivebase->Drive(0, 0, 0);

			TransmitImage();
		}

		void TeleopInit() {
			drivebase->GoSlow();
			GlobalInit();
		}

		void TeleopPeriodic() {
			// Drive controls
			float driveX = CurveInput(gamepad1->GetRawAxis(AXS_DRIVE_X));
			float driveY = CurveInput(gamepad1->GetRawAxis(AXS_DRIVE_Y));
			float driveR = 0.7*CurveInput(gamepad1->GetRawAxis(AXS_DRIVE_R));
			drivebase->Drive(driveX, driveY, driveR);

			// Intake & transit wheels
			intake->Set(((float) (gamepad1->GetRawAxis(AXS_INTAKE_IN) > 0.1) * -0.8) + gamepad2->GetRawAxis(B_AXS_TOTE_OUT) - gamepad2->GetRawAxis(B_AXS_TOTE_IN));
			// Lift
			if (gamepad1->GetRawButton(BTN_STOP_LIFT))
				lift->Set(0);
			else
				lift->Set(gamepad1->GetRawAxis(AXS_LIFT_DOWN) > 0.1 ? -1 : 1);
			// Arms
			intakeArms->Set(-(float) gamepad1->GetRawButton(BTN_ARMS_OUT) + (float) gamepad2->GetRawButton(B_BTN_ARMS_IN) - (float) gamepad2->GetRawButton(B_BTN_ARMS_OUT));
			// Forks
			fork->Set(-(float) gamepad1->GetRawButton(BTN_OPEN_FORK));

			TransmitImage();
		}

		void TestInit() {
			int8_t liftDir = 1;
			if (GetAutonMode() == AUTON_MODE_STOP) {
				compressor->Start();
				return;
			} else if (GetAutonMode() != AUTON_MODE_STACK3) {
				// Go down if the lift is starting from the top.
				liftDir = -1;
			}
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
			}
		}

		void DisabledPeriodic() {
			TransmitImage();
		}


		void TransmitImage() {
			DreadVision::RegisterFrame(camera->GetImage(frame));
			CameraServer::GetInstance()->SetImage(DreadVision::GetFrame());
		}

		float CurveInput(float x) {
			float out = 0.255000975f*std::exp2(2.299113817f*std::fabs(x));
			if (std::fabs(x) < DRIVER_DEADZONE)
				return 0.0f;
			return x < 0.0f ? -out : out;
		}

		bool StartCamera() {
			imaqError = IMAQdxOpenCamera("cam1", IMAQdxCameraControlModeController, &cameraSession);
			if (imaqError != IMAQdxErrorSuccess) {
				DriverStation::ReportError("cam1 IMAQdxOpenCamera error: " + std::to_string((long) imaqError) + "\n");
				return false;
			}
			imaqError = IMAQdxConfigureGrab(cameraSession);
			if (imaqError != IMAQdxErrorSuccess) {
				DriverStation::ReportError("cam1 IMAQdxConfigureGrab error: " + std::to_string((long) imaqError) + "\n");
				return false;
			}
			IMAQdxStartAcquisition(cameraSession);
			return true;
		}
	};
}

START_ROBOT_CLASS(dreadbot::Robot);
