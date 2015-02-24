#include <string>
using std::string;


#define MULTILINE(...) #__VA_ARGS__

string config = MULTILINE(

<?xml version="1.0" encoding="UTF-8"?>
<!--SET CONTROLLER TO XMODE!-->
<Dreadbot version="5.0">
	<Drivebase type="Mecanum">
		<motors>
			<motor position="frontLeft">1</motor>
			<motor position="frontRight">2</motor>
			<motor position="backLeft">3</motor>
			<motor position="backRight">4</motor>
		</motors>
		<controller controllerID="0">
			<axis dir="transY">
				<ID>1</ID>
				<deadzone>0.1</deadzone>
				<accel>0.25</accel>
				<invert>false</invert>
			</axis>
			<axis dir="transX">
				<ID>0</ID>
				<deadzone>0.1</deadzone>
				<accel>0.25</accel>
				<invert>false</invert>
			</axis>
			<axis dir="rot">
				<ID>4</ID>
				<deadzone>0.1</deadzone>
				<accel>0.25</accel>
				<invert>false</invert>
			</axis>
		</controller>
	</Drivebase>
	<motorgroups>
		<group name="intake" deadzone="0.1">
			<motor outputID="0" invert="true" CAN="false"></motor>
			<motor outputID="1" invert="false" CAN="false"></motor>
			<motor outputID="2" invert="false" CAN="false"></motor>
			<motor outputID="3" invert="true" CAN="false"></motor>
		</group>
	</motorgroups>
	<pneumaticgroups>
		<group name="lift" deadzone="0.1">
			<dsolenoid actionCount="2" forwardID="0" reverseID="1" invert="false"></dsolenoid>
		</group>
		<group name="liftArms" deadzone="0.1">
			<dsolenoid actionCount="1" ID="2" invert="false"></dsolenoid>
		</group>
		<group name="intakeArms" deadzone="0.1">
			<dsolenoid actionCount="2" forwardID="3" reverseID="4" invert="false"></dsolenoid>
		</group>
	</pneumaticgroups>
</Dreadbot>

);
