#pragma once

namespace dreadbot
{
	class Velocity
	{
	public:
		double x;
		double y;
		double r;
		Velocity *limit;

		Velocity(void);
		Velocity(double x, double y, double r);
		Velocity(Velocity *v);

		double magnitude(void);
		double rot_magnitude(void);

		void set(double x, double y, double r);
		void set(Velocity *v);

		void set_limit(double x, double y, double r);
		void set_limit(Velocity *v);

		void ramp_to(double x, double y, double r);
		void ramp_to(Velocity *v);

		Velocity& operator+(const Velocity& lhs, const Velocity& rhs);
		Velocity& operator+=(const Velocity& rhs);
		Velocity& operator-(const Velocity& lhs, const Velocity& rhs);
		Velocity& operator-=(const Velocity& rhs);

	protected:
	private:
	};
}
