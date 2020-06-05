#include "ExoRobot.h"

#include "DebugMacro.h"

ExoRobot::ExoRobot(AlexTrajectoryGenerator *tj) {
    trajectoryGenerator = tj;
    std::cout
        << "EXO ROBOT CONSTRUCTOR"
        << "address: " << &trajectoryGenerator << '\n';
}
ExoRobot::ExoRobot(){
    DEBUG_OUT("EXO ROBOT CONSTRUCTOR")}

ExoRobot::~ExoRobot() {
    DEBUG_OUT("Delete ExoRobot object begins")
    freeMemory();
    joints.clear();
    copleyDrives.clear();
    DEBUG_OUT("ExoRobot deleted")
}

bool ExoRobot::initPositionControl() {
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
    return returnValue;
}

bool ExoRobot::initTorqueControl() {
    DEBUG_OUT("Initialising Torque Control on all joints ")
    bool returnValue = true;
    for (auto p : joints) {
        if (((ActuatedJoint *)p)->setMode(TORQUE_CONTROL) != TORQUE_CONTROL) {
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
    return returnValue;
}

void ExoRobot::startNewTraj() {
    DEBUG_OUT("Start New Traj");
    // FROM ORIGINAL: Set the bit flip state to zero
    /*/todo: add once rest is addressed*/
    /*generate new Trajectory in trajectory object*/
    /*/todo: change to happen outside robot object after functional*/
    jointspace_state startNewTrajJointspace;
    double robotJointspace[NUM_JOINTS];
    //store current joint angles in joint space objects
    /*\todo: turn into a robot function for getting current STATE of robot*/
    for (auto joint : joints) {
        int j = joint->getId();
        DEBUG_OUT("GOT ID num:" << j)

        robotJointspace[j - 1] = deg2rad(joint->getQ());
    }
    /*/todo: bellow should be a single function taking in the STATE of the robot*/
    startNewTrajJointspace = {.q = {robotJointspace[0],
                                    robotJointspace[1],
                                    robotJointspace[2],
                                    robotJointspace[3],
                                    deg2rad(85),   //robotJointspace[4],
                                    deg2rad(85)},  //robotJointspace[5]},
                              .time = 0};
    DEBUG_OUT("GOT Jointspace params")
    std::cout
        << "address: " << &trajectoryGenerator << '\n';
    dynamic_cast<AlexTrajectoryGenerator *>(trajectoryGenerator)->generateAndSaveSpline(startNewTrajJointspace);
    DEBUG_OUT("GENERATED SPLINE")
    // Index Resetting
    // this->currTrajProgress = 0;
    // initialize timer for moveThrough Traj first step
    DEBUG_OUT("set var")
    clock_gettime(CLOCK_MONOTONIC, &prevTime);
    DEBUG_OUT("FINSIHED")
}

bool ExoRobot::moveThroughTraj() {
    bool returnValue = true;

    timespec currTime;
    clock_gettime(CLOCK_MONOTONIC, &currTime);

    double elapsedSec = currTime.tv_sec - prevTime.tv_sec + (currTime.tv_nsec - prevTime.tv_nsec) / 1e9;
    prevTime = currTime;

    // This should check to make sure that the "GO" button is pressed.
    if (true) {
        currTrajProgress += elapsedSec;
        DEBUG_OUT("Elapsed Time: " << currTrajProgress)
        // calculate the current desired position trajectory for current time.
        std::cout
            << "address: " << &trajectoryGenerator << '\n';
        std::vector<double> setPoints = trajectoryGenerator->getSetPoint(currTrajProgress);
        int i = 0;
        for (auto p : joints) {
            setMovementReturnCode_t setPosCode = ((ActuatedJoint *)p)->setPosition(setPoints[i]);
            if (setPosCode == INCORRECT_MODE) {
                std::cout << "Joint ID " << p->getId() << ": is not in Position Control " << std::endl;
                returnValue = false;
            } else if (setPosCode != SUCCESS) {
                // Something bad happened
                std::cout << "Joint " << p->getId() << ": Unknown Error " << std::endl;
                returnValue = false;
            }
            i++;
        }
    } else {
        DEBUG_OUT("Not moving")
    }

    return returnValue;
}
bool ExoRobot::initialiseTrajGen(AlexTrajectoryGenerator *tj) {
    std::cout
        << "Object passed in address"
        << "address: " << &tj << '\n';
    trajectoryGenerator = tj;
    std::cout
        << "Object set "
        << "address: " << &trajectoryGenerator << '\n';

    return true;
}

bool ExoRobot::initialiseJoints() {
    for (int id = 0; id < NUM_JOINTS; id++) {
        copleyDrives.push_back(new CopleyDrive(id + 1));
        joints.push_back(new DummyActJoint(id, jointMinMap[id], jointMaxMap[id], copleyDrives[id]));
    }
    return true;
}

bool ExoRobot::initialiseNetwork() {
    DEBUG_OUT("ExoRobot::initialiseNetwork()");

    bool status;
    for (auto joint : joints) {
        status = joint->initNetwork();
        if (!status)
            return false;
    }

    return true;
}
bool ExoRobot::initialiseInputs() {
    inputs.push_back(new Keyboard());
    return true;
}
void ExoRobot::freeMemory() {
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
void ExoRobot::updateRobot() {
    Robot::updateRobot();
    keyboard.updateInput();
}