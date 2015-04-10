#pragma once
#include <WPILib.h>
#include "../lib/pugixml.hpp"
#include "MecanumDrive.h"
#include <string>
#include <map>
#include <vector>
using std::string;
using std::vector;
using std::map;

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

	const int VEL_DEADZONE = 0.05;

	class SimplePneumatic
	{
	public:
		SimplePneumatic();
		void Set(DoubleSolenoid::Value value); //!< Smart set - automatically handles for invert
	private:
		DoubleSolenoid* dPneumatic;
		Solenoid* sPneumatic;
		int actionCount;
		bool invert; //If true, the given directions are inverted. Forward, Backward = Backward, Forward. On, Off = Off, On
		friend class XMLInput;
	};
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
	class PneumaticGrouping
	{
	public:
		void Set(DoubleSolenoid::Value value); //!< Passes the set value to all pneumatics in the group
		void Set(float value);
		void SetDeadzone(float newDeadzone);
	protected:
		string name; //!< Used to identify this pneumatic group
		vector<SimplePneumatic> pneumatics;
		float deadzone;
		friend class XMLInput;
	};
	class MotorGrouping
	{
	public:
		MotorGrouping();
		void Set(float value); //!< Passes the set value to all motors in the group
		void SetDeadzone(float newDeadzone); //!< Sets a deadzone that is handled automatically by the Set() function.
	protected:
		string name; //!< Used to identify this motor group
		vector<SimpleMotor> motors;
		float deadzone;
		friend class XMLInput;
	};

	//Singleton class for managing pretty much all motors, pneumatics, and controllers.
	class XMLInput
	{
	public:
		static XMLInput* getInstance();
		void setDrivebase(MecanumDrive* newDrivebase); //Sets the drivebase that velocity information is sent to.
		void loadXMLConfig(); //Clears previous configuration and loads from the XML doc in Config.h
		void updateDrivebase(); //Handels all drivebase-related stuff, including inverts, deadzones, and the sensativity curve.
		Joystick* getController(int ID); //!< Gets a joystick with the given ID. If joystick does not exist, creates joystick with ID and returns it.
		CANTalon* getCANMotor(int ID); //!< Gets a CANTalon with the given ID. If the CANTalon does not exist, creates CANTalon with ID and returns it.
		Talon* getPWMMotor(int ID); //!< Gets a Talon with the given ID. If the Talon does not exist, creates CANTalon with ID and returns it.
		DoubleSolenoid* getDPneum(int forwardID); //!< Gets a DoubleSolenoid based on the ID. The ID is for the FORWARD output thingy.
		Solenoid* getSPneum(int ID); //!< Gets a single solenoid based on the ID.
		PneumaticGrouping* getPGroup(string name); //Find a pneumatic grouping by name (found in Config.h)
		MotorGrouping* getMGroup(string name); //Find a motor grouping by name (found in Config.h)
	private:
		XMLInput();

		map<string, PneumaticGrouping> pGroups;
		map<string, MotorGrouping> mGroups;

		MecanumDrive* drivebase;
		static XMLInput* singlePtr;
		Joystick* controllers[MAX_CONTROLLERS];	//All pointers are *supposed* to be null unless they are in usage.
		CANTalon* canMotors[MAX_MOTORS];
		Talon* pwmMotors[MAX_MOTORS];
		DoubleSolenoid* dPneums[MAX_PNEUMS];
		Solenoid * sPneums[MAX_PNEUMS];

		//Axis stuff for drivebase-specific controls
		int driveController;
		enum dirCodes {x, y, r};
		int axes[3];
		bool inverts[3];
		float deadzones[3];;

		DISALLOW_COPY_AND_ASSIGN(XMLInput); //Prevents copying/assigning - critical for a singleton. That's a cool macro.
	};
}
