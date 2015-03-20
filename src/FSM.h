#pragma once
#include <iostream>
#include "WPILib.h"
#include "SmartDashboard/SmartDashboard.h"

#define END_STATE_TABLE {nullptr, 0, nullptr, nullptr}

namespace dreadbot
{
	class FSMState
	{
	public:
		virtual void enter() = 0;
		virtual int update() = 0;
	};

	class FSMTransition
	{
	public:
		FSMState* currentState;
		int input;
		void (*action)(int input, FSMState* current, FSMState* nextState);
		FSMState* nextState;
	};

	class FiniteStateMachine
	{
	public:
		virtual void init(FSMTransition* newStateTable, FSMState* initState);
		virtual void update();
	protected:
		FSMTransition* stateTable;
		FSMState* currentState;
	};
}
