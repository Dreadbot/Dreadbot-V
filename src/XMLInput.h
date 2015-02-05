#pragma once
#include <WPILib.h>
#include "../lib/pugixml.hpp"
#include "MecanumDrive.h"
#include <string>
#include <vector>
using std::string;
using std::vector;


/*
 * Uses an XML config file to create custom control settings
 * Documentation coming soon. Functions should be self
 * explanatory, but the XML config stuff itself needs some
 * documentation
 */

namespace dreadbot
{
	const int MAX_CONTROLLERS = 5;
	const int MAX_MOTORS = 10;
	const int MAX_PNEUMS = 10;
	enum bindType {axis, toggle, stateChange, holdForActive};

	class SimpleMotor
	{
	public:
		SimpleMotor();
		void Set(float value); //!< Smart set - automatically handles for invert, CAN/PWM controls. Give it a value and go.
	private:
		bool CAN;
		bool invert;
		CANTalon* CANMotor;
		Talon* PWMMotor;
		friend class XMLInput;
	};
	struct MotorGrouping
	{
		vector<SimpleMotor> motors;
		string name;
		void Set(float value); //!< Passes a the set value to all motors in the group
	};

	class XMLInput
	{
	public:
		static XMLInput* getInstance();
		void setDrivebase(MecanumDrive* newDrivebase);
		void loadXMLConfig(string filename);
		void updateDrivebase();
		Joystick* getController(int ID);
		CANTalon* getCANMotor(int ID);
		Talon* getPWMMotor(int ID);
		DoubleSolenoid* getPneum(int ID);
	private:
		XMLInput();
		MecanumDrive* drivebase;
		static XMLInput* singlePtr;
		Joystick* controllers[MAX_CONTROLLERS];	//All pointers are *supposed* to be null unless they are in usage.
		CANTalon* canMotors[MAX_MOTORS];
		Talon* pwmMotors[MAX_MOTORS];
		DoubleSolenoid* pneums[MAX_PNEUMS];

		//Axis stuff for drivebase-specific controls
		int transXAxis;
		int transYAxis;
		int rotAxis;
		bool invertX;
		bool invertY;
		bool invertR;
		float transXDeadzone;
		float transYDeadzone;
		float rotDeadzone; //Rot Deadzone. I call that as a band name.
		int driveController;

		DISALLOW_COPY_AND_ASSIGN(XMLInput); //Prevents copying/assigning - critical for a singleton. That's a cool macro.
	};
};
