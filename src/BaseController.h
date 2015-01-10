#pragma once
#include <WPILib.h>
#include "Keycodes.h"

enum controlType {dmode, xmode};

class BaseController
{
public:

	BaseController(int controlID = 0);
	~BaseController();

	void setControlSlot(int newSlot = 0);
	void getControlSlot();
	float getAxisState(int axisID);
	bool getButtonState(int buttonID);
	void setControlType(controlType newType);
	void getControlType();

protected:
	Joystick* controller;
	controlType type;
	int slot;
	int* axisVals;
	bool* buttonVals;
};
