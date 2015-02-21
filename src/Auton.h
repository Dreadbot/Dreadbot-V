/*

#pragma once

#include "WPILib.h"

namespace dreadbot {
  struct Goal {
    
  }
  class RobotState { // Describes a goal state for the robot
  public:
    RobotState(IterativeRobot* _robot, Goal* goal) {
      robot = _robot;
    }
    
    bool Accomplished(); // Returns true when the goal is accomplished
    void ExecuteAndYield(); // Pause execution until the goal is accomplished
    
  private:
    IterativeRobot* robot;
    Goal* goal;
  }
  
  class Auton {
  public:
    Auton(MecanumDrive* _drivebase, RPS* _tracker) {
      drivebase = _drivebase;
      tracker = _tracker;
    }
    
  private:
    MecanumDrive* drivebase;
    RPS* tracker;
    
    float TimeCheck(); // Return time until match ends
    
  }
}
*/
