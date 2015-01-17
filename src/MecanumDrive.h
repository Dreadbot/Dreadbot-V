// Parker Stebbins/1.13.2015

#pragma once

#include "WPILib.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include "mathutil.h"


#define __SQUAREINPUTS 1


namespace dreadbot {
	class MecanumDrive {
	public:
		enum class drivemode {
			absolute, // Position mode
			relative  // Velocity mode
		};

		enum motorType : uint8_t {
			leftFront = 0,
			rightFront = 1,
			leftRear = 2,
			rightRear = 3
		};

		MecanumDrive(int adr_0, int adr_1, int adr_2, int adr_3) {
			mode = drivemode::relative;

			motors[leftFront] = new CANTalon(adr_0);
			motors[rightFront] = new CANTalon(adr_1);
			motors[leftRear] = new CANTalon(adr_2);
			motors[rightRear] = new CANTalon(adr_3);

			motors[leftFront]->SetSensorDirection(true);
			motors[rightFront]->SetSensorDirection(false);
			motors[leftRear]->SetSensorDirection(true);
			motors[rightRear]->SetSensorDirection(false);

			for (uint8_t i = 0; i < 4; ++i) {
				motors[i]->SetPosition(0.0);
				motors[i]->ConfigMaxOutputVoltage(12.0);
				motors[i]->ConfigEncoderCodesPerRev(1024);
				motors[i]->SelectProfileSlot(0);
			}

		}

		void Set(int adr_0, int adr_1, int adr_2, int adr_3) {
			for (int i = 0; i < 4; i++)
				delete motors[i];
			mode = drivemode::relative;

			motors[leftFront] = new CANTalon(adr_0);
			motors[rightFront] = new CANTalon(adr_1);
			motors[leftRear] = new CANTalon(adr_2);
			motors[rightRear] = new CANTalon(adr_3);

			motors[leftFront]->SetSensorDirection(true);
			motors[rightFront]->SetSensorDirection(false);
			motors[leftRear]->SetSensorDirection(true);
			motors[rightRear]->SetSensorDirection(false);

			for (uint8_t i = 0; i < 4; ++i) {
				motors[i]->SetPosition(0.0);
				motors[i]->ConfigMaxOutputVoltage(12.0);
				motors[i]->ConfigEncoderCodesPerRev(1024);
				motors[i]->SelectProfileSlot(0);
			}

		}

		~MecanumDrive() {
			for (uint8_t i = 0; i < 4; ++i) {
				delete motors[i];
			}
		}

		void Drive(double x, double y, double rotation) {
			Vector2<double> vec_out(x, -y);
			double rot_out;

			if (mode == drivemode::absolute) {
				//vec_out.rotate((double) gyroscope->GetAngle());
				// yield thread until the robot has passed within a certain tolerance of the target?
				// a callback might work as well
			} else if (mode == drivemode::relative) {
			#ifdef __SQUARE_INPUTS
				vec_out = vec_out*vec_out;
			#endif
				rot_out = rotation;
			}

			double wspeeds[4] = {
				vec_out.x + vec_out.y + rot_out,
				-vec_out.x + vec_out.y - rot_out,
				-vec_out.x + vec_out.y + rot_out,
				vec_out.x + vec_out.y - rot_out
			};

			double maxMagnitude = *std::max_element(wspeeds, wspeeds + 4);
			if (maxMagnitude > 1.0) {
			    for (uint8_t i = 0; i < 4; ++i) {
			        wspeeds[i] /= maxMagnitude;
			    }
			}

			// check for stalled motors so we don't burn up or lose a pinion
			bool stall = false;
			for (uint8_t i = 0; i < 4; ++i) {
				// stall current of a 2.5" CIM is 133 amps
				// files.andymark.com/CIM-motor-curve.pdf
				stall = stall || (motors[i]->GetOutputCurrent() > 30);
				stall |= stall;
			}

			for (uint8_t i = 0; i < 4; ++i) {
				motors[i]->Set(wspeeds[i]*(!stall), syncGroup);
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
			for (uint8_t i = 0; i < 4; ++i) {
				motors[i]->SetControlMode(cmode);
			}
		}

		void Engage() {
			for (uint8_t i = 0; i < 4; ++i) {
				motors[i]->EnableControl();
			}
		}

		void Disengage() {
			for (uint8_t i = 0; i < 4; ++i) {
				motors[i]->Disable();
			}
		}

		void SetPID(double p, double i, double d) {
			for (uint8_t i = 0; i < 4; ++i) {
				motors[i]->SetPID(p, i, d);
			}
		}

		void SDGetPID() {
			SetPID (SmartDashboard::GetNumber("P", 0.5),
					SmartDashboard::GetNumber("I", 0.0),
					SmartDashboard::GetNumber("D", 0.0));
		}

		void SDOutputDiagnostics() {
			for (uint8_t i = 0; i < 4; ++i) {
				SmartDashboard::PutNumber(motorNames[i] + " temperature", motors[i]->GetTemperature());
				SmartDashboard::PutNumber(motorNames[i] + " encoder position", motors[i]->GetPosition());
				SmartDashboard::PutNumber(motorNames[i] + " encoder velocity", motors[i]->GetSpeed());
				SmartDashboard::PutNumber(motorNames[i] + " bus voltage", motors[i]->GetBusVoltage());
				SmartDashboard::PutNumber(motorNames[i] + " output current", motors[i]->GetOutputCurrent());
				SmartDashboard::PutNumber(motorNames[i] + " error", motors[i]->GetClosedLoopError());
				SmartDashboard::PutBoolean(motorNames[i] + " alive", motors[i]->IsAlive());
				SmartDashboard::PutNumber(motorNames[i] + " firmware version", motors[i]->GetFirmwareVersion());
			}
		}

	protected:
		static const uint8_t syncGroup = 0x00;
		const std::string motorNames[4] = {
			"Left-front",
			"Right-front",
			"Left-rear",
			"Right-rear"
		};

		drivemode mode;
		CANTalon* motors[4];

	private:
		DISALLOW_COPY_AND_ASSIGN(MecanumDrive);
	};
}
