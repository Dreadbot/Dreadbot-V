#include "SimplePID.h"
#include "Notifier.h"
#include <cmath>

namespace dreadbot {

	/**
	 * Allocate a PID object with the given constants for P, I, D
	 * @param Kp the proportional coefficient
	 * @param Ki the integral coefficient
	 * @param Kd the derivative coefficient
	 * @param period the loop time for doing calculations. This particularly effects calculations of the
	 * integral and differental terms. The default is 50ms.
	 */
	SimplePID::SimplePID(double Kp, double Ki, double Kd, bool continuous, double period) {
		m_controlLoop = new Notifier(SimplePID::CallCalculate, this);
		m_P = Kp;
		m_I = Ki;
		m_D = Kd;
		m_maximumOutput = 1.0;
		m_minimumOutput = -1.0;
		m_maximumInput = 1.0;
		m_minimumInput = -1.0;
		m_continuous = false;
		m_enabled = false;
		m_setpoint = 0.0;
		m_error = 0.0;
		m_prevError = 0.0;
		m_totalError = 0.0;
		m_tolerance = 0.05;
		m_result = 0.0;
		m_period = period;
		m_measuredValue = 0.0;
		m_controlLoop->StartPeriodic(m_period);
	}

	SimplePID::~SimplePID() {
		delete m_controlLoop;
	}

	void SimplePID::CallCalculate(void *controller) {
		SimplePID *control = (SimplePID*) controller;
		control->Calculate();
	}

	void SimplePID::Calculate() {
		if (m_enabled) {
			double input = m_measuredValue;
			m_error = m_setpoint - input;
			if (m_continuous) {
				if (fabs(m_error) > (m_maximumInput - m_minimumInput) / 2) {
					if (m_error > 0)
						m_error = m_error - m_maximumInput + m_minimumInput;
					else
						m_error = m_error + m_maximumInput - m_minimumInput;
				}
			}
			if (((m_totalError + m_error) * m_I < m_maximumOutput) && ((m_totalError + m_error) * m_I > m_minimumOutput))
				m_totalError += m_error;
			m_result = m_P * m_error + m_I * m_totalError + m_D * (m_error - m_prevError);
			m_prevError = m_error;
			if (m_result > m_maximumOutput)
				m_result = m_maximumOutput;
			else if (m_result < m_minimumOutput)
				m_result = m_minimumOutput;
		}
	}

	void SimplePID::SetPID(double p, double i, double d) {
		m_P = p;
		m_I = i;
		m_D = d;
	}

	double SimplePID::GetOutput() {
		return m_result;
	}

	void SimplePID::SetMV(double mv) {
		m_measuredValue = mv;
	}

	void SimplePID::SetInputRange(double minimumInput, double maximumInput) {
		m_minimumInput = minimumInput;
		m_maximumInput = maximumInput;
		SetSetpoint(m_setpoint);
	}

	void SimplePID::SetOutputRange(double minimumOutput, double maximumOutput) {
		m_minimumOutput = minimumOutput;
		m_maximumOutput = maximumOutput;
	}

	void SimplePID::SetSetpoint(double setpoint) {
		if (m_maximumInput > m_minimumInput) {
			if (setpoint > m_maximumInput)
				m_setpoint = m_maximumInput;
			else if (setpoint < m_minimumInput)
				m_setpoint = m_minimumInput;
			else
				m_setpoint = setpoint;
		} else
			m_setpoint = setpoint;
	}

	void SimplePID::SetTolerance(double tolerance) {
		m_tolerance = tolerance;
	}

	bool SimplePID::OnTarget() {
		return (fabs(m_error) < m_tolerance * (m_maximumInput - m_minimumInput));;
	}

	void SimplePID::Enable() {
		m_enabled = true;
	}

	void SimplePID::Disable() {
		m_enabled = false;
	}

	void SimplePID::Reset() {
		Disable();
		m_prevError = 0;
		m_totalError = 0;
		m_result = 0;
	}
}
