#pragma once

#include "WPILib.h"
#include <algorithm>
#include <cmath>
#include "mathutil.h"
#include <vector>
//#include "Tracker.h"


#define SQUARE_INPUTS
#define OUTPUT_EVERYTHING

#define MOTOR_COUNT 4
#define STALL_MOTOR_CURRENT 40


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

		MecanumDrive(int motorId_lf, int motorId_rf, int motorId_lr, int motorId_rr) {
			mode = drivemode::relative;

			motors[m_leftFront] = new CANTalon(motorId_lf);
			motors[m_rightFront] = new CANTalon(motorId_rf);
			motors[m_leftRear] = new CANTalon(motorId_lr);
			motors[m_rightRear] = new CANTalon(motorId_rr);

			motors[m_leftFront]->SetSensorDirection(true);
			motors[m_rightFront]->SetSensorDirection(false);
			motors[m_leftRear]->SetSensorDirection(true);
			motors[m_rightRear]->SetSensorDirection(false);

			// Configure loop parameters
			for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
				motors[i]->SetPosition(0.0);
				motors[i]->ConfigEncoderCodesPerRev(1024);
				motors[i]->SelectProfileSlot(0);
			}
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
			double rot_out;

			if (mode == drivemode::relative) {
				#ifdef SQUARE_INPUTS
					vec_out = vec_out*vec_out;
				#endif
				rot_out = rotation;
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

			//soft breaker
			bool stall = false;
			for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
				// files.andymark.com/CIM-motor-curve.pdf
				stall = stall || (motors[i]->GetOutputCurrent() > STALL_MOTOR_CURRENT);
				stall |= stall;
			}
			#ifdef OUTPUT_EVERYTHING
				SmartDashboard::PutBoolean("Overcurrent", stall);
			#endif
			for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
				#ifdef OUTPUT_EVERYTHING
					SmartDashboard::PutNumber(motorNames[i] + ".SP", wspeeds[i]);
				#endif
				motors[i]->Set(wspeeds[i]*(!stall)*motorReversals[i], syncGroup);
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

		void SetPID(double p, double i, double d) {
			for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
				motors[i]->SetPID(p, i, d);
			}
		}

		void SD_RetrievePID() {
			SetPID (SmartDashboard::GetNumber("P", 0.5),
					SmartDashboard::GetNumber("I", 0.0),
					SmartDashboard::GetNumber("D", 0.0));
		}

		void SD_OutputDiagnostics() {
			for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
				SmartDashboard::PutNumber(motorNames[i] + ".temp", motors[i]->GetTemperature());
				SmartDashboard::PutNumber(motorNames[i] + ".encoder p", motors[i]->GetPosition());
				SmartDashboard::PutNumber(motorNames[i] + ".encoder v", motors[i]->GetSpeed());
				SmartDashboard::PutNumber(motorNames[i] + ".output current", motors[i]->GetOutputCurrent());
				SmartDashboard::PutNumber(motorNames[i] + ".PID error", motors[i]->GetClosedLoopError());
				SmartDashboard::PutBoolean(motorNames[i] + ".alive", motors[i]->IsAlive());
			}
		}

	protected:
		// CAN sync group
		const uint8_t syncGroup = 0x00;

		const std::string motorNames[MOTOR_COUNT] = {"Left-front", "Right-front", "Left-rear", "Right-rear"};

		// specify reverse outputs ((2*(!(i%2)))-1)
		const int motorReversals[MOTOR_COUNT] = {1, -1, 1, -1};

		// command method.
		drivemode mode;
		CANTalon* motors[4];

	private:
		DISALLOW_COPY_AND_ASSIGN(MecanumDrive);
	};


	// Assign multiple motors to one output value.
	class MotorGroup {
	public:
		explicit MotorGroup(std::vector<uint16_t> addresses) {
			motors();
			for (uint16_t i = 0; i <= addresses.size(); ++i) {
				motors[i] = new CANTalon(addresses[i]);
			}
		}

		void Set(float sp, uint8_t syncGroup) {
			for (uint16_t i = 0; i < motors.size(); ++i) {
				motors[i]->Set(sp, syncGroup);
			}
		}

		void SetEnabled(bool enabled) {
			for (uint16_t i = 0; i < motors.size(); ++i) {
				if (enabled) {
					motors[i]->EnableControl();
				} else {
					motors[i]->Disable();
				}
			}
		}
	private:
		std::vector<CANTalon*> motors;
	};
}
