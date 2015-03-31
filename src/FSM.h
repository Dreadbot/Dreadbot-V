#pragma once

#include "WPILib.h"


#define END_STATE_TABLE {nullptr, 0, nullptr, nullptr}

namespace dreadbot {
	struct FSMState {
		virtual void enter() = 0;
		virtual int update() = 0;
		virtual ~FSMState() {}
	};

	struct FSMTransition {
		FSMState* currentState;
		int input;
		void (*action)(int input, FSMState* current, FSMState* nextState);
		FSMState* nextState;
	};

	class FiniteStateMachine {
	public:
		virtual void init(FSMTransition* newStateTable, FSMState* initState);
		virtual void update();
		virtual ~FiniteStateMachine() {}
	protected:
		FSMTransition* stateTable;
		FSMState* currentState;
	};
}
