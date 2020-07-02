#include "SteppingFirstLeft.h"

void SteppingFirstLeft::entry(void) {
    std::cout
        << "==================" << endl
        << " Stepping 1st Left" << endl
        << "==================" << endl;
    /*/TODO CHANGE to selecting NORMALwALK to be from OD.mode traj param map equivalent*/
    /*ATM is just normal walk - choose from crutch*/
    /*MUST HAVE A CHECK THAT Its the correct motion here as well - or throw an error and don't move!*/
    trajectoryGenerator->initialiseTrajectory(robot->getNextMotion(), robot->getJointStates());
    robot->startNewTraj();
    robot->setCurrentState(AlexState::StepFirstL);
}
void SteppingFirstLeft::during(void) {
    robot->moveThroughTraj();
}
void SteppingFirstLeft::exit(void) {
    DEBUG_OUT("EXITING STEPPING FIRST LEFT")
}
