#include "XMLInput.h"

namespace Input
{
	motorBinding::motorBinding()
	{
		//Simple constructor for setting default, unworkable
		//(and thus detectable) values for the variables.
		inputID = 1;
		motorSlot = -1;
		deadzone = 0;
		activeCooldown = 0;
		cooldown = 50;
		type = axis;
		inverse = false;
		name = "NULL";
		enable = false;
		speed = 0.5;
		controller = NULL;
		motor = NULL;
	}
	motorBinding::~motorBinding()
	{
		delete motor;
	}

	//XMLInput member stuff
	XMLInput* XMLInput::singlePtr = NULL;
	XMLInput::XMLInput()
	{
		drivebase = NULL;
		for (int i = 0; i < MAX_CONTROLLERS; i++)
			controllers[i] = NULL;

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
	void XMLInput::updateMotors()
	{
		for (auto iter = motors.begin(); iter != motors.end(); iter++)
		{
			if (iter->controller == NULL || iter->motor == NULL) //Idiot check
				continue;

			if (iter->type == axis)
			{
				float input = iter->controller->GetRawAxis(iter->inputID);

				//Deadzone
				if (fabs(input) < iter->deadzone)
					input = 0;
				if (iter->inverse)
					input *= -1;

				//Commit motion to motor
				iter->motor->Set(input);
			}
			else
			{
				//Do a cooldown check. If cooldown is active, skip.
				if (iter->activeCooldown > 0 && iter->type != holdForActive)
				{
					iter->activeCooldown--;
					continue;
				}

				if (iter->type == toggle)
				{
					if (iter->controller->GetRawButton(iter->inputID))
						iter->enable = !iter->enable; //Flip the state of the motor
					if (iter->enable)
					{
						iter->motor->Set(iter->speed);
						iter->activeCooldown = iter->cooldown;
					}
				}
				else if (iter->type == stateChange)
				{
					if (iter->controller->GetRawButton(iter->inputID))
						iter->speed *= -1;
					iter->motor->Set(iter->speed);
					iter->activeCooldown = iter->cooldown;
				}
				else
				{ //Type = holdForActive
					if (iter->controller->GetRawButton(iter->inputID) && !iter->inverse)
					{
						iter->motor->Set(iter->speed);
						iter->activeCooldown = iter->cooldown;
					}
					else if (!iter->controller->GetRawButton(iter->inputID) && iter->inverse)
					{
						iter->motor->Set(iter->speed);
						iter->activeCooldown = iter->cooldown;
					}
				}
			}
		}
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

		if (drivebase != NULL) //Idiot check
			drivebase->Drive(xInput, yInput, rInput);
		//TODO: Add error logging - DriverStationLCD
	}
	Talon* XMLInput::getMotor(int ID)
	{ //Gets a motor pointer based on its motorSlot variable (mandatory)
		for (auto iter = motors.begin(); iter != motors.end(); iter++)
		{
			//Simple search. Since there won't be a huge amount of motors, this is
			//semi-OK. Still irksome though.
			if (iter->motorSlot == ID)
				return iter->motor;
		}
		return NULL; //Default oh-crap-I-screwed-up case.
	}
	Talon* XMLInput::getMotor(string searchName)
	{//Same as XMLInput::getMotor(int ID), but uses a name provided by the XML file.
		for (auto iter = motors.begin(); iter != motors.end(); iter++)
		{
			if (iter->name == searchName)
				return iter->motor;
		}
		return NULL;
	}
	Joystick* XMLInput::getController(int ID)
	{
		if (ID < MAX_CONTROLLERS && ID > -1)
			return controllers[ID];
		return NULL;
	}
	void XMLInput::loadXMLConfig(string filename)
	{
		//Clear anything currently present
		motors.clear();
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
		//drivebase->SetMotors(motorList[0], motorList[1], motorList[2], motorList[3]); //Use these motors as drivebase motors

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
			}
			else if (axisDir == "transX")
			{
				transXAxis = atoi(axis.child_value("ID"));
				transXDeadzone = atof(axis.child_value("deadzone"));
				break;
			}
			else if (axisDir == "rot")
			{
				rotAxis = atoi(axis.child_value("ID"));
				rotDeadzone = atof(axis.child_value("deadzone"));
				break;
			}
		}

		//Single motor loading
		pugi::xml_node indMotors = doc.child("IndMotors");
		for (auto motor = indMotors.child("motor"); motor; motor = motor.next_sibling())
		{
			//Assign motor
			motorBinding binding;
			binding.motorSlot = motor.attribute("ID").as_int();
			binding.motor = new Talon(binding.motorSlot);

			//Controller work - set the controller.
			binding.inputID = atoi(motor.child_value("inputID"));
			int contrID = atoi(motor.child_value("contrID"));
			if (controllers[contrID] != NULL)
				controllers[contrID] = new Joystick(contrID);
			binding.controller = controllers[contrID];

			//Set type and do type-specific work.
			string type = motor.attribute("type").as_string();
			if (type == "axis")
				binding.type = axis;
			if (type == "toggle")
				binding.type = toggle;
			if (type == "stateChange")
				binding.type = stateChange;
			if (type == "holdForActive")
				binding.type = holdForActive;

			if (binding.type == axis)
			{
				//Load deadzone information
				binding.deadzone = atof(motor.child_value("deadzone"));
			}
			else
			{
				//Load other information... hopefully, it won't freak out too badly
				//if information is missing... I hope.
				if (motor.child_value("inverse") == "true")
					binding.inverse = true;
				else
					binding.inverse = false;
				if (motor.child_value("enable") == "true")
					binding.enable = true;
				else
					binding.enable = false;
				binding.cooldown = atof(motor.child_value("cooldown"));
				binding.name = motor.attribute("name").as_string();
				binding.speed = atof(motor.child_value("speed"));
			}

			//Push the new control binding onto the binding list
			motors.push_back(binding);
		}
	}
};
