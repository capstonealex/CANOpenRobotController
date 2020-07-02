#include "ExoTestState.h"

ExoTestState::ExoTestState(StateMachine *m, AlexRobot *exo, AlexTrajectoryGenerator *tg, const char *name) : State(m, name), robot(exo), trajectoryGenerator(tg){};

void ExoTestState::updateCrutch() {
    RobotMode modeSelected = robot->getNextMotion();
    if (modeSelected != robot->getCurrentMotion()) {
        std::cout << "Setting current Mode to:" << robot->pb.printRobotMode(modeSelected) << std::endl;
        //update current mode to send out to crutch
        robot->setCurrentMotion(modeSelected);
    }
}
void ExoTestState::updateFlag() {
    if (robot->getResetFlag() == false && robot->getGo() == false) {
        DEBUG_OUT("Reset Trajectory FLAG SET to TRUE!")
        robot->setResetFlag(true);
    }
}