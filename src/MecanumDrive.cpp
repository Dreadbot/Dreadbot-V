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

	motors[m_leftFront]->SetSensorDirection(false);
	motors[m_rightFront]->SetSensorDirection(false);
	motors[m_leftRear]->SetSensorDirection(false);
	motors[m_rightRear]->SetSensorDirection(false);

	// Configure parameters
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		motors[i]->SetControlMode(CANSpeedController::ControlMode::kSpeed);
		motors[i]->SetPosition(0.0);
		motors[i]->SelectProfileSlot(0);
		//motors[i]->SetFeedbackDevice(CANTalon::QuadEncoder);
		motors[i]->SetPID(0.5, 0, 0, 0);
		motors[i]->SetVoltageRampRate(0.5); //Ramp up for drive motors
	}
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

void MecanumDrive::GoFast() {
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		motors[i]->SetPID(1, 0, 0); //Magically makes the robot drive faster.
	}
}

void MecanumDrive::GoSlow() {
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		motors[i]->SetPID(0.5, 0, 0); //Magically makes the robot drive slower.
	}
}

void MecanumDrive::GoSpeed(double speed) {
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		motors[i]->SetPID(speed, 0, 0); //Magically makes the robot drive slower.
	}
}

// Drive with wheel velocity
void MecanumDrive::Drive_v(double x, double y, double rotation) {
	double x_out = y;
	double y_out = x;
	double rot_out = -rotation - (sqrt(x*x + y*y) * 0.01f);

	double wspeeds[4];
	wspeeds[m_leftFront] = x_out + y_out + rot_out;
	wspeeds[m_rightFront] = -x_out + y_out - rot_out;
	wspeeds[m_rightRear] = x_out + y_out - rot_out;
	wspeeds[m_leftRear] = -x_out + y_out + rot_out;

	float absSpeeds[4];
	for (int i = 0; i < 4; ++i)
		absSpeeds[i] = fabs(wspeeds[i]);

	double maxMagnitude = *std::max_element(absSpeeds, absSpeeds + MOTOR_COUNT);
	if (maxMagnitude > 1.0) { //Normalizes wheel speeds if any one speed's absolute value is above 1, since the range of accepted inputs is between 1 and -1.
		for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
			wspeeds[i] /= maxMagnitude;
		}
	}

	//Determines if a motor has stalled and proceeds to do exactly nothing with that information.
	/*
	bool stall = true;
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		// files.andymark.com/CIM-motor-curve.pdf
		stall = stall && (motors[i]->GetOutputCurrent() > STALL_MOTOR_CURRENT);
	}
	*/
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		motors[i]->Set(wspeeds[i]*motorReversals[i]*512.f, syncGroup);
	}
}

void MecanumDrive::SetDriveMode(drivemode newMode) {
	for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
		motors[i]->SetControlMode(CANSpeedController::ControlMode::kSpeed);
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
		SmartDashboard::PutNumber(motorNames[i] + ".setpoint", motors[i]->GetSetpoint());
		SmartDashboard::PutNumber(motorNames[i] + ".encoder p", motors[i]->GetPosition());
		SmartDashboard::PutNumber(motorNames[i] + ".encoder v", motors[i]->GetEncVel());
		SmartDashboard::PutNumber(motorNames[i] + ".error", motors[i]->GetClosedLoopError());
//		SmartDashboard::PutNumber(motorNames[i] + ".voltage", motors[i]->GetOutputVoltage());
//		SmartDashboard::PutNumber(motorNames[i] + ".amperage", motors[i]->GetOutputCurrent());
	}

}
