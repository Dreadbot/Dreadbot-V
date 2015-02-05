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

namespace Input
{
	const int MAX_CONTROLLERS = 5;
	const int MAX_MOTORS = 10;
	const int MAX_PNEUMS = 10;
	enum bindType {axis, toggle, stateChange, holdForActive};

	class SimpleMotor
	{
	public:
		SimpleMotor();
		void Set(float value);
	private:
		bool CAN;
		CANTalon* CANMotor;
		Talon* PWMMotor;
		friend class XMLInput;
	};


	class XMLInput
	{
	public:
		static XMLInput* getInstance();
		void setDrivebase(dreadbot::MecanumDrive* newDrivebase);
		void loadXMLConfig(string filename);
		void updateDrivebase();
		Joystick* getController(int ID);
		CANTalon* getMotor(int ID);
		Talon* getTMotor(int ID);
		DoubleSolenoid* getPneum(int ID);
	private:
		XMLInput();
		dreadbot::MecanumDrive* drivebase;
		static XMLInput* singlePtr;
		Joystick* controllers[MAX_CONTROLLERS];	//All pointers are *supposed* to be null unless they are in usage.
		CANTalon* motors[MAX_MOTORS];
		Talon* tMotors[MAX_MOTORS];
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
