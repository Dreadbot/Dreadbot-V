#include "MecanumDrive.h"


MecanumDrive::MecanumDrive(uint32_t frontLeftMotorChannel, uint32_t rearLeftMotorChannel, uint32_t frontRightMotorChannel, uint32_t rearRightMotorChannel) {
	m_frontLeftMotor = new CANTalon(frontLeftMotorChannel);
	m_frontRightMotor = new CANTalon(rearLeftMotorChannel);
	m_rearLeftMotor = new CANTalon(frontRightMotorChannel);
	m_rearRightMotor = new CANTalon(rearRightMotorChannel);
	gyroscope = new Gyro(1);
	accelerometer = new BuiltInAccelerometer();
}

MecanumDrive::~MecanumDrive() {
	delete m_frontLeftMotor;
	delete m_rearLeftMotor;
	delete m_frontRightMotor;
	delete m_rearRightMotor;
}

/**
 * @param dx
 * @param dy
 * @param speed
 *
 */

void MecanumDrive::Drive_p(float dx, float dy, float rotation) {
	// out(t) = e(t)*p + int(e(t), 0, t)*i + dt*d
	// Update the accelerometer
	double ax = accelerometer->GetX();
	double ay = accelerometer->GetY();
	RotateVector(ax, ay, (double) gyroscope->GetAngle());
}

/**
 * @param x The speed that the robot should drive in the X direction. [-1.0..1.0]
 * @param y The speed that the robot should drive in the Y direction.
 * This input is inverted to match the forward == -1.0 that joysticks produce. [-1.0..1.0]
 * @param rotation The rate of rotation for the robot that is completely independent of
 * the translation. [-1.0..1.0]
 */
void MecanumDrive::Drive_v(float x, float y, float rotation) {
	double xIn = x;
	double yIn = y;
	// Negate y for the joystick.
	yIn = -yIn;
	// Compenstate for gyro angle.
	RotateVector(xIn, yIn, (double) gyroscope->GetAngle());

	double wheelSpeeds[4];
	wheelSpeeds[kFrontLeftMotor] = xIn + yIn + rotation;
	wheelSpeeds[kFrontRightMotor] = -xIn + yIn - rotation;
	wheelSpeeds[kRearLeftMotor] = -xIn + yIn + rotation;
	wheelSpeeds[kRearRightMotor] = xIn + yIn - rotation;

	Normalize(wheelSpeeds);

	m_frontLeftMotor->Set(wheelSpeeds[kFrontLeftMotor], syncGroup);
	m_frontRightMotor->Set(wheelSpeeds[kFrontRightMotor], syncGroup);
	m_rearLeftMotor->Set(wheelSpeeds[kRearLeftMotor], syncGroup);
	m_rearRightMotor->Set(wheelSpeeds[kRearRightMotor], syncGroup);
}

/**
 * Normalize all wheel speeds if the magnitude of any wheel is greater than 1.0.
 */
void MecanumDrive::Normalize(double *wheelSpeeds) {
	double maxMagnitude = fabs(wheelSpeeds[0]);
	int8_t i;
	for (i = 1; i < 4; i++) {
		double temp = fabs(wheelSpeeds[i]);
		if (maxMagnitude < temp)
			maxMagnitude = temp;
	}
	if (maxMagnitude > 1.0) {
		for (i = 0; i < 4; i++) {
			wheelSpeeds[i] = wheelSpeeds[i]/maxMagnitude;
		}
	}
}


void MecanumDrive::RotateVector(double &x, double &y, double angle) {
	angle *= 0.01745329251;
	double cosA = cos(angle);
	double sinA = sin(angle);
	double xOut = x*cosA - y*sinA;
	double yOut = x*sinA + y*cosA;
	x = xOut;
	y = yOut;
}


void MecanumDrive::EnableControl() {
	if (m_frontLeftMotor != NULL)
		m_frontLeftMotor->EnableControl();
	if (m_frontRightMotor != NULL)
		m_frontRightMotor->EnableControl();
	if (m_rearLeftMotor != NULL)
		m_rearLeftMotor->EnableControl();
	if (m_rearRightMotor != NULL)
		m_rearRightMotor->EnableControl();
}


void MecanumDrive::DisableControl() {
	if (m_frontLeftMotor != NULL)
		m_frontLeftMotor->Disable();
	if (m_frontRightMotor != NULL)
		m_frontRightMotor->Disable();
	if (m_rearLeftMotor != NULL)
		m_rearLeftMotor->Disable();
	if (m_rearRightMotor != NULL)
		m_rearRightMotor->Disable();
}
