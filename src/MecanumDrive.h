#pragma once

#include "WPILib.h"
#include "mathutil.h"
#include "rps.h"
#include "SimplePID.h"
#include <algorithm>
#include <cmath>
/**
 * Maximum v in all directions for an unloaded CIM at 12 volts is 600 encoder ticks/second
 * Motor drive and sensor directions should be inverted for the left side of the robot
 */

//#define SQUARE_INPUTS
#define OUTPUT_EVERYTHING
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

		void Set(int motorId_lf, int motorId_rf, int motorId_lr, int motorId_rr) {
			mode = drivemode::relative;

			motors[m_leftFront] = new CANTalon(motorId_lf, CONTROL_PERIOD);
			motors[m_rightFront] = new CANTalon(motorId_rf, CONTROL_PERIOD);
			motors[m_leftRear] = new CANTalon(motorId_lr, CONTROL_PERIOD);
			motors[m_rightRear] = new CANTalon(motorId_rr, CONTROL_PERIOD);

			motors[m_leftFront]->SetSensorDirection(true);
			motors[m_rightFront]->SetSensorDirection(false);
			motors[m_leftRear]->SetSensorDirection(true);
			motors[m_rightRear]->SetSensorDirection(false);

			// Configure loop parameters
			for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
				motors[i]->SetControlMode(CANSpeedController::ControlMode::kSpeed);
				motors[i]->SetPosition(0.0);
				motors[i]->SelectProfileSlot(0);
				motors[i]->SetPID(0.2, 0, 0, 0);
			}

			x_ctrl = new SimplePID(0.2, 0, 0, false);
			y_ctrl = new SimplePID(0.2, 0, 0, false);
			r_ctrl = new SimplePID(0.2, 0, 0, true);
		}

		MecanumDrive(int motorId_lf, int motorId_rf, int motorId_lr, int motorId_rr) {
			Set(motorId_lf, motorId_rf, motorId_lr, motorId_rr);
		}

		~MecanumDrive() {
			for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
				delete motors[i];
			}
		}

		void Drive_p(double x, double y, double rotation) {
			if (mode == drivemode::absolute) {
				//vec_out.rotate((double) gyroscope->GetAngle());
				// yield thread until the robot has passed within a certain tolerance of the target?
				// a callback might work as well
			}
		}

		void Drive_v(double x, double y, double rotation) {
			Vector2<double> vec_out(x, -y);
			vec_out.x = (std::abs(vec_out.x) < 0.01) ? 0.0 : vec_out.x;
			vec_out.y = (std::abs(vec_out.y) < 0.01) ? 0.0 : vec_out.y;
			double rot_out = rotation;

			if (mode == drivemode::relative) {
				#ifdef SQUARE_INPUTS
					vec_out = vec_out*vec_out;
				#endif
			}

			double wspeeds[4];
			wspeeds[m_leftFront] = vec_out.x + vec_out.y + rot_out;
			wspeeds[m_rightFront] = -vec_out.x + vec_out.y - rot_out;
			wspeeds[m_leftRear] = -vec_out.x + vec_out.y + rot_out;
			wspeeds[m_rightRear] = vec_out.x + vec_out.y - rot_out;

			double maxMagnitude = *std::max_element(wspeeds, wspeeds + MOTOR_COUNT);
			if (maxMagnitude > 1.0) {
				for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
					wspeeds[i] /= maxMagnitude;
				}
			}

			bool stall = false;
			for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
				// files.andymark.com/CIM-motor-curve.pdf
				stall = stall || (motors[i]->GetOutputCurrent() > STALL_MOTOR_CURRENT);
				stall |= stall;
			}
			#ifdef OUTPUT_EVERYTHING
				SmartDashboard::PutBoolean("Overcurrent", stall);
			#endif
			stall = !stall;
			for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
				motors[i]->Set(wspeeds[i]*motorReversals[i]*6000, syncGroup); // *stall
			}
		}

		void SetDriveMode(drivemode newMode) {
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

		void Engage() {
			for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
				motors[i]->EnableControl();
			}
		}

		void Disengage() {
			for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
				motors[i]->Disable();
			}
		}

		void SD_RetrievePID() {
			for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
				motors[i]->SetPID(SmartDashboard::GetNumber("P", 0.5), SmartDashboard::GetNumber("I", 0.0),SmartDashboard::GetNumber("D", 0.0));
			}
		}

		void SD_OutputDiagnostics() {
			for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
				SmartDashboard::PutNumber(motorNames[i] + ".temp", motors[i]->GetTemperature());
				SmartDashboard::PutNumber(motorNames[i] + ".setpoint", motors[i]->GetSetpoint());
				//SmartDashboard::PutNumber(motorNames[i] + ".encoder p", motors[i]->GetPosition());
				SmartDashboard::PutNumber(motorNames[i] + ".encoder v", motors[i]->GetEncVel());
				SmartDashboard::PutNumber(motorNames[i] + ".error", motors[i]->GetClosedLoopError());
			}
		}

	protected:
		// CAN sync group
		const uint8_t syncGroup = 0;
		const std::string motorNames[MOTOR_COUNT] = {"Left-front", "Right-front", "Left-rear", "Right-rear"};
		const double motorReversals[MOTOR_COUNT] = {-1.0, 1.0, -1.0, 1.0};
		SimplePID* x_ctrl;
		SimplePID* y_ctrl;
		SimplePID* r_ctrl;
		drivemode mode;
		//rps* positioner;
		CANTalon* motors[4];

	private:
		DISALLOW_COPY_AND_ASSIGN(MecanumDrive);
	};
}
