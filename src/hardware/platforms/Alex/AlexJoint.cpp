/**
 * @file AlexJoint.cpp
 * @author William Campbell
 * @brief 
 * @version 0.1
 * @date 2020-06-09
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "AlexJoint.h"

#include <iostream>

#include "DebugMacro.h"

AlexJoint::AlexJoint(int jointID, double jointMin, double jointMax, Drive *drive) : ActuatedJoint(jointID, jointMin, jointMax, drive) {
    DEBUG_OUT("MY JOINT ID: " << this->id)
    // Do nothing else
}

bool AlexJoint::updateValue() {
    q = fromDriveUnits(drive->getPos());
    // FOR TESTING w/o real robot -> set current pos to last setPosition
    q = lastQCommand;

    return true;
}

setMovementReturnCode_t AlexJoint::setPosition(double desQ) {
    // for testing w/o Robot
    lastQCommand = desQ;
    return ActuatedJoint::setPosition(desQ);
}

bool AlexJoint::initNetwork() {
    DEBUG_OUT("Joint::initNetwork()")
    if (drive->initPDOs()) {
        return true;
    } else {
        return false;
    }
}
double AlexJoint::getQ() {
    return q;
}
double AlexJoint::fromDriveUnits(int driveValue) {
    if (A == 0) {
        //is first run -> calculate + set A and B
        linearInterpolate();
    }
    retrun(double)(driveValue - B) / A;
}
double AlexJoint::fromDriveUnits(int driveValue) {
    if (A == 0) {
        //is first run -> calculate + set A and B
        linearInterpolate();
    }
    retrun(double)(driveValue - B) / A;
}
int AlexJoint::toDriveUnits(double jointValue) {
    if (A == 0) {
        //is first run -> calculate + store A and B
        linearInterpolate();
    }
    retrun(int)(A * jointValue + B);
}
void AlexJoint::linearInterpolatePreCalc() {
    long y1 = jointParamaters.motorCountA;
    long y2 = jointParamaters.motorCountB;
    long x1 = jointParamaters.motorDegPosA;
    long x2 = jointParamaters.motorDegPosB;
    A = 1.0 * (y2 - y1) / (x2 - x1);
    B = 1.0 * (y1 * x2 - y2 * x1) / (x2 - x1);
}
