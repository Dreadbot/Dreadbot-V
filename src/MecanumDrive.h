#pragma once

#include "ErrorBase.h"
#include <stdlib.h>
#include "HAL/HAL.hpp"
#include "CANTalon.h"
#include "Gyro.h"
#include "BuiltInAccelerometer.h"
#include "Utility.h"
#include "WPIErrors.h"
#include <math.h>

#undef max
#include <algorithm>

#define syncGroup 0x80

class CANTalon;

class MecanumDrive: public ErrorBase {
public:
	enum MotorType {
		kFrontLeftMotor = 0,
		kFrontRightMotor = 1,
		kRearLeftMotor = 2,
		kRearRightMotor = 3
	};

	MecanumDrive(uint32_t frontLeftMotorChannel, uint32_t rearLeftMotorChannel,
				 uint32_t frontRightMotorChannel, uint32_t rearRightMotorChannel);
	~MecanumDrive();

	void Drive_p(float x, float y, float rotation);
	void Drive_v(float x, float y, float rotation);

	void EnableControl();
	void DisableControl();

protected:
	double x = 0.0;
	double y = 0.0;

	void Normalize(double *wheelSpeeds);
	void RotateVector(double &x, double &y, double angle);

	CANTalon *m_frontLeftMotor;
	CANTalon *m_frontRightMotor;
	CANTalon *m_rearLeftMotor;
	CANTalon *m_rearRightMotor;

private:
	Gyro* gyroscope;
	BuiltInAccelerometer* accelerometer;
	DISALLOW_COPY_AND_ASSIGN(MecanumDrive);
};
