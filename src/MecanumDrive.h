#pragma once

#include "WPILib.h"
#include <algorithm>
#include <cmath>

#define MOTOR_COUNT 4
#define STALL_MOTOR_CURRENT 50
#define CONTROL_PERIOD 3


namespace dreadbot {
	class MecanumDrive {
	public:
		enum class drivemode {
			absolute, // Position control for autonomous
			relative  // Velocity control for operator control
		};

		enum motorType : uint8_t {
			m_leftFront = 0,
			m_rightFront = 1,
			m_leftRear = 2,
			m_rightRear = 3
		};

		void Set(int motorId_lf, int motorId_rf, int motorId_lr, int motorId_rr); //Sets the CAN IDs used for the motors.
		MecanumDrive(int motorId_lf, int motorId_rf, int motorId_lr, int motorId_rr);
		~MecanumDrive();

		void GoSlow();
		void GoFast();
		void GoSpeed(double speed);
		void Drive_v(double x, double y, double rotation); //Velocity based driving.
		void SetDriveMode(drivemode newMode);
		
		void Engage();
		void Disengage();

		void SD_RetrievePID();
		void SD_OutputDiagnostics(); //Outputs a bunch of useful motor stats, many of which are disabled (commented out)

	protected:
		bool m_enabled = false;
		const uint8_t syncGroup = 0x00;
		const std::string motorNames[MOTOR_COUNT] = {"LF Drive [1]", "RF Drive [2]", "LB Drive [3]", "RB Drive [4]"};
		const double motorReversals[MOTOR_COUNT] = {-1.0, 1.0, -1.0, 1.0};
		drivemode mode = drivemode::relative;
		CANTalon* motors[4];

	private:
		DISALLOW_COPY_AND_ASSIGN(MecanumDrive);
	};
}
