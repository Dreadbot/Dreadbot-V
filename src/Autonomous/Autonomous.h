#
namespace dreadbot 
{
	//States
	class GettingTote : public FSMState
	{
	public:
		GettingTote();
		virtual void enter();
		virtual int update();
 		void setHardware(MecanumDrive* newDrivebase, MotorGrouping* newIntake);
	private:
		MecanumDrive* drivebase;
		MotorGrouping* intake;
		bool timerActive;
		Timer getTimer;
		Timer eStopTimer;
	};
	class DriveToZone : public FSMState
	{
	public:
		DriveToZone();
		virtual void enter();
		virtual int update();
		void setHardware(MecanumDrive* newDrivebase);
		Timer driveTimer;
		int dir;
		bool strafe;
	protected:
		MecanumDrive* drivebase;
		bool timerActive;
	};
	class ForkGrab : public FSMState
	{
	public:
		ForkGrab();
		virtual void enter();
		virtual int update();
		Timer grabTimer;
		PneumaticGrouping* lift;
		MecanumDrive* drivebase;
	protected:
		bool timerActive;
	};
	class Rotate : public DriveToZone
	{
	public:
		Rotate();
		virtual void enter();
		virtual int update();

		int rotateConstant;
	};
	class Stopped : public FSMState
	{
	public:
		virtual void enter();
		virtual int update();
		PneumaticGrouping* lift;
		MecanumDrive* drivebase;
	};
	class PushContainer : public DriveToZone
	{
	public:
		PushContainer();
		virtual void enter();
		virtual int update();
		Talon* pusher1;
		Talon* pusher2;
		int pushConstant;
		bool enableScaling;
	};
	class BackAway : public ForkGrab
	{
	public:
		virtual void enter();
		virtual int update();
		MecanumDrive* drivebase;
	};
	class RotateDrive : public Rotate
	{
	public:
		RotateDrive();
		int update();
		void enter();
	};
	class StrafeLeft : public DriveToZone
	{
	public:
		StrafeLeft();
		int update();
		void enter();
	};

	//Needs to appear in https://en.wikipedia.org/wiki/Kludge#Computer_science
	class HALBot
	{
	public:
		enum fsmInputs {no_update, finish, timerExpired, nextTote, eStop};

		HALBot();
		~HALBot();
		static bool enoughTotes(); //Stupid global way of determining if the robot has acquired enough totes. Kludgish, really.
		static void incrTote(); //Increment the collected tote cout.
		static int getToteCount(); //Gets the tote count. Used in a few kludgish areas. Also stupid.
		void setMode(AutonMode newMode); //Called during AutonomousInit. Determines what autonomous mode to run.
		AutonMode getMode(); //Gets the mode. Used in AutonomousInit. Also really, really dumb/stupid.
		void init(MecanumDrive* drivebase, MotorGrouping* intake, PneumaticGrouping* lift); //Sets hardware, intializes stuff, and prepares the transition tables. Assumes that the setMode thing has been used already.
		void update(); //Basically just a cheap call to FiniteStateMachine::update. 
	private:
		static int toteCount; //How many totes have been acquired. 
		FiniteStateMachine* fsm;
		static AutonMode mode; //The mode that the robot is in. Also dumb.

		FSMTransition transitionTable[15]; //The transition table used for transitioning. Changes based on the setMode thingy.

		//State objects. These should be self-explanatory.
		GettingTote* gettingTote; 
		DriveToZone* driveToZone;
		ForkGrab* forkGrab;
		Rotate* rotate;
		Rotate* rotate2;
		Stopped* stopped;
		PushContainer* pushContainer;
		BackAway* backAway;
		RotateDrive* rotateDrive;
		StrafeLeft* strafeLeft;
	};
}
