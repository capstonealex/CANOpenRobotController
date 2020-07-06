#include "SteppingLastLeft.h"

void SteppingLastLeft::entry(void) {
    std::cout
        << "==================" << endl
        << " Stepping Last Left" << endl
        << "==================" << endl;
    /*/TODO CHANGE to selecting NORMALwALK to be from OD.mode traj param map equivalent*/
    /*ATM is just normal walk - choose from crutch*/
    robot->setPos(RobotMode::NORMALWALK, Foot::Right); /*Testing Virtual*/
    trajectoryGenerator->initialiseTrajectory(RobotMode::NORMALWALK, robot->getJointStates());
    robot->startNewTraj();
    robot->copleyDrives[0]->setNextMotion(RobotMode::NORMALWALK);
}
void SteppingLastLeft::during(void) {
    robot->moveThroughTraj();
}
void SteppingLastLeft::exit(void) {
    DEBUG_OUT("EXITING STEPPING LAST LEFT")
}
