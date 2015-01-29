#include "mathutil.h"
#include <cmath>
#include "WPILib.h"

namespace dreadbot {
	// Stores position, velocity, rotational position, and rotational velocity
	struct inertial_frame {
		inertial_frame(Vector2<double> _position, Vector2<double> _velocity, double _rp, double _rv) {
			position = _position;
			velocity = _velocity;
			rp = _rp;
			rv = _rv;
		}
		Vector2<double> position; // position
		Vector2<double> velocity; // velocity
		double rp; // rotational position
		double rv; // rotational velocity
	};

	/**
	 * Robot positioning system
	 * Manages the built-in accelerometer and gyroscope, as well as integrates the
	 * accelerometer values to yield position and velocity.
	 * Noise accumulation shouldn't be much of an issue in 15 seconds of auton,
	 * but we can still apply a Madgewick filter if it becomes problematic.
	 */
	class rps {
	public:
		rps(uint16_t gyroCh) {
			accelerometer = new BuiltInAccelerometer();
			gyro = new Gyro(gyroCh);
			gyro->InitGyro();
			current_frame = new inertial_frame(Vector2<double>(0.0, 0.0), Vector2<double>(0.0, 0.0), 0.0, 0.0);
			integrationLoop = new Notifier(CallTracker, this);
			integrationLoop->StartPeriodic(0.01); // Built-in accelerometer outputs 800 signals per second
			// Integration period is less than that in order to reduce noise and to avoid hogging CPU
			loopTimer = new Timer();
			tracking_active = false;
		}

		void Start() { // Start tracker
			tracking_active = true;
			loopTimer->Start();
		}

		void Stop() {
			tracking_active = false;
			loopTimer->Stop();
		}

		void Reset() {
			gyro->Reset();
			current_frame = new inertial_frame(Vector2<double>(0.0, 0.0), Vector2<double>(0.0, 0.0), 0.0, 0.0);
		}

		inertial_frame GetTrackingData() {
			current_frame->rp = gyro->GetAngle();
			current_frame->rv = gyro->GetRate();
			return *current_frame;
		}

	private:
		void RunCalculations() {
			if (tracking_active) {
				double dt = loopTimer->Get();
				current_frame->velocity = current_frame->velocity + dt*Vector2<double>(accelerometer->GetX(), accelerometer->GetY());
				current_frame->position = current_frame->position + current_frame->velocity*dt;
				loopTimer->Stop();
				loopTimer->Reset();
				loopTimer->Start();
			}
		}

		static void CallTracker(void *controller) {
			rps *control = (rps*) controller;
			control->RunCalculations();
		}

		inertial_frame* current_frame;
		bool tracking_active;
		Timer* loopTimer;
		Notifier* integrationLoop;
		BuiltInAccelerometer* accelerometer;
		Gyro* gyro;
	};

}
