#include <cmath>
#include <thread>
#include "WPILib.h"
#include "mathutil.h"
#include "Vector2.h"

namespace dreadbot {
	// Stores position, velocity, rotational position, and rotational velocity
	struct intertial_frame {
		intertial_frame(Vector2<double> _position, Vector2<double> _velocity, double _rp, double _rv) {
			position = _position;
			velocity = _velocity;
			rp = _rp;
			rv = _rv;
		}
		Vector2<double> ip; // position
		Vector2<double> iv; // velocity
		double rp; // rotational position
		double rv; // rotational velocity
	};

	// Robot inertial tracking system
	class rps {
	public:
		rps(uint16_t gyroCh) {
			accelerometer = new BuiltInAccelerometer();
			gyro = new Gyro(gyroCh);
			Reset();
		}

		void Start() { // Start tracker
			tracking_active = true;
			gyro->InitGyro();
		}

		void Reset() {
			gyro->Reset();

			intertial_frame current_point = new intertial_frame(Vector2<double>(0.0, 0.0), Vector2<double>(0.0, 0.0), 0.0, 0.0);
		}

		intertial_frame* GetTrackingData() {
			Vector2<double> result_pos = accelerometer->GetPosition();
			return new intertial_frame(result_pos, accelerometer->GetVelocity(), gyro->GetAngle(), gyro->GetRate());
		}

	private:
		intertial_frame current_point;
		bool tracking_active;
		BuiltInAccelerometer* accelerometer;
		Gyro* gyro;
	};

}
