#include <cmath>

namespace dreadbot
{
	/******************************************************************************
	**
	**  Velocity
	**
	******************************************************************************/
	Velocity::Velocity(void)
	{
		Velocity(0, 0, 0);
	}
	Velocity::Velocity(double x, double y, double r)
	{
		this->set(x, y, r);
		this->limit = nullptr;
	}
	Velocity::Velocity(Velocity *v)
	{
		Velocity(v->x, v->y, v->r);
	}

	/*
	**  magnitude
	*/
	double Velocity::magnitude(void)
	{
		return std::cmath::sqrt( (this->x * this->x) + (this->y * this->y) );
	}
	double Velocity::rot_magnitude(void)
	{
		return std::cmath::abs(this->r);
	}

	/*
	** set
	*/
	void Velocity::set(double x, double y, double r)
	{
		this->x = x;
		this->y = y;
		this->r = r;
	}
	void Velocity::set(Velocity *v)
	{
		return this->set(v->x, v->y, v->r);
	}

	/*
	** set_limit
	*/
	void Velocity::set_limit(double x, double y, double r)
	{
		this->limit = new Velocity(x, y, r);
	}
	void Velocity::set_limit(Velocity *v)
	{
		this->limit = v;
	}

	/*
	**  ramp_to
	*/
	void Velocity::ramp_to(double x, double y, double r)
	{
		return this->ramp_to(new Velocity(x,y,r));
	}
	void Velocity::ramp_to(Velocity *target)
	{
		Velocity *new_vel = new Velocity(target);
		Velocity *delta   = new Velocity(0,0,0);
		double lim, mag, scale;

		if (this->limit != nullptr) {
			delta = target - this;

			// Limit translational acceleration
			mag = delta->magnitude();
			lim = this->limit->magnitude();
			if (mag > lim) {
				scale = lim / mag;
				new_vel->x = this->x + (scale * delta->x);
				new_vel->y = this->y + (scale * delta->y);
			}

			// Limit rotational acceleration
			mag = delta->rot_magnitude();
			lim = this->limit->rot_magnitude();
			if (mag > lim) {
				scale = lim / mag;
				new_vel->r = this->r + (scale * delta->r);
			}
		}

		this->set(new_vel);
	}

	void Velocity->SmartDashboard(char *label)
	{
		char buf[80];
		sprintf(buf, "%0.6f, %0.6f, %0.6f", this->x, this->y, this->r);
		SmartDashboard::PutString(label, buf);
	}

	/*
	**  operator +
	*/
	Velocity& operator+(const Velocity& lhs, const Velocity& rhs)
	{
		return new Velocity(lhs->x+rhs->x, lhs->y+rhs->y, lhs->r+rhs->r);
	}
	Velocity& operator+=(const Velocity& rhs)
	{
		this->x += rhs->x;
		this->y += rhs->y;
		this->r += rhs->r;
		return *this;
	}

	/*
	**  operator -
	*/
	Velocity& operator-(const Velocity& lhs, const Velocity& rhs)
	{
		return new Velocity(lhs->x-rhs->x, lhs->y-rhs->y, lhs->r-rhs->r);
	}
	Velocity& operator-=(const Velocity& rhs)
	{
		this->x -= rhs->x;
		this->y -= rhs->y;
		this->r -= rhs->r;
		return *this;
	}
