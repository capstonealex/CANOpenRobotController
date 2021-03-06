
/**
 * 
 * \file AlexRobot.h
 * \author William Campbell 
 * \version 0.1
 * \date 2020-06-10
 * \copyright Copyright (c) 2019
 * 
 * \breif  The<code> AlexRobot</ code> class represents an ExoSkeleton Robot in terms of its
 * representation of the Alex exoskeleton hardware whose memory is managed in this class.
 *
 *
 * Version 0.1
 * Date: 07/04/2020
 */

#ifndef AlexRobot_H_INCLUDED
#define AlexRobot_H_INCLUDED

#include <time.h>

#include <map>

#include "AlexJoint.h"
#include "AlexTrajectoryGenerator.h"
#include "CopleyDrive.h"
#include "Keyboard.h"
#include "Buttons.h"
#include "Robot.h"
#include "RobotParams.h"
#include "SchneiderDrive.h"
#include "pocketBeagle.h"

/**
     * \todo Load in paramaters and dictionary entries from JSON file.
     * 
     */
/**
 * \brief Example implementation of the Robot class, representing an X2 Exoskeleton, using DummyActuatedJoint and AlexTrajectoryGenerator.
 * 
 */
class AlexRobot : public Robot {
   private:
    /** Parameters associated with Trajectory Progression */
    double currTrajProgress = 0;
    double currTrajTime; /*currently loaded trajectories total time of execution, must be set before begining a trajectory*/
    timespec prevTime;
    /*Flag for loading in new trajectories only after a green button release*/
    bool resetTrajectory;
    /**
     * \brief motor drive position control profile paramaters, user defined.
     * 
     */

    motorProfile posControlMotorProfile{4000000, 190000, 190000};

   public:
    AlexRobot();
    /**
      * \brief Default <code>AlexRobot</code> constructor.
      * Initialize memory for the Exoskelton <code>Joint</code> + sensors. 
      * Load in exoskeleton paramaters to  <code>TrajectoryGenerator.</code>.
      */
    AlexRobot(AlexTrajectoryGenerator *tj);
    ~AlexRobot();
    AlexTrajectoryGenerator *trajectoryGenerator;
    Keyboard keyboard;
    pocketBeagle pb;
    Buttons buttons;

    // Base class drive pointer: can be any type of derived driver class.
    vector<Drive *> Drives;

    // /**
    //  * \brief Timer Variables for moving through trajectories
    //  *
    //  */
    struct timeval tv, tv_diff, moving_tv, tv_changed, stationary_tv, start_traj, last_tv;

    /**
       * \brief Initialises all joints to position control mode. 
       * 
       * \return true If all joints are successfully configured
       * \return false  If some or all joints fail the configuration
       */
    bool initPositionControl();

    /**
       * \brief Initialises all joints to torque control mode.
       *
       * \return true If all joints are successfully configured
       * \return false  If some or all joints fail the configuration
   */
    bool initTorqueControl();

    /** 
      * /brief For each joint, move through(send appropriate commands to joints) the currently 
      * generated trajectory of the TrajectoryGenerator object - this assumes the trajectory and robot is in position control. 
      *
      * /return true if successful
      * /return false if not successful (e.g. any joint not in position control.)
      */
    bool moveThroughTraj();

    /** 
      *  \brief Begin a new trajectory with the currently loaded trajectory paramaters. 
      * Using the <code>AlexRobot</code> current configuration (read in from joint objects) 
      * and the trajecotry generator object, generate and save a spline to move from current 
      * to desired position.
      * 
      */
    void startNewTraj();

    /**
       * \brief Implementation of Pure Virtual function from <code>Robot</code> Base class.
       * Create designed <code>Joint</code> and <code>Driver</code> objects and load into 
       * Robot joint vector.
       */
    bool initialiseJoints();

    /**
       * \brief Implementation of Pure Virtual function from <code>Robot</code> Base class.
       * Initialize each <code>Drive</code> Objects underlying CANOpen Networking.

      */
    bool initialiseNetwork();
    /**
       * \brief Implementation of Pure Virtual function from <code>Robot</code> Base class.
       * Initialize each <code>Input</code> Object.

      */
    bool initialiseInputs();

    /**
       * \brief Free robot objects vector pointer memory.
       */
    void
    freeMemory();
    /**
       * \brief update current state of the robot, including input and output devices. 
       * Overloaded Method from the Robot Class. 
       * Example. for a keyboard input this would poll the keyboard for any button presses at this moment in time.
       */
    void updateRobot();
    /**
       * \brief getter method for currentTrajectory progress variable.
       *
       * \return double currentTrajProgress
       */
    double getCurrTrajProgress();

    /*testing*/
    void
    bitFlip();
    /**
    * \brief Get each joints state (angle) and return in a vector
    * 
    * \return std::vector<double> 
    */
    std::vector<double> getJointStates();
    /**
 * \brief Set the Next Motion object
 * 
 * @param nextMotion 
 */
    void setNextMotion(RobotMode nextMotion);
    /**
 * \brief Get the Next Motion OD entry
 * 
 * \return RobotMode 
 */
    RobotMode getNextMotion();
    /**
 * \brief Set the Current Motion object from the od.nextMotion entry
 * 
 * \return current RobotMode
 * 
 */
    void setCurrentMotion(RobotMode nextMotion);
    /**
 * \brief Get the Current Motion OD entry
 * 
 * \return RobotMode 
 */
    RobotMode getCurrentMotion();
    /**
     * \brief Get the Go OD entry
     * 
     * \return int 
     */
    bool getGo();
    /**
 * \brief Set the Current State object
 * 
 * @param state 
 */
    void setCurrentState(AlexState state);
    /**
 * \brief set Entry flag value
 * 
 * @param value 
 */
    void setResetFlag(bool value);
    /**
 * \brief get Entry Flag value
 * 
 * \return true 
 * \return false 
 */
    bool getResetFlag();
    /**
    * \todo Move jointMinMap and jointMaxMap to RobotParams.h
    * 
    */

   /**
    * \brief disable all joints of the robot, returns true if successful
    * 
    */
   bool disableJoints();
    /**
       * \brief Joint Limit Map between Joint value and max Degrees possible
       * \param int Joint value
       * \return int maxDeg 
       */
    std::map<int, double> jointMinMap = {{LEFT_HIP, 70},
                                         {LEFT_KNEE, 0},
                                         {RIGHT_HIP, 70},
                                         {RIGHT_KNEE, 0},
                                         {LEFT_ANKLE, 75},
                                         {RIGHT_ANKLE, 75}};
    /**
       * \brief Joint Limit Map between Joint value and max Degrees possible
       * \param int Joint value
       * \return int maxDeg 
       */
    std::map<int, double> jointMaxMap = {{LEFT_HIP, 210},
                                         {LEFT_KNEE, 120},
                                         {RIGHT_HIP, 210},
                                         {RIGHT_KNEE, 120},
                                         {LEFT_ANKLE, 105},
                                         {RIGHT_ANKLE, 105}};
};
#endif /*AlexRobot_H*/