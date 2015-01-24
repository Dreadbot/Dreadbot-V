#include "XMLInput.h"

namespace Input
{
	control::control()
	{
		controlType = axis;
		inputID = -1;
		deadzone = 0.025f;
		activeCooldown = 0;
		cooldown = 0;
		inverse = false;
		enable = true;
		controller = NULL;
	}
	binding::binding()
	{
		outputSlot = -1;
	}
	binding::~binding()
	{

	}
	motorBinding::motorBinding()
	{
		outputSlot = -1;
		speed = 0.25f;
		motor = NULL;
	}
	motorBinding::~motorBinding()
	{
	}
	pneumBinding::pneumBinding()
	{
		outputSlot = -1;
		reverseSlot = -2;
		state = DoubleSolenoid::kOff;
		solenoid = NULL;
	}
	pneumBinding::~pneumBinding()
	{
	}
	void motorBinding::update()
	{
		for (auto iter = inputs.begin(); iter != inputs.end(); iter++)
		{
			float input = 0;
			//For axes
			if (iter->controlType == axis)
			{
				//Get actual input
				input = iter->controller->GetRawAxis(iter->inputID);
				if (iter->inverse) input = -1;

				//Deadzone
				if (fabs(input) < fabs(iter->deadzone))
					input = 0;
			}
			//For buttons
			else
			{
				//This is probably a button. Probably.
				if (iter->activeCooldown > 0)
				{
					iter->activeCooldown--;
					continue;
				}
				iter->activeCooldown = abs(iter->cooldown); //Set a cooldown so the button can't be spammed. abs() is a stupid check - a check for stupid.
				bool bInput = iter->controller->GetRawButton(iter->inputID);

				//Inverse button output
				if (iter->inverse)
					bInput = !bInput;
				input = (int) bInput;

			}

			//Apply inputs to actual output
			if (iter->controlType == axis)
				motor->Set(input);
			else
			{
				if (iter->controlType == holdForActive && input)
					motor->Set(speed);
				if (iter->controlType == stateChange)
				{
					speed = -speed;
					motor->Set(speed);
				}
				if (iter->controlType == toggle)
				{
					iter->enable = !-iter->enable;
					motor->Set(speed);
				}
			}
		}
	}
	void pneumBinding::update()
	{
		for (auto iter = inputs.begin(); iter != inputs.end(); iter++)
		{
			float input = 0;
			//For axes
			if (iter->controlType == axis)
			{
				//Get actual input
				input = iter->controller->GetRawAxis(iter->inputID);
				if (iter->inverse) input = -1;
				//Deadzone
				if (fabs(input) < fabs(iter->deadzone))
					input = 0;
			}
			//For buttons
			else
			{
				//This is probably a button. Probably.
				if (iter->activeCooldown > 0)
				{
					iter->activeCooldown--;
					continue;
				}
				iter->activeCooldown = abs(iter->cooldown); //Set a cooldown so the button can't be spammed. abs() is a stupid check - a check for stupid.
				bool bInput = iter->controller->GetRawButton(iter->inputID);

				//Inverse button output
				if (iter->inverse)
					bInput = !bInput;
				input = (int) bInput;

			}

			//Apply settings
			if (iter->controlType == axis)
			{
				if (input > 0)
					solenoid->Set(DoubleSolenoid::kForward);
				else if (input < 0)
					solenoid->Set(DoubleSolenoid::kReverse);
				else
					solenoid->Set(DoubleSolenoid::kOff);
			}
			else
			{
				//This is a button. Almost certainly.
				if (iter->controlType == holdForActive && input)
					solenoid->Set(state);
				else if (iter->controlType == toggle)
				{
					if (state == DoubleSolenoid::kForward)
						state = DoubleSolenoid::kReverse;
					else
						state = DoubleSolenoid::kForward;
					solenoid->Set(state);
				}
			}
		}
	}

	//XMLInput member stuff
	XMLInput* XMLInput::singlePtr = NULL;
	XMLInput::XMLInput()
	{
		drivebase = NULL;
		for (int i = 0; i < MAX_CONTROLLERS; i++)
			controllers[i] = NULL;
		for (int i = 0; i < MAX_MOTORS; i++)
			motors[i] = NULL;
		for (int i = 0; i < MAX_PNEUMS; i++)
			pneums[i] = NULL;

		transXAxis = 0;
		transYAxis = 0;
		rotAxis = 0;
		transXDeadzone = 0;
		transYDeadzone = 0;
		rotDeadzone = 0;
		driveController = 1;
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

		//if (drivebase != NULL) //Idiot check
		//	drivebase->Drive(xInput, yInput, rInput);
		SmartDashboard::PutNumber("xInput:", xInput);
		SmartDashboard::PutNumber("yInput:", yInput);
		SmartDashboard::PutNumber("rInput:", rInput);
	}
	void XMLInput::updateInds()
	{
		for (auto iter = mBindings.begin(); iter != mBindings.end(); iter++)
			iter->update();
		for (auto iter = pBindings.begin(); iter != pBindings.end(); iter++)
			iter->update();
	}
	Joystick* XMLInput::getController(int ID)
	{
		if (ID < MAX_CONTROLLERS && ID > -1)
			return controllers[ID];
		return NULL;
	}
	Talon* XMLInput::getMotor(int ID)
	{
		if (ID < MAX_CONTROLLERS - 1 && ID > -1)
			return motors[ID];
		return NULL;
	}
	DoubleSolenoid* XMLInput::getPneum(int ID)
	{
		if (ID < MAX_CONTROLLERS - 1 && ID > -1)
			return pneums[ID];
		return NULL;
	}
	void XMLInput::loadXMLConfig(string filename)
	{
		//Clear anything currently present
		mBindings.clear();
		pBindings.clear();
		for (int i = 0; i < MAX_CONTROLLERS; i++)
		{
			delete controllers[i];
			controllers[i] = NULL;
		}

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
		drivebase->Set(motorList[0], motorList[1], motorList[2], motorList[3]); //Use these motors as drivebase motors

		//Drivebase control loading - rig joystick
		int controlID = base.child("controller").attribute("ID").as_int();
		if (controllers[controlID] == NULL)
			controllers[controlID] = new Joystick(controlID);
		driveController = controlID;

		//Drivebase control loading - get axes
		for (auto axis = base.child("controller").child("axis"); axis; axis = axis.next_sibling())
		{
			string axisDir = axis.attribute("dir").as_string();
			if (axisDir == "transY")
			{
				transYAxis = atoi(axis.child_value("ID"));
				transYDeadzone = atof(axis.child_value("deadzone"));
				if (axis.child_value("invert") == "true")
					invertY = true;
				else
					invertY = false;
			}
			else if (axisDir == "transX")
			{
				transXAxis = atoi(axis.child_value("ID"));
				transXDeadzone = atof(axis.child_value("deadzone"));
				if (axis.child_value("invert") == "true")
					invertX = true;
				else
					invertX = false;
			}
			else if (axisDir == "rot")
			{
				rotAxis = atoi(axis.child_value("ID"));
				rotDeadzone = atof(axis.child_value("deadzone"));
				if (axis.child_value("invert") == "true")
					invertR = true;
				else
					invertR = false;
			}
		}

		//Single motor loading
		pugi::xml_node XMLMotors = doc.child("motors");
		for (auto motor = XMLMotors.child("motor"); motor; motor = motor.next_sibling())
		{
			motorBinding newMotor;
			newMotor.outputSlot = motor.attribute("outputID").as_int();
			//Create motor object
			if (motors[newMotor.outputSlot] == NULL)
				motors[newMotor.outputSlot] = new Talon(newMotor.outputSlot);
			newMotor.motor = motors[newMotor.outputSlot];
			newMotor.speed = atof(motor.child_value("speed"));

			//Load control settings
			for (auto XMLControl = motor.child("controls").child("control"); XMLControl; XMLControl = XMLControl.next_sibling())
			{
				control newControl;

				//Assign joystick
				int controllerID = XMLControl.attribute("controllerID").as_int();
				if (controllers[controllerID] == NULL)
					controllers[controllerID] = new Joystick(controllerID);
				newControl.controller = controllers[controllerID];

				newControl.cooldown = atoi(XMLControl.child_value("cooldown"));
				newControl.deadzone = atof(XMLControl.child_value("deadzone"));
				newControl.inputID =  atoi(XMLControl.child_value("inputID"));
				if (XMLControl.child_value("invert") == "true")
					newControl.inverse = true;
				else
					newControl.inverse = false;

				string controlType = XMLControl.attribute("type").as_string();
				if (controlType == "axis")
					newControl.controlType = axis;
				else if (controlType == "toggle")
					newControl.controlType = toggle;
				else if (controlType == "stateChange")
					newControl.controlType = stateChange;
				else if (controlType == "holdForActive")
					newControl.controlType = holdForActive;

				newMotor.inputs.push_back(newControl);
			}
		}

		//Single pneumatic loading
		pugi::xml_node XMLPneums = doc.child("pneumatics");
		for (auto pneum = XMLPneums.child("pneumatic"); pneum; pneum = pneum.next_sibling())
		{
			pneumBinding newPneum;
			newPneum.outputSlot = pneum.attribute("forwardID").as_int();
			newPneum.reverseSlot = pneum.attribute("reverseID").as_int();
			//Create motor object
			if (pneums[newPneum.outputSlot] == NULL)
				pneums[newPneum.outputSlot] = new DoubleSolenoid(newPneum.outputSlot, newPneum.reverseSlot);
			newPneum.solenoid = pneums[newPneum.outputSlot];
			string state = pneum.child_value("state");
			if (state == "kOff")
				newPneum.state = DoubleSolenoid::kOff;
			if (state == "kForward")
				newPneum.state = DoubleSolenoid::kForward;
			if (state == "kReverse")
				newPneum.state = DoubleSolenoid::kReverse;

			//Load control settings
			for (auto XMLControl = pneum.child("controls").child("control"); XMLControl; XMLControl = XMLControl.next_sibling())
			{
				control newControl;

				//Assign joystick
				int controllerID = XMLControl.attribute("controllerID").as_int();
				if (controllers[controllerID] == NULL)
					controllers[controllerID] = new Joystick(controllerID);
				newControl.controller = controllers[controllerID];

				newControl.cooldown = atoi(XMLControl.child_value("cooldown"));
				newControl.deadzone = atof(XMLControl.child_value("deadzone"));
				newControl.inputID =  atoi(XMLControl.child_value("inputID"));
				if (XMLControl.child_value("invert") == "true")
					newControl.inverse = true;
				else
					newControl.inverse = false;

				string controlType = XMLControl.attribute("type").as_string();
				if (controlType == "axis")
					newControl.controlType = axis;
				else if (controlType == "toggle")
					newControl.controlType = toggle;
				else if (controlType == "stateChange")
					newControl.controlType = stateChange;
				else if (controlType == "holdForActive")
					newControl.controlType = holdForActive;

				newPneum.inputs.push_back(newControl);
			}
		}
	}
}

