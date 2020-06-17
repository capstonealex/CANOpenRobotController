////////// STATE ////////////////////
//-------  Sitting Down ------------/////
////////////////////////////////////
#include "SittingDwn.h"
void SittingDwn::entry(void) {
    std::cout << "Sitting Down State Entered " << endl
              << "===================" << endl
              << " GREEN -> SIT DOWN " << endl
              << "===================" << endl;
    robot->setPos(RobotMode::SITDWN);
    trajectoryGenerator->initialiseTrajectory(RobotMode::SITDWN, robot->getJointStates());
    robot->startNewTraj();
    DEBUG_OUT("INITIAL SITTING DOWN POS:")
    robot->printStatus();
}
void SittingDwn::during(void) {
    robot->moveThroughTraj();
}
void SittingDwn::exit(void) {
    DEBUG_OUT("EXIT SITTING DOWN POS:")
    robot->printStatus();
    std::cout
        << "Sitting Down State Exited " << endl;
}