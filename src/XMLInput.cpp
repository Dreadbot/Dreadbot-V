#include "XMLInput.h"

namespace dreadbot
{
	//SimplePneumatic stuff
	SimplePneumatic::SimplePneumatic()
	{
		invert = false;
		dPneumatic = NULL;
		sPneumatic = NULL;
		actionCount = 2;
	}
	void SimplePneumatic::Set(DoubleSolenoid::Value value)
	{
		if (invert)
		{
			if (value == DoubleSolenoid::kForward)
				value = DoubleSolenoid::kReverse;
			else if (value == DoubleSolenoid::kReverse)
				value = DoubleSolenoid::kForward;
		}
		if (actionCount > 1) //Is this a double solenoid?
			dPneumatic->Set(value);
		else if (value != DoubleSolenoid::kOff)
			sPneumatic->Set(true);
		else
			sPneumatic->Set(false);
	}

	//SimpleMotor stuff
	SimpleMotor::SimpleMotor()
	{
		CAN = false;
		invert = false;
		CANMotor = NULL;
		PWMMotor = NULL;
	}
	void SimpleMotor::Set(float value)
	{
		if (invert)
			value *= -1;

		if (CAN && CANMotor != NULL)
			CANMotor->Set(value);
		else if (!CAN && PWMMotor != NULL)
			PWMMotor->Set(value);
	}

	//MotorGrouping stuff
	MotorGrouping::MotorGrouping()
	{
		deadzone = 0;
	}
	void MotorGrouping::Set(float value)
	{
		if (fabs(value) < deadzone)
			value = 0; //Automatic deadzone processing
		for (auto iter = motors.begin(); iter != motors.end(); iter++)
			iter->Set(value);
		//Ta-da!
	}
	void MotorGrouping::SetDeadzone(float newDeadzone)
	{
		deadzone = fabs(newDeadzone); //No negative deadzones.
	}

	//PneumaticGrouping stuff
	void PneumaticGrouping::Set(DoubleSolenoid::Value value)
	{
		for (auto iter = pneumatics.begin(); iter != pneumatics.end(); iter++)
			iter->Set(value);
		//Ta-da!
	}
	void PneumaticGrouping::Set(float value)
	{
		//Deadzone processing
		if (fabs(value) < deadzone)
			value = 0;

		if (value == 0)
			Set(DoubleSolenoid::kOff);
		else if (value > 0)
			Set(DoubleSolenoid::kForward);
		else if (value < 0)
			Set(DoubleSolenoid::kReverse);
	}
	void PneumaticGrouping::SetDeadzone(float newDeadzone)
	{
		deadzone = fabs(newDeadzone);
	}

	//XMLInput stuff
	XMLInput* XMLInput::singlePtr = NULL;
	XMLInput::XMLInput()
	{
		drivebase = NULL;
		for (int i = 0; i < MAX_CONTROLLERS; i++)
			controllers[i] = NULL;
		for (int i = 0; i < MAX_MOTORS; i++)
		{
			canMotors[i] = NULL;
			pwmMotors[i] = NULL;
		}
		for (int i = 0; i < MAX_PNEUMS; i++)
		{
			dPneums[i] = NULL;
			sPneums[i] = NULL;
		}

		transXAxis = 0;
		transYAxis = 0;
		rotAxis = 0;
		transXDeadzone = 0;
		transYDeadzone = 0;
		rotDeadzone = 0;
		driveController = 1;
		invertX = false;
		invertY = false;
		invertR = false;
	}
	XMLInput* XMLInput::getInstance()
	{
		if (singlePtr == NULL)
			singlePtr = new XMLInput;
		return singlePtr;
	}
	void XMLInput::setDrivebase(dreadbot::MecanumDrive* newDrivebase)
	{
		drivebase = newDrivebase;
	}
	void XMLInput::updateDrivebase()
	{
		float xInput = controllers[driveController]->GetRawAxis(transXAxis);
		float yInput = controllers[driveController]->GetRawAxis(transYAxis);
		float rInput = controllers[driveController]->GetRawAxis(rotAxis);

		//Deadzones
		if (fabs(xInput) < transXDeadzone)
			xInput = 0;
		if (fabs(yInput) < transYDeadzone)
			yInput = 0;
		if (fabs(rInput) < rotDeadzone)
			rInput = 0;

		//Invert
		if (invertX)
			xInput = -xInput;
		if (invertY)
			yInput = -yInput;
		if (invertR)
			rInput = -rInput;

		if (drivebase != NULL) //Idiot check
			drivebase->Drive_v(xInput, yInput, rInput);
	}
	Joystick* XMLInput::getController(int ID)
	{
		if (ID < MAX_CONTROLLERS && ID > -1)
		{
			if (controllers[ID] == NULL)
				controllers[ID] = new Joystick(ID);
			return controllers[ID];
		}
		return NULL;
	}
	CANTalon* XMLInput::getCANMotor(int ID)
	{
		if (ID < MAX_CONTROLLERS - 1 && ID > -1)
		{
			if (canMotors[ID] == NULL)
				canMotors[ID] = new CANTalon(ID);
			return canMotors[ID];
		}
		return NULL;
	}
	Talon* XMLInput::getPWMMotor(int ID)
	{
		if (ID < MAX_CONTROLLERS - 1 && ID > -1)
		{
			if (pwmMotors[ID] == NULL)
				pwmMotors[ID] = new Talon(ID);
			return pwmMotors[ID];
		}
		return NULL;
	}
	DoubleSolenoid* XMLInput::getDPneum(int forwardID)
	{
		if (forwardID < MAX_PNEUMS - 1 && forwardID > -1)
			return dPneums[forwardID];
		return NULL;
	}

	Solenoid* XMLInput::getSPneum(int ID)
	{
		if (ID > MAX_PNEUMS - 1 || ID < 0)
			return NULL;
		if (sPneums[ID] == NULL)
			sPneums[ID] = new Solenoid(ID);
		return sPneums[ID];
	}
	MotorGrouping* XMLInput::getMGroup(string name)
	{
		for (auto iter = mGroups.begin(); iter != mGroups.end(); iter++)
			if (iter->name == name)
				return &(*iter);
		return NULL;
	}
	PneumaticGrouping* XMLInput::getPGroup(string name)
	{
		for (auto iter = pGroups.begin(); iter != pGroups.end(); iter++)
			if (iter->name == name)
				return &(*iter);
		return NULL;
	}
	void XMLInput::loadXMLConfig(string filename)
	{
		pGroups.clear();
		mGroups.clear();

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(filename.c_str());
		SmartDashboard::PutString("XML Load Result: ", result.description());

		//Load drivebase motors
		int motorList[4];
		pugi::xml_node base = doc.child("Dreadbot").child("Drivebase");
		for (auto motor = base.child("motors").child("motor"); motor; motor = motor.next_sibling())
		{
			string motorPos = motor.attribute("position").as_string();

			if (motorPos == "frontLeft")
				motorList[0] = atoi(motor.child_value());
			else if (motorPos == "frontRight")
				motorList[1] = atoi(motor.child_value());
			else if (motorPos == "backLeft")
				motorList[2] = atoi(motor.child_value());
			else if (motorPos == "backRight")
				motorList[3] = atoi(motor.child_value());
		}
		drivebase->Set(motorList[0], motorList[1], motorList[2], motorList[3]);

		//Drivebase control loading - rig joystick
		int controlID = base.child("controller").attribute("controllerID").as_int();
		if (controllers[controlID] == NULL)
			controllers[controlID] = new Joystick(controlID);
		driveController = controlID;

		//Drivebase control loading - get axes
		string invert;
		for (auto axis = base.child("controller").child("axis"); axis; axis = axis.next_sibling())
		{
			string axisDir = axis.attribute("dir").as_string();
			invert = axis.child_value("invert");
			if (axisDir == "transY")
			{
				transYAxis = atoi(axis.child_value("ID"));
				transYDeadzone = atof(axis.child_value("deadzone"));
				if (invert.find("true")) //I really don't understand how this works...
					invertY = false;
				else
					invertY = true;
			}
			else if (axisDir == "transX")
			{
				transXAxis = atoi(axis.child_value("ID"));
				transXDeadzone = atof(axis.child_value("deadzone"));
				if (invert.find("true"))
					invertX = false;
				else
					invertX = true;
			}
			else if (axisDir == "rot")
			{
				rotAxis = atoi(axis.child_value("ID"));
				rotDeadzone = atof(axis.child_value("deadzone"));
				if (invert.find("true"))
					invertR = false;
				else
					invertR = true;
			}
		}

		//Load all motor groups
		pugi::xml_node motorgroups = doc.child("Dreadbot").child("motorgroups");
		for (auto motorgroup = motorgroups.child("group"); motorgroup; motorgroup = motorgroup.next_sibling())
		{
			//Assign group information
			MotorGrouping newMGroup;
			newMGroup.name = motorgroup.attribute("name").as_string();
			//Check for duplicate motor groups, and output if there are
			for (auto iter = mGroups.begin(); iter != mGroups.end(); iter++)
				if (iter->name == newMGroup.name)
					SmartDashboard::PutBoolean("Duplicate Motor Groups", true);

			newMGroup.deadzone = fabs(motorgroup.attribute("deadzone").as_float());
			for (auto motor = motorgroup.child("motor"); motor; motor = motor.next_sibling())
			{
				//Get motor-specific information and assign motor pointers
				SimpleMotor newMotor;
				newMotor.CAN = motor.attribute("CAN").as_bool();
				if (newMotor.CAN)
					newMotor.CANMotor = getCANMotor(motor.attribute("outputID").as_int());
				else
					newMotor.PWMMotor = getPWMMotor(motor.attribute("outputID").as_int());
				newMotor.invert = motor.attribute("invert").as_bool();
				newMGroup.motors.push_back(newMotor);
			}
			mGroups.push_back(newMGroup);
		}

		//Load all pneumatic groups
		pugi::xml_node pneumgroups = doc.child("Dreadbot").child("pneumaticgroups");
		for (auto pneumgroup = pneumgroups.child("group"); pneumgroup; pneumgroup = pneumgroup.next_sibling())
		{
			PneumaticGrouping newPGroup;
			newPGroup.name = pneumgroup.attribute("name").as_string();
			//Check for duplicate motor groups, and output if there are
			for (auto iter = pGroups.begin(); iter != pGroups.end(); iter++)
				if (iter->name == newPGroup.name)
					SmartDashboard::PutBoolean("Duplicate Pneumatic Groups", true);

			newPGroup.deadzone = fabs(pneumgroup.attribute("deadzone").as_float());

			for (auto pneumatic = pneumgroup.child("dsolenoid"); pneumatic; pneumatic = pneumatic.next_sibling())
			{
				SimplePneumatic newPneum;
				newPneum.invert = pneumatic.attribute("invert").as_bool();
				newPneum.actionCount = pneumatic.attribute("actionCount").as_int();

				if (newPneum.actionCount > 1) //Is this a double solenoid?
				{
					int forwardID = pneumatic.attribute("forwardID").as_int();
					int reverseID = pneumatic.attribute("reverseID").as_int();
					if (getDPneum(forwardID) != NULL)
						newPneum.dPneumatic = getDPneum(forwardID);
					else
					{
						newPneum.dPneumatic = new DoubleSolenoid(forwardID, reverseID);
						dPneums[forwardID] = newPneum.dPneumatic;
					}
				}
				else //This is a single solenoid
					newPneum.sPneumatic = getSPneum(pneumatic.attribute("ID").as_int());
				newPGroup.pneumatics.push_back(newPneum);
			}
			pGroups.push_back(newPGroup);
		}
	}
};
