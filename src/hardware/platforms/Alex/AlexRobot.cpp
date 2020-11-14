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
    Drives.clear();
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

    for (auto p : joints) {
        ((ActuatedJoint *)p)->enable();
    }
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
    // Index Resetting
    currTrajProgress = 0;
    clock_gettime(CLOCK_MONOTONIC, &prevTime);
}

bool AlexRobot::moveThroughTraj() {
    bool returnValue = true;
    timespec currTime;
    clock_gettime(CLOCK_MONOTONIC, &currTime);

    double elapsedSec = currTime.tv_sec - prevTime.tv_sec + (currTime.tv_nsec - prevTime.tv_nsec) / 1e9;
    double trajTimeUS = trajectoryGenerator->getStepDuration();
    prevTime = currTime;
    // This should check to make sure that the "GO" button is pressed.
    if (getGo()) {
        currTrajProgress += elapsedSec;
        double fracTrajProgress = currTrajProgress / trajTimeUS;
        std::vector<double> setPoints = trajectoryGenerator->getSetPoint(fracTrajProgress);
        int i = 0;
        //std::cout << currTrajProgress << " , ";
        for (auto p : joints) {
            //std::cout << rad2deg(setPoints[i]) << ",";
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
        //std::cout << std::endl;
    } else {
        //DEBUG_OUT("PRESS Go to go!")
    }

    return returnValue;
}

bool AlexRobot::initialiseJoints() {
    JointKnownPos hipParam{HIP_MOTOR_POS1, HIP_MOTOR_POS2, HIP_MOTOR_DEG1, HIP_MOTOR_DEG2};
    JointKnownPos kneeParam{KNEE_MOTOR_POS1, KNEE_MOTOR_POS2, KNEE_MOTOR_DEG1, KNEE_MOTOR_DEG2};
    JointKnownPos ankleParam{ANKLE_MOTOR_POS1, ANKLE_MOTOR_POS2, ANKLE_MOTOR_DEG1, ANKLE_MOTOR_DEG2};

    for (int id = 0; id < NUM_JOINTS; id++) {
        if (id == LEFT_HIP || id == RIGHT_HIP) {
            Drives.push_back(new CopleyDrive(id + 1));
            joints.push_back(new AlexJoint(id, jointMinMap[id], jointMaxMap[id], Drives[id], hipParam));
        } else if (id == LEFT_KNEE || id == RIGHT_KNEE) {
            Drives.push_back(new CopleyDrive(id + 1));
            joints.push_back(new AlexJoint(id, jointMinMap[id], jointMaxMap[id], Drives[id], kneeParam));
        } else {  // is an ankle  ->  CHANGE DRIVE to Schneider drives NOT COPLEY
            Drives.push_back(new SchneiderDrive(id + 1));
            joints.push_back(new AlexJoint(id, jointMinMap[id], jointMaxMap[id], Drives[id], ankleParam));
        }
    }
    return true;
}

bool AlexRobot::initialiseNetwork() {
    DEBUG_OUT("AlexRobot::initialiseNetwork()");
#ifndef VIRTUAL
    bool status;
    for (auto joint : joints) {
        status = joint->initNetwork();
        if (!status)
            return false;
    }
#endif
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
    for (auto p : Drives) {
        DEBUG_OUT("Delete Drive Node: " << p->getNodeID())
        delete p;
    }
}
void AlexRobot::updateRobot() {
    Robot::updateRobot();
    keyboard.updateInput();
    buttons.updateInput();
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
void AlexRobot::setCurrentMotion(RobotMode mode) {
    *(&CO_OD_RAM.currentMovement) = static_cast<int>(mode);
}

RobotMode AlexRobot::getCurrentMotion() {
    return static_cast<RobotMode>(*(&CO_OD_RAM.currentMovement));
}
void AlexRobot::setNextMotion(RobotMode mode) {
    *(&CO_OD_RAM.nextMovement) = static_cast<int>(mode);
}
RobotMode AlexRobot::getNextMotion() {
    return static_cast<RobotMode>(*(&CO_OD_RAM.nextMovement));
}
void AlexRobot::setCurrentState(AlexState state) {
    *(&CO_OD_RAM.currentState) = static_cast<int>(state);
    //:" << *(&CO_OD_RAM.currentState));
}
bool AlexRobot::getGo() {
    if (*(&CO_OD_RAM.goButton) == 1) {
        return true;
    }
    return false;
}

void AlexRobot::setResetFlag(bool value) {
    resetTrajectory = value;
}

bool AlexRobot::getResetFlag() {
    return resetTrajectory;
    ;
}

bool AlexRobot::disableJoints() {
    bool tmp = true;
    for (auto p : joints) {
        if (((ActuatedJoint *)p)->disable() == false) {
            std::cout << "Drive failed to be disabled!" << std::endl;
            tmp = false;
        }
    }

    return tmp;
}

#ifdef VIRTUAL
void AlexRobot::setVirtualPosition(double angle) {
    for (auto joint : joints) {
        ((AlexJoint *)joint)->setPosition(angle);
    }
}
#endif

double AlexRobot::toJointDriveAngle(int driveValue) {
    AlexJoint *joint = (AlexJoint *)joints[0];
    return (joint->fromDriveUnits(driveValue));
}

double AlexRobot::toJointDriveAngle1(int driveValue) {
    AlexJoint *joint = (AlexJoint *)joints[1];
    return (joint->fromDriveUnits(driveValue));
}