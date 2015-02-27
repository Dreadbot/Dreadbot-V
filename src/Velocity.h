#pragma once

namespace dreadbot
{
	class Velocity
	{
	public:
		double x;
		double y;
		double r;
		double limit_t;
		double limit_r;
		std::string label;

		Velocity(void);
		Velocity(double x, double y, double r);
		Velocity(Velocity *v);

		double magnitude(void);
		double rot_magnitude(void);

		void set(double x, double y, double r);
		void set(Velocity *v);

		void set_limit(double t, double r);

		void ramp_to(double x, double y, double r);
		void ramp_to(Velocity *v);

		void Put(void);
		void Put(std::string label);

		Velocity *add(Velocity *rhs);
		Velocity *sub(Velocity *rhs);

	protected:
	private:
	};
}
