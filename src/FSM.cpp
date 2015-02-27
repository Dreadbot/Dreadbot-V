#include "FSM.h"

namespace dreadbot
{
	void FiniteStateMachine::init(FSMTransition* newStateTable, FSMState* initState)
	{
		stateTable = newStateTable;
		currentState = initState;
	}
	void FiniteStateMachine::update()
	{
		int input = currentState->update();
		SmartDashboard::PutNumber("update input", input);
		for (auto state = &this->stateTable[0]; state->currentState != nullptr; state++)
		{
			if (state->input == input && state->currentState == this->currentState)
			{
				if (state->action != nullptr)
					state->action(input, state->currentState, state->nextState); 
				this->currentState = state->nextState;
				SmartDashboard::PutBoolean("Some kind of state transition happened", true);
				break;
			}
		}
	}
}
