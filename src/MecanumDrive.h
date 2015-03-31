#pragma once

#include "WPILib.h"
#include "mathutil.h"
#include "rps.h"
#include "SimplePID.h"
#include <algorithm>
#include <cmath>

#define SQUARE_INPUTS
#define MOTOR_COUNT 4
#define STALL_MOTOR_CURRENT 50
#define CONTROL_PERIOD 10


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

		void Set(int motorId_lf, int motorId_rf, int motorId_lr, int motorId_rr);
		MecanumDrive(int motorId_lf, int motorId_rf, int motorId_lr, int motorId_rr);
		~MecanumDrive();

		void GoSlow();
		void GoFast();
		void Drive_p(double x, double y, double rotation);
		void Drive_v(double x, double y, double rotation);
		void SetDriveMode(drivemode newMode);
		
		void Engage();
		void Disengage();

		void SD_RetrievePID();
		void SD_OutputDiagnostics();

	protected:
		bool m_enabled = false;
		const uint8_t syncGroup = 0x00;
		const std::string motorNames[MOTOR_COUNT] = {"Left-front", "Right-front", "Left-rear", "Right-rear"};
		const double motorReversals[MOTOR_COUNT] = {-1.0, 1.0, -1.0, 1.0};
		SimplePID* x_ctrl;
		SimplePID* y_ctrl;
		SimplePID* r_ctrl;
		drivemode mode = drivemode::relative;
		//rps* positioner;
		CANTalon* motors[4];

	private:
		DISALLOW_COPY_AND_ASSIGN(MecanumDrive);
	};
}
