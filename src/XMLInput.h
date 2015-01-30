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

	class control
	{
	public:
		control();
		float speed;
		bindType controlType;	//The type of control this motor is bound to
		int inputID;			//The control that this motor is bound to.
		float deadzone;			//Deadzone for axis mode
		float activeCooldown;	//How much cooldown is actually left?
		float cooldown;			//Cooldown for button mode
		bool inverse;			//If axis, multiples value by -1. If button, inverts the output of the button or axis.
		bool enable;			//Used for stateChange and toggle modes
		Joystick* controller;
	};
	class binding
	{
	public:
		binding();
		~binding();
		virtual void update()=0;	//Updates the state of the output device
		int outputSlot;				//Used to get individual outputs, also a simple identifier.
		vector<control> inputs;		//All inputs bound to this output
	};
	class motorBinding : public binding
	{
	public:
		motorBinding();
		~motorBinding();
		void update();
		CANTalon* motor;
	};
	class pneumBinding : public binding
	{
	public:
		pneumBinding();
		~pneumBinding();
		void update();
		DoubleSolenoid::Value state;
		DoubleSolenoid* solenoid;
		int reverseSlot; //The outputSlot is assumed to be the forward ID.
	};

	class XMLInput
	{
	public:
		static XMLInput* getInstance();
		void setDrivebase(dreadbot::MecanumDrive* newDrivebase);
		void loadXMLConfig(string filename);
		void updateDrivebase();
		void updateInds();
		Joystick* getController(int ID);
		CANTalon* getMotor(int ID);
		DoubleSolenoid* getPneum(int ID);
	private:
		XMLInput();
		dreadbot::MecanumDrive* drivebase;
		static XMLInput* singlePtr;
		Joystick* controllers[MAX_CONTROLLERS];	//All pointers are *supposed* to be null unless they are in usage.
		CANTalon* motors[MAX_MOTORS];
		DoubleSolenoid* pneums[MAX_PNEUMS];

		vector<motorBinding> mBindings;
		vector<pneumBinding> pBindings;

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
