#include "GettingTote.h"

namespace dreadbot
{
	GettingTote::GettingTote()
	{
		timerActive = false;
	}
	void GettingTote::enter()
	{
		raiseLift();
		eStopTimer.Start(); //estop timer - if limit is passed, automatically estops the robot.
		sysLog->log("State: GettingTote");
	}
	int GettingTote::update()
	{
		if (isToteInTransit() && !timerActive) //Run once, upon tote contact with transit wheels.
		{
			//Stay still while a tote is loaded
			timerActive = true; //This doesn't really control a timer anymore...
			drive(0, 0, 0);
		}
		if (!isToteInTransit() && timerActive)
		{
			//Tote successfully collected - it should have just left contact with the transit wheels.
			intake(0);
			timerActive = false;
			eStopTimer.Stop();
			eStopTimer.Reset();
			return RoboState::timerExpired;
		}
		if (timerActive) 
		{
			//Keep sucking a tote in
			if (RoboState::toteCount >= 2)
				intake(-1);
			else 
				intake(-0.5);

			return RoboState::no_update;
		}
		if (RoboState::toteCount != 0) //Open the intake arms for grabbing totes after the first tote is collected
			intakeArmsOut();
	
		drivebase->Drive_v(0, -0.65, 0);
		if (RoboState::toteCount >= 2)
			intake(-1);
		else
			intake(-0.6);

		if (eStopTimer.Get() >= 3.5)
			intakeArmsIn();
		else if (eStopTimer.Get() >= 2.5 && eStopTimer.Get() < 3.5)
			intakeArmsLax(); //this used to be an intakeArms->set(0). This *should* set the arms to lax... or something.

		// Emergeny stop in case the tote is missed.
		// Stop the drivebase, raise the lift, pacify the intake arms, and stop the transit wheels.
		if (eStopTimer.Get() >= ESTOP_TIME) 
		{
			eStopTimer.Stop();
			eStopTimer.Reset();
			drive(0, 0, 0);
			intake(0);
			raiseLift(); // Raise the lift
			sysLog->log("Emergency-stopped in GettingTote", Hydra::error);
			return RoboState::eStop;
		}

		return RoboState::no_update;
	}	
}
