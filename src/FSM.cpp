#include "FSM.h"

namespace dreadbot
{
	FiniteStateMachine::FiniteStateMachine(FSMTransition* newStateTable, FSMState* initState)
	{
		stateTable = newStateTable;
		currentState = initState;
	}
	void FiniteStateMachine::update()
	{
		int input = currentState->update();
		for (auto state = &stateTable[0]; state->currentState != nullptr; state++)
			if (state->input == input && state->currentState == currentState)
			{
				if (state->action != nullptr)
					state->action(input, state->currentState, state->nextState); 
				currentState = state->nextState;
			}
	}
}
