// Bare-bones PID controller object

#pragma once

#include "Base.h"

class Notifier;

namespace dreadbot {
	class SimplePID {
	private:
		double m_P;			// factor for "proportional" control
		double m_I;			// factor for "integral" control
		double m_D;			// factor for "derivative" control
		double m_maximumOutput;	// |maximum output|
		double m_minimumOutput;	// |minimum output|
		double m_maximumInput;		// maximum input - limit setpoint to this
		double m_minimumInput;		// minimum input - limit setpoint to this
		bool m_continuous;	// do the endpoints wrap around? eg. Absolute encoder
		bool m_enabled; 			//is the pid controller enabled
		double m_prevError;	// the prior sensor input (used to compute velocity)
		double m_totalError; //the sum of the errors for use in the integral calc
		double m_tolerance;	//the percetage error that is considered on target
		double m_setpoint;
		double m_error;
		double m_result;
		double m_period;
		double m_measuredValue; //the last value recorded from the measurement device.
		Notifier *m_controlLoop;
		static void CallCalculate(void *controller);
		void Calculate();
		//DISALLOW_COPY_AND_ASSIGN(SimplePID);

	public:
		SimplePID(double p, double i, double d, bool continuous, double period = 0.05);
		~SimplePID();
		double GetOutput();
		void SetInputRange(double minimumInput, double maximumInput);
		void SetOutputRange(double mimimumOutput, double maximumOutput);
		void SetPID(double p, double i, double d);
		void SetMV(double mv);
		void SetSetpoint(double setpoint);
		double GetSetpoint();
		double GetError();
		void SetTolerance(double percent);
		bool OnTarget();
		void Enable();
		void Disable();
		void Reset();
	};
}
