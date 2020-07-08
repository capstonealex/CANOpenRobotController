#include "AlexRobot.h"

#include "DebugMacro.h"

AlexRobot::AlexRobot(AlexTrajectoryGenerator *tj) {
    trajectoryGenerator = tj;
}
AlexRobot::AlexRobot(){
    DEBUG_OUT("EXO ROBOT CONSTRUCTOR")}

AlexRobot::~AlexRobot() {
    DEBUG_OUT("Delete AlexRobot object begins")
    freeMemory();
    joints.clear();
    copleyDrives.clear();
    DEBUG_OUT("AlexRobot deleted")
}

bool AlexRobot::initPositionControl() {
    DEBUG_OUT("Initialising Position Control on all joints ")
    bool returnValue = true;
    for (auto p : joints) {
        if (((ActuatedJoint *)p)->setMode(POSITION_CONTROL, posControlMotorProfile) != POSITION_CONTROL) {
            // Something back happened if were are here
            DEBUG_OUT("Something bad happened")
            returnValue = false;
        }
        // Put into ReadyToSwitchOn()
        ((ActuatedJoint *)p)->readyToSwitchOn();
    }

    // Pause for a bit to let commands go
    usleep(2000);
    for (auto p : joints) {
        ((ActuatedJoint *)p)->enable();
    }
    usleep(2000);
    for (auto p : joints) {
        ((AlexJoint *)p)->enableContinuousProfile();
    }
    return returnValue;
}

bool AlexRobot::initTorqueControl() {
    DEBUG_OUT("Initialising Torque Control on all joints ")
    bool returnValue = true;
    for (auto p : joints) {
        if (((ActuatedJoint *)p)->setMode(TORQUE_CONTROL) != TORQUE_CONTROL) {
            // Something back happened if were are here
            DEBUG_OUT("Failed to initialize Torque Control")
            returnValue = false;
        }
        // Put into ReadyToSwitchOn()
        ((ActuatedJoint *)p)->readyToSwitchOn();
    }

    // Pause for a bit to let commands go
    usleep(2000);
    for (auto p : joints) {
        ((ActuatedJoint *)p)->enable();
    }
    return returnValue;
}
void AlexRobot::startNewTraj() {
    DEBUG_OUT("Start New Traj");
    // Index Resetting
    currTrajProgress = 0;
    clock_gettime(CLOCK_MONOTONIC, &prevTime);
    DEBUG_OUT("EXIT Start New Traj");
}

bool AlexRobot::moveThroughTraj() {
    bool returnValue = true;
    timespec currTime;
    clock_gettime(CLOCK_MONOTONIC, &currTime);

    double elapsedSec = currTime.tv_sec - prevTime.tv_sec + (currTime.tv_nsec - prevTime.tv_nsec) / 1e9;
    double trajTimeUS = trajectoryGenerator->getStepDuration();
    prevTime = currTime;
    // This should check to make sure that the "GO" button is pressed.
    if (true) {
        currTrajProgress += elapsedSec;
        double fracTrajProgress = currTrajProgress / trajTimeUS;
        std::vector<double> setPoints;
        try {
            setPoints = trajectoryGenerator->getSetPoint(fracTrajProgress);
        } catch (std::domain_error) {
            //run alex -> trigger feet together movemnt (safety position)
        }
        int i = 0;
        std::cout << currTrajProgress << " , ";
        for (auto p : joints) {
            std::cout << rad2deg(setPoints[i]) << ",";
            setMovementReturnCode_t setPosCode = ((ActuatedJoint *)p)->setPosition(rad2deg(setPoints[i]));
            if (setPosCode == INCORRECT_MODE) {
                std::cout << "Joint ID: " << p->getId() << ": is not in Position Control " << std::endl;
                returnValue = false;
            } else if (setPosCode != SUCCESS) {
                // Something bad happened
                std::cout << "Joint " << p->getId() << ": Unknown Error " << std::endl;
                returnValue = false;
            }
            i++;
        }
        std::cout << std::endl;
    } else {
        DEBUG_OUT("Not moving")
    }

    return returnValue;
}

bool AlexRobot::initialiseJoints() {
    JointKnownPos hipParam{250880, 0, 90, 180};
    JointKnownPos kneeParam{250880, 0, 90, 0};
    JointKnownPos ankleParam{0, -800000, 90, 115};

    for (int id = 0; id < NUM_JOINTS; id++) {
        copleyDrives.push_back(new CopleyDrive(id + 1));
        if (id == LEFT_HIP || id == RIGHT_HIP) {
            joints.push_back(new AlexJoint(id, jointMinMap[id], jointMaxMap[id], copleyDrives[id], hipParam));
        } else if (id == LEFT_KNEE || id == RIGHT_KNEE) {
            joints.push_back(new AlexJoint(id, jointMinMap[id], jointMaxMap[id], copleyDrives[id], kneeParam));
        } else {  // is an ankle  ->  CHANGE DRIVE to Ankle drives NOT COPLEY
            joints.push_back(new AlexJoint(id, jointMinMap[id], jointMaxMap[id], copleyDrives[id], ankleParam));
        }
    }
    return true;
}

bool AlexRobot::initialiseNetwork() {
    DEBUG_OUT("AlexRobot::initialiseNetwork()");

    // bool status;
    // for (auto joint : joints) {
    //     status = joint->initNetwork();
    //     if (!status)
    //         return false;
    // }

    return true;
}
bool AlexRobot::initialiseInputs() {
    inputs.push_back(new Keyboard());
    return true;
}
void AlexRobot::freeMemory() {
    keyboard.~Keyboard();
    for (auto p : joints) {
        DEBUG_OUT("Delete Joint ID: " << p->getId())
        delete p;
    }
    for (auto p : copleyDrives) {
        DEBUG_OUT("Delete Drive Node: " << p->getNodeID())
        delete p;
    }
}
void AlexRobot::updateRobot() {
    Robot::updateRobot();
    keyboard.updateInput();
}
double AlexRobot::getCurrTrajProgress() {
    return currTrajProgress;
}
std::vector<double> AlexRobot::getJointStates() {
    std::vector<double> robotJointspace;
    int i = 0;
    for (auto joint : joints) {
        robotJointspace.push_back(joint->getQ());
        i++;
    }
    return robotJointspace;
}
void AlexRobot::bitFlip() {
    for (auto joint : joints) {
        joint->bitFlip();
    }
}
void AlexRobot::setPos(RobotMode mode) {
    std::vector<double> initialPoints;
    if (mode == RobotMode::SITDWN) {
        initialPoints = {180,
                         0,
                         180,
                         0,
                         0,
                         0};
    } else if (mode == RobotMode::STNDUP) {
        initialPoints = {90,
                         90,
                         90,
                         90,
                         0,
                         0};
    }
    int i = 0;
    for (auto p : joints) {
        DEBUG_OUT("SETTING JOINT:" << p->getId() << "TO DEG: " << initialPoints[i]);
        setMovementReturnCode_t setPosCode = ((ActuatedJoint *)p)->setPosition(initialPoints[i]);
        p->setQ(initialPoints[i]);
        i++;
    }
}
void AlexRobot::setPos(RobotMode mode, Foot standingLeg) {
    std::vector<double> initialPoints;
    if (mode == RobotMode::NORMALWALK && standingLeg == Foot::Left) {
        initialPoints = {140.421,
                         30.4901,
                         177.933,
                         5.74668,
                         85,
                         85};
        /* these initial positions shoul causeanerror*/
    } else if (mode == RobotMode::NORMALWALK && standingLeg == Foot::Right) {
        // initialPoints = {178.896,
        //                  5.31037,
        //                  169.214,
        //                  17.5468,
        //                  85,
        //                  85};
        initialPoints = {178.896,
                         5.31037,
                         140.421,
                         30.4901,
                         85,
                         85};
    }
    int i = 0;
    for (auto p : joints) {
        DEBUG_OUT("SETTING JOINT:" << p->getId() << "TO DEG: " << initialPoints[i]);
        setMovementReturnCode_t setPosCode = ((ActuatedJoint *)p)->setPosition(initialPoints[i]);
        p->setQ(initialPoints[i]);
        i++;
    }
    // Above needs a cycle of updateJoints to run for the values to register in q therfore hard setting q values in joints here.
}