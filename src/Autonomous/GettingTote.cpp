#include "GettingTote.h"

namespace dreadbot
{
	GettingTote::GettingTote()
	{
		timerActive = false;
	}
	void GettingTote::enter()
	{
		lift->Set(1); //Raise the lift
		eStopTimer.Start(); //estop timer - if limit is passed, automatically estops the robot.
		sysLog->log("State: GettingTote");
	}
	int GettingTote::update()
	{
		if (isToteInTransit() && !timerActive) //Run once, upon tote contact with transit wheels.
		{
			//Stay still while a tote is loaded
			timerActive = true; //This doesn't really control a timer anymore...
			drivebase->Drive_v(0, 0, 0);
		}
		if (!isToteInTransit() && timerActive)
		{
			//Tote successfully collected - it should have just left contact with the transit wheels.
			intake->Set(0);
			timerActive = false;
			eStopTimer.Stop();
			eStopTimer.Reset();
			return HALBot::timerExpired;
		}
		if (timerActive) 
		{
			//Keep sucking a tote in
			if (HALBot::getToteCount() >= 2) 
				intake->Set(-1); 
			else 
				intake->Set(-0.5);

			return HALBot::no_update;
		}
		if (HALBot::getToteCount() != 0) //Open the intake arms for grabbing totes after the first tote is collected
			intakeArms->Set(-1);
	
		drivebase->Drive_v(0, -0.65, 0);
		if (HALBot::getToteCount() >= 2) 
			intake->Set(-1);
		else
			intake->Set(-0.6);

		if (eStopTimer.Get() >= 3.5)
			intakeArms->Set(1);
		else if (eStopTimer.Get() >= 2.5 && eStopTimer.Get() < 3.5)
			intakeArms->Set(0);

		// Emergeny stop in case the tote is missed.
		// Stop the drivebase, raise the lift, passify the intake arms, and stop the transit wheels.
		if (eStopTimer.Get() >= ESTOP_TIME) 
		{
			eStopTimer.Stop();
			eStopTimer.Reset();
			drivebase->Drive_v(0, 0, 0);intake->Set(0);
			lift->Set(1); // Raise the lift
			sysLog->log("Emergency-stopped in GettingTote", Hydra::error);
			return HALBot::eStop;
		}

		return HALBot::no_update;
	}	
}
