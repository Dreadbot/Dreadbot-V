#include "MecanumDrive.h"

/**
 * Maximum v in all directions for an unloaded CIM at 12 volts is 600 encoder ticks/second
 * Motor drive and sensor directions should be inverted on the left side of the robot
 */
 
using namespace dreadbot;

// Constructor helper method
void MecanumDrive::Set(int motorId_lf, int motorId_rf, int motorId_lr, int motorId_rr) {
	mode = drivemode::relative;

	motors[m_leftFront] = new CANTalon(motorId_lf, CONTROL_PERIOD);
	motors[m_rightFront] = new CANTalon(motorId_rf, CONTROL_PERIOD);
	motors[m_leftRear] = new CANTalon(motorId_lr, CONTROL_PERIOD);
	motors[m_rightRear] = new CANTalon(motorId_rr, CONTROL_PERIOD);

	motors[m_leftFront]->SetSensorDirection(true);
	motors[m_rightFront]->SetSensorDirection(false);
	motors[m_leftRear]->SetSensorDirection(true);
	motors[m_rightRear]->SetSensorDirection(false);

	// Configure parameters
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		motors[i]->SetControlMode(CANSpeedController::ControlMode::kSpeed);
		motors[i]->SetPosition(0.0);
		motors[i]->SelectProfileSlot(0);
		motors[i]->SetPID(0.5, 0, 0, 0);
		motors[i]->SetVoltageRampRate(0.5f); //Ramp up for drive motors
	}

	//x_ctrl = new SimplePID(0.2, 0, 0, false);
	//y_ctrl = new SimplePID(0.2, 0, 0, false);
	//r_ctrl = new SimplePID(0.2, 0, 0, true);
}

// Constructor
MecanumDrive::MecanumDrive(int motorId_lf, int motorId_rf, int motorId_lr, int motorId_rr) {
	Set(motorId_lf, motorId_rf, motorId_lr, motorId_rr);
}

// Destructor
MecanumDrive::~MecanumDrive() {
	Disengage();
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {

		delete motors[i];
	}
}

// Drive to position
void MecanumDrive::Drive_p(double x, double y, double rotation) {
	if (mode == drivemode::absolute) {
		//vec_out.rotate((double) gyroscope->GetAngle());
		// yield thread until the robot has passed within a certain tolerance of the target?
		// a callback might work?
	}
}

// Drive with wheel velocity
void MecanumDrive::Drive_v(double x, double y, double rotation) {
	Vector2<double> vec_out(y, x);
	double rot_out = -rotation;

	if (mode == drivemode::relative) {
		/*
		#ifdef SQUARE_INPUTS
			vec_out.x = vec_out.x*std::abs(vec_out.x);
			vec_out.y = vec_out.y*std::abs(vec_out.y);
			rot_out = rot_out*std::abs(rot_out);
		#endif
		*/
	}

	double wspeeds[4];
	wspeeds[m_leftFront] = vec_out.x + vec_out.y + rot_out;
	wspeeds[m_rightFront] = -vec_out.x + vec_out.y - rot_out;
	wspeeds[m_leftRear] = -vec_out.x + vec_out.y + rot_out;
	wspeeds[m_rightRear] = vec_out.x + vec_out.y - rot_out;

	float absSpeeds[4];
	for (int i = 0; i < 4; ++i)
		absSpeeds[i] = fabs(wspeeds[i]);

	double maxMagnitude = *std::max_element(absSpeeds, absSpeeds + MOTOR_COUNT);
	if (maxMagnitude > 1.0) {
		for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
			wspeeds[i] /= maxMagnitude;
		}
	}

	bool stall = true;
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		// files.andymark.com/CIM-motor-curve.pdf
		stall = stall && (motors[i]->GetOutputCurrent() > STALL_MOTOR_CURRENT);
	}
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		motors[i]->Set(wspeeds[i]*motorReversals[i]*SmartDashboard::GetNumber("Speed", 1000.0), syncGroup); // *stall
	}
}

void MecanumDrive::SetDriveMode(drivemode newMode) {
	mode = newMode;
	CANSpeedController::ControlMode cmode;
	if (mode == drivemode::relative) {
		cmode = CANSpeedController::ControlMode::kSpeed;
	} else if (mode == drivemode::absolute) {
		cmode = CANSpeedController::ControlMode::kPosition;
	}
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		motors[i]->SetControlMode(cmode);
	}
}

void MecanumDrive::Engage() {
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		motors[i]->EnableControl();
	}
	m_enabled = true;
}

void MecanumDrive::Disengage() {
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		motors[i]->Disable();
	}
	m_enabled = false;
}

void MecanumDrive::SD_RetrievePID() {
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		motors[i]->SetPID(SmartDashboard::GetNumber("P", 0.5), SmartDashboard::GetNumber("I", 0.0),SmartDashboard::GetNumber("D", 0.0));
	}
}

void MecanumDrive::SD_OutputDiagnostics() {
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i)
	{
//		SmartDashboard::PutNumber(motorNames[i] + ".temp", motors[i]->GetTemperature());
//		SmartDashboard::PutNumber(motorNames[i] + ".setpoint", motors[i]->GetSetpoint());
//		SmartDashboard::PutNumber(motorNames[i] + ".encoder p", motors[i]->GetPosition());
//		SmartDashboard::PutNumber(motorNames[i] + ".encoder v", motors[i]->GetEncVel());
//		SmartDashboard::PutNumber(motorNames[i] + ".error", motors[i]->GetClosedLoopError());
		SmartDashboard::PutNumber(motorNames[i] + ".voltage", motors[i]->GetOutputVoltage());
	}

}
