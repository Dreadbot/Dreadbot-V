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

	class SimplePneumatic
	{
	public:
		SimplePneumatic();
		void Set(DoubleSolenoid::Value value); //!< Smart set - automatically handles for invert
	private:
		DoubleSolenoid* dPneumatic;
		Solenoid* sPneumatic;
		int actionCount;
		bool invert;
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

	class XMLInput
	{
	public:
		static XMLInput* getInstance();
		void setDrivebase(MecanumDrive* newDrivebase);
		void loadXMLConfig(string filename);
		void updateDrivebase();
		Joystick* getController(int ID); //!< Gets a joystick with the given ID. If joystick does not exist, creates joystick with ID and returns it.
		CANTalon* getCANMotor(int ID); //!< Gets a CANTalon with the given ID. If the CANTalon does not exist, creates CANTalon with ID and returns it.
		Talon* getPWMMotor(int ID); //!< Gets a Talon with the given ID. If the Talon does not exist, creates CANTalon with ID and returns it.
		DoubleSolenoid* getDPneum(int forwardID); //!< Gets a DoubleSolenoid based on the ID. The ID is for the FORWARD output thingy.
		Solenoid* getSPneum(int ID); //!< Gets a single solenoid based on the ID.
		PneumaticGrouping* getPGroup(string name);
		MotorGrouping* getMGroup(string name);
	private:
		XMLInput();

		vector<PneumaticGrouping> pGroups;
		vector<MotorGrouping> mGroups;

		MecanumDrive* drivebase;
		static XMLInput* singlePtr;
		Joystick* controllers[MAX_CONTROLLERS];	//All pointers are *supposed* to be null unless they are in usage.
		CANTalon* canMotors[MAX_MOTORS];
		Talon* pwmMotors[MAX_MOTORS];
		DoubleSolenoid* dPneums[MAX_PNEUMS];
		Solenoid * sPneums[MAX_PNEUMS];

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
}
