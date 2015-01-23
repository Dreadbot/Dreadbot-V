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
		type = tMotor;
		name = "Not a Control";
	}
	binding::~binding()
	{

	}
	motorBinding::motorBinding()
	{
		outputSlot = -1;
		type = tMotor;
		name = "Not a Control";
		speed = 0.25f;
		motor = NULL;
	}
	motorBinding::~motorBinding()
	{
		delete motor;
	}
	pneumBinding::pneumBinding()
	{
		outputSlot = -1;
		type = pneumatic;
		name = "Not a Control";
		state = DoubleSolenoid::kOff;
		solenoid = NULL;
	}
	pneumBinding::~pneumBinding()
	{
		delete solenoid;
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
				if (iter->controlType == holdForActive)
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
				if (iter->controlType == holdForActive)
					solenoid->Set(state);
				else if (iter->controlType == toggle)
				{
					if (state == DoubleSolenoid::kForward)
						state = DoubleSolenoid::kReverse;
					else
						state = DoubleSolenoid::kForward;
					solenoid->set(state);
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

		if (drivebase != NULL) //Idiot check
			drivebase->Drive(xInput, yInput, rInput);
		//TODO: Add error logging - DriverStationLCD
	}
	Joystick* XMLInput::getController(int ID)
	{
		if (ID < MAX_CONTROLLERS && ID > -1)
			return controllers[ID];
		return NULL;
	}
//	void XMLInput::loadXMLConfig(string filename)
//	{
//		//Clear anything currently present
//		controls.clear();
//		for (int i = 0; i < MAX_CONTROLLERS; i++)
//		{
//			delete controllers[i];
//			controllers[i] = NULL;
//		}
//
//		pugi::xml_document doc;
//		pugi::xml_parse_result result = doc.load_file(filename.c_str());
//		SmartDashboard::PutString("XML Load Result: ", result.description());
//
//		//Load drivebase motors
//		int motorList[4];
//		pugi::xml_node base = doc.child("Dreadbot").child("Drivebase");
//		for (auto motor = base.child("motors").child("motor"); motor; motor = motor.next_sibling())
//		{
//			string motorPos = motor.attribute("position").as_string();
//
//			if (motorPos == "frontLeft")
//				motorList[0] = atoi(motor.child_value());
//			else if (motorPos == "frontRight")
//				motorList[1] = atoi(motor.child_value());
//			else if (motorPos == "backLeft")
//				motorList[2] = atoi(motor.child_value());
//			else if (motorPos == "backRight")
//				motorList[3] = atoi(motor.child_value());
//		}
//		drivebase->Set(motorList[0], motorList[1], motorList[2], motorList[3]); //Use these motors as drivebase motors
//
//		//Drivebase control loading - rig joystick
//		int controlID = base.child("controller").attribute("ID").as_int();
//		if (controllers[controlID] == NULL)
//			controllers[controlID] = new Joystick(controlID);
//		driveController = controlID;
//
//		//Drivebase control loading - get axes
//		for (auto axis = base.child("controller").child("axis"); axis; axis = axis.next_sibling())
//		{
//			string axisDir = axis.attribute("dir").as_string();
//			if (axisDir == "transY")
//			{
//				transYAxis = atoi(axis.child_value("ID"));
//				transYDeadzone = atof(axis.child_value("deadzone"));
//			}
//			else if (axisDir == "transX")
//			{
//				transXAxis = atoi(axis.child_value("ID"));
//				transXDeadzone = atof(axis.child_value("deadzone"));
//			}
//			else if (axisDir == "rot")
//			{
//				rotAxis = atoi(axis.child_value("ID"));
//				rotDeadzone = atof(axis.child_value("deadzone"));
//			}
//		}
//
//		//Single motor loading
//		pugi::xml_node indMotors = doc.child("IndMotors");
//		for (auto motor = indMotors.child("motor"); motor; motor = motor.next_sibling())
//		{
//			//Assign motor
//			motorBinding binding;
//			binding.motorSlot = motor.attribute("ID").as_int();
//			binding.motor = new Talon(binding.motorSlot);
//
//			//Controller work - set the controller.
//			binding.inputID = atoi(motor.child_value("inputID"));
//			int contrID = atoi(motor.child_value("contrID"));
//			if (controllers[contrID] != NULL)
//				controllers[contrID] = new Joystick(contrID);
//			binding.controller = controllers[contrID];
//
//			//Set type and do type-specific work.
//			string type = motor.attribute("type").as_string();
//			if (type == "axis")
//				binding.type = axis;
//			if (type == "toggle")
//				binding.type = toggle;
//			if (type == "stateChange")
//				binding.type = stateChange;
//			if (type == "holdForActive")
//				binding.type = holdForActive;
//
//			if (binding.type == axis)
//			{
//				//Load deadzone information
//				binding.deadzone = atof(motor.child_value("deadzone"));
//			}
//			else
//			{
//				//Load other information... hopefully, it won't freak out too badly
//				//if information is missing... I hope.
//				if (motor.child_value("inverse") == "true")
//					binding.inverse = true;
//				else
//					binding.inverse = false;
//				if (motor.child_value("enable") == "true")
//					binding.enable = true;
//				else
//					binding.enable = false;
//				binding.cooldown = atof(motor.child_value("cooldown"));
//				binding.name = motor.attribute("name").as_string();
//				binding.speed = atof(motor.child_value("speed"));
//			}
//
//			//Push the new control binding onto the binding list
//			motors.push_back(binding);
//		}
//	}
}
