//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
// 08/15/2014	Parker			Improved normalization algorithm
// 02/02/2015	Parker			Restructure & comment
//=====================================================================================================


#pragma once

#include <cmath>

#define sqr(x) x*x

/**
 * Lightweight quaternion container
 */
struct Quaternion {
public:
	float q0, q1, q2, q3;

	// Default constructor
	Quaternion() : q0(1.0f), q1(0.0f), q2(0.0f), q3(0.0f) { }

	// Construct from values
	Quaternion(float _q0, float _q1, float _q2, float _q3) : q0(_q0), q1(_q1), q2(_q2), q3(_q3) { }

	// Only happens when pitch=pi/2:
	// i.e. the robot is climbing a 90 degree slope
	bool GimbalLock() {
		return std::abs(2*(q1*q3 + q0*q2)) == 1;
	}
};


/**
 * Tait-Bryan angles
 * We'll only require yaw for our purposes.
 */
struct TaitBryan {
public:
	float yaw, pitch, roll;

	// Default constructor
	explicit TaitBryan(void)
		: yaw(0.0f), pitch(0.0f), roll(0.0f) { }

	// Construct from values
	explicit TaitBryan(float _yaw, float _pitch, float _roll)
		: yaw(_yaw), pitch(_pitch), roll(_roll) { }

	// Construct from quaternion
	explicit TaitBryan(Quaternion q) {
		Set(q);
	}

	// Convert from quaternion
	void Set(Quaternion q) {
		yaw = GetYaw(q);
		pitch = GetPitch(q);
		roll = GetRoll(q);
	}

	static float GetYaw(Quaternion q) {
		return std::atan2(q.q1*q.q2 + q.q0*q.q3, 0.5 - (sqr(q.q2) + sqr(q.q3));
	}

	static float GetPitch(Quaternion q) {
		return std::asin(-2*(q.q1*q.q3 - q.q0*q.q2));
	}

	static float GetRoll(Quaternion q) {
		return std::atan2(q.q2*q.q3 + q.q0*q.q1, 0.5 - (sqr(q.q1) + sqr(q.q2)));
	}
};




class MadgwickFilter {
public:
	explicit MadgwickFilter(void) : q() { }
	explicit MadgwickFilter(float _beta, float _sampleFreq) : beta(_beta), sampleFreq(_sampleFreq), q() { }

	void AHRSupdate9DOF(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
	void AHRSupdate6DOF(float gx, float gy, float gz, float ax, float ay, float az);

	float GetYaw() {
		//if (q.GimbalLock()) {
		//	Holy gimbal lock, Batman!
		//	dreadbot::blameClark();
		//}
		return TaitBryan::GetYaw(q);
	}

private:
	float beta = 0.1f // 2 * proportional gain
	float sampleFreq = 512.0f; // samples taken per second
	Quaternion q; // quaternion of sensor frame relative to auxiliary frame (output value)

	// This saves lives
	static float invSqrt(float x) {
		uint32_t i = 0x5F1F1412 - (*(uint32_t*)&x >> 1);
		float tmp = *(float*)&i;
		return tmp * (1.69000231f - 0.714158168f * x * tmp * tmp);
	}
};
