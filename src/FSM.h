#pragma once
#include <iostream>

#define END_STATE_TABLE {nullptr, 0, nullptr, nullptr}

namespace dreadbot
{
	class FSMState
	{
	public:
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
