namespace dreadbot
{
	enum AutonMode {
		AUTON_MODE_STOP,	// Switch = 0
		AUTON_MODE_MOVE,	// Switch = 1
		AUTON_MODE_TOTE,	// Switch = 2
		AUTON_MODE_CONTAINER,	// Switch = 3
	};
	int GetAutonMode(void);
};
