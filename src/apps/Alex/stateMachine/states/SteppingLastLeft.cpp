#include "SteppingLastLeft.h"

void SteppingLastLeft::entry(void) {
    std::cout
        << "==================" << endl
        << " Stepping Last Left" << endl
        << "==================" << endl;
    /*/TODO CHANGE to selecting NORMALwALK to be from OD.mode traj param map equivalent*/
    /*ATM is just normal walk - choose from crutch*/
    trajectoryGenerator->initialiseTrajectory(RobotMode::NORMALWALK, robot->getJointStates());
    trajectoryGenerator->setTrajectoryStanceRight();
    robot->startNewTraj();
    CO_OD_RAM.currentState = (int)RobotMode::NORMALWALK;
}
void SteppingLastLeft::during(void) {
    robot->moveThroughTraj();
}
void SteppingLastLeft::exit(void) {
    DEBUG_OUT("EXITING STEPPING LAST LEFT")
}
