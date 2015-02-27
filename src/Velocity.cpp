#include <iostream>
#include <cmath>
#include "Velocity.h"
#include <math.h>
#include <stdio.h>

namespace dreadbot
{
	/******************************************************************************
	**
	**  Velocity
	**
	******************************************************************************/
	Velocity::Velocity(void)
	{
		this->set(0, 0, 0);
		this->set_limit(0,0);
		this->label = "";
	}
	Velocity::Velocity(double x, double y, double r)
	{
		this->set(x, y, r);
		this->set_limit(0,0);
		this->label = "";
	}
	Velocity::Velocity(Velocity *v)
	{
		this->set(v->x, v->y, v->r);
		this->set_limit(0,0);
		this->label = "";
	}

	/*
	**  magnitude
	*/
	double Velocity::magnitude(void)
	{
		return sqrt( (this->x * this->x) + (this->y * this->y) );
	}
	double Velocity::rot_magnitude(void)
	{
		return fabs(this->r);
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
	void Velocity::set_limit(double t, double r)
	{
		this->limit_t = fabs(t);
		this->limit_r = fabs(r);
	}

	/*
	**  ramp_to
	*/
	void Velocity::ramp_to(double x, double y, double r)
	{
		Velocity v;
		v = Velocity(x,y,r);
		return this->ramp_to(&v);
	}
	void Velocity::ramp_to(Velocity *target)
	{
		Velocity new_vel = Velocity(target);
		Velocity *delta;
		double lim, mag, scale;

		delta = target->sub(this);

		if (this->limit_t != 0) {
			// Limit translational acceleration
			mag = delta->magnitude();
			lim = this->limit_t;
			if (mag > lim) {
				scale = lim / mag;
				new_vel.x = this->x + (scale * delta->x);
				new_vel.y = this->y + (scale * delta->y);
			}
		}

		if (this->limit_r != 0) {
			// Limit rotational acceleration
			mag = delta->rot_magnitude();
			lim = this->limit_r;
			if (mag > lim) {
				scale = lim / mag;
				new_vel.r = this->r + (scale * delta->r);
			}
		}

		this->set(&new_vel);
	}

	/*
	**  Put
	*/
	void Velocity::Put(void)
	{
		this->Put(this->label);
	}
	void Velocity::Put(std::string label)
	{
		char buf[80];
		sprintf(buf, "%0.6f, %0.6f, %0.6f", this->x, this->y, this->r);
		printf("%s: %s\n", label.c_str(), buf);
		//SmartDashboard::PutString(label.c_str(), buf);
	}

	/*
	**  operator +
	*/
	Velocity *Velocity::add(Velocity *rhs)
	{
		return new Velocity(this->x+rhs->x, this->y+rhs->y, this->r+rhs->r);
	}

	/*
	**  operator -
	*/
	Velocity *Velocity::sub(Velocity *rhs)
	{
		return new Velocity(this->x-rhs->x, this->y-rhs->y, this->r-rhs->r);
	}
};
