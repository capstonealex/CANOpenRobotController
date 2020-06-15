
/**
 * \file Drive.h
 * \author Justin Fong
 * \brief  The <code>Drive</code> class is used to interface with a CANOpen motor drive. According to the CiA402 standard
 * 
 * This class enables low level functions to the system. It does limited error 
 * checking. 
 * \version 0.1
 * \date 2020-04-07
 * \version 0.1
 * \copyright Copyright (c) 2020
 * 
 */

#ifndef DRIVE_H_INCLUDED
#define DRIVE_H_INCLUDED

#include <CANopen.h>
#include <CO_command.h>
#include <string.h>

#include <map>
#include <sstream>
#include <vector>

/**
 * \brief Enum representing possible Drive control modes
 *
*/

enum ControlMode {
    UNCONFIGURED = 0,     /**< 0 */
    POSITION_CONTROL = 1, /**< 1 */
    VELOCITY_CONTROL = 2, /**< 2 */
    TORQUE_CONTROL = 3,   /**< 3 */
    ERROR = -1            /**< -1 */
};
/**
 * \brief Enum representing possible Drive states 
*/
enum DriveState {
    DISABLED = 0,           /**< 0 */
    READY_TO_SWITCH_ON = 1, /**< 1 */
    ENABLED = 2,            /**< 2 */
};

/**
 * @brief Enum representing commonly-used entries defined in the Object Dictionary for CiA402 Drives
*/
enum OD_Entry_t {
    STATUS_WORD = 0, /**< 0 */
    ACTUAL_POS = 1,  /**< 1 */
    ACTUAL_VEL = 2,  /**< 2 */
    ACTUAL_TOR = 3,  /**< 3 */
    TARGET_POS = 11, /**< 11 */
    TARGET_VEL = 12, /**< 12 */
    TARGET_TOR = 13  /**< 13 */
};

/**
 * \brief Map between commonly-used Object Dictionary (OD) entries and their respective memory addresses
 *
 *  Used to generate PDO Configurations
 *  NOTE: The addresses are written in hexadecimal
 *  
 * 
 */
static std::map<OD_Entry_t, int> OD_Addresses = {
    {STATUS_WORD, 0x6041},
    {ACTUAL_POS, 0x6064},
    {ACTUAL_VEL, 0x606C},
    {ACTUAL_TOR, 0x6077},
    {TARGET_POS, 0x607A},
    {TARGET_VEL, 0x60FF},
    {TARGET_TOR, 0x6071}};

/**
 * \brief Map between commonly-used Object Dictionary (OD) entries and their respective data lengths
 *  
 *  Used to generate PDO Configurations
 *  NOTE: The addresses are written in hexadecimal
 * 
 */
static std::map<OD_Entry_t, int> OD_Data_Size = {
    {STATUS_WORD, 0x0010},
    {ACTUAL_POS, 0x0020},
    {ACTUAL_VEL, 0x0020},
    {ACTUAL_TOR, 0x0010},
    {TARGET_POS, 0x0020},
    {TARGET_VEL, 0x0020},
    {TARGET_TOR, 0x0010}};

/**
 * \brief Struct for Drive motor controller motion profile
 *
 *  Stores desired velocity, acceleration and deceleration values for motor profile.
 */
struct motorProfile {
    int profileVelocity;
    int profileAcceleration;
    int profileDeceleration;
};
/**
 * The sdoReturnCode_t is used to determine whether the correct number of SDO messages
 * were recieved succesfully by nodes on the network.  
 */
enum sdoReturnCode_t {
    CORRECT_NUM_CONFIRMATION = 1,
    INCORRECT_NUM_CONFIRMATION = -1,
};

/**
 * @ingroup Robot
 * \brief Abstract class describing a Drive used to communicate with a CANbus device. Note that many functions are implemented according to the CiA 402 Standard (but can be overridden)
 * 
 */
class Drive {
   protected:
    /**
        * \brief The CAN Node ID used to address this drive on the CAN bus
        * 
        */
    int NodeID;

    /**
    * \brief Generates the list of SDO commands required to configure TPDOs on the drive
    * 
    * Transmit Process Data Objects (TPDOs) require prior setup to be used to send data from a drive's object dictionary,
    * this setup is performed using Service Data Object (SDO) commands
    *      
    * \param items A list of OD_Entry_t items which are to be configured with this TPDO
    * \param PDO_Num The number/index of this PDO
    * \param SyncRate The rate at which this PDO transmits (e.g. number of Sync Messages. 0xFF represents internal trigger event)
    * \return std::string 
    */
    std::vector<std::string> generateTPDOConfigSDO(std::vector<OD_Entry_t> items, int PDO_Num, int SyncRate);

    /**
    * \brief Generates the list of SDO commands required to configure RPDOs on the drives
    * 
    * Receive Process Data Objects (RPDOs) require prior setup to be used to receive data sent to a drive's object dictionary,
    * this setup is performed using Service Data Object (SDO) commands
    *
    * \param items A list of OD_Entry_t items which are to be configured with this RPDO
    * \param PDO_Num The number/index of this PDO
    * \param UpdateTiming 0-240 represents hold until next sync message, 0xFF represents immediate update
    * \return std::string 
    */
    std::vector<std::string> generateRPDOConfigSDO(std::vector<OD_Entry_t> items, int PDO_Num, int UpdateTiming);

    /**
       * 
       * \brief  Generates the list of SDO commands required to configure Position control in CANopen motor drive
       * 
       * \param positionProfile.profileVelocity Velocity value used by position mode motor trajectory generator.   
       * \param positionProfile.profileAccelration Acceleration value Position Mode motor trajectory generator will attempt to achieve.  
       * \param positionProfile.profileDeceleration Deceleration value Position Mode motor trajectory generator will use at the end of trapezoidal motion profile. 
       * 
       * NOTE: More details on params and profiles can be found in the CANopne CiA 402 series specifications:
       *           https://www.can-cia.org/can-knowledge/canopen/cia402/
       */

    /**
     * @brief Generates the list of SDO commands required to configure Position control in CANopen motor drive
     * @param positionProfile 
     * @return 
    */

    std::vector<std::string> generatePosControlConfigSDO(motorProfile positionProfile);

    /**
       * 
       * \brief  Generates the list of commands required to configure Velocity control in CANopen motor drive
       * 
       *     
       * \param Profile Velocity, value used by Velocity mode motor trajectory generator.
       *     
       * \param Profile Acceleration, value Velocity mode motor trajectory generator will attempt to achieve.
       *     
       * \param Profile Deceleration, value Velocity mode motor trajectory generator will use at end of trapezoidal profile.
       *     
       * NOTE: More details on params and profiles can be found in the CANopne CiA 402 series specifications:
       *           https://www.can-cia.org/can-knowledge/canopen/cia402/
       */

    /**
     * @brief 
     * @param velocityProfile 
     * @return 
    */
    std::vector<std::string> generateVelControlConfigSDO(motorProfile velocityProfile);

    /**
       *
       * \brief  Generates the list of commands required to configure Torque control in CANopen motor drive
       *
       * NOTE: More details on params and profiles can be found in the CANopne CiA 402 series specifications:
       *           https://www.can-cia.org/can-knowledge/canopen/cia402/
       */
    std::vector<std::string> generateTorqueControlConfigSDO();

    /**                                                                                                             
        * \brief messages Properly formatted SDO Messages
        * 
        * \return int number of messages successfully processed(return OK) 
              */

    sdoReturnCode_t sendSDOMessages(std::vector<std::string> messages);

   private:
    /**
        * \brief Current status word of the drive
        * 
        */
    int statusWord;

    /**
     * \brief Current error state of the drive 
     * 
     */
    int error;

    /**
        * \brief State of the drive
        * 
        */
    DriveState driveState = DISABLED;

    /**
        * \brief The mode in which the drive is currently configured
        * 
        */
    ControlMode controlMode = UNCONFIGURED;

   public:
    /**
        * \brief Construct a new Drive object
        * 
        */
    Drive();

    /**
           * \brief Construct a new Drive object
           * 
           * \param NodeID the CANopen Node ID of this drive
           */
    Drive(int NodeID);

    /**
           * \brief Destroy the Drive object
           * 
           */
    virtual ~Drive(){};

    /**
           * \brief Initialises the drive (SDO start message)
           * 
           * \return True if successful, False if not
           */
    virtual bool Init() = 0;

    /**
    * \brief Initialises a standard set of PDOs for the use of the drive. These are:
    * 
    *   TPDO | Mapping Address | Usage
    *   ---- | ---- | ----
    *   TPDO1: COB-ID 180+{NODE-ID} | Status Word (0x6041) | Send on Internal %Event Trigger
    *   TPDO2: COB-ID 280+{NODE-ID} | Actual Position (0x6064), Actual Velocity (0x606C) | Sent every SYNC Message
    *   TPDO3: COB-ID 380+{NODE-ID} | Actual Torque (0x607C) | Sent every SYNC MEssage
    *
    *   RPDO | Mapping Address | Usage
    *   ---- | ---- | ----
    *   RPDO3: COB-ID 300+{NODE-ID} | Target Position (0x607A) | Applied immediately when received
    *   RPDO4: COB-ID 400+{NODE-ID} | Target Velocity (0x60FF) | Applied immediately when received    
    *   RPDO5: COB-ID 500+{NODE-ID} | Target Torque (0x6071) | Applied immediately when received    
    *
    * \return true 
    * \return false 
    */
    virtual bool initPDOs();

    /**
           * Sets the drive to Position control with set parameters (through SDO messages)
           * 
           * Note: Should be overloaded to allow parameters to be set
           * 
           * \param motorProfile The position control motor profile to be used
           * 
           * \return true if successful
           * \return false if not
           */
    virtual bool initPosControl(motorProfile posControlMotorProfile) = 0;

    /**
           * Sets the drive to Velocity control with default parameters (through SDO messages)
           * 
           * Note: Should be overloaded to allow parameters to be set
           * 
           * \return true if successful
           * \return false if not
           */
    virtual bool initVelControl(motorProfile velControlMotorProfile) = 0;

    /**
           * Sets the drive to Torque control with default parameters (through SDO messages)
           * 
           * Note: Should be overloaded to allow parameters to be set
           * 
           * \return true if successful
           * \return false if not
           */
    virtual bool initTorqueControl() = 0;

    /**
           * Updates the internal representation of the state of the drive 
           * 
           * \return The current value of the status word (0x6041)
           */
    virtual int updateDriveStatus();

    /**
           * Writes the desired position to the Target Position of the motor drive (0x607A)
           * 
           * \return true if successful
           * \return false if not
           */
    virtual bool setPos(int position);

    /**
           * Writes the desired velocity to the Target Velocity of the motor drive (0x60FF)
           * 
           * \return true if successful
           * \return false if not
           */
    virtual bool setVel(int velocity);

    /**
           * Writes the desired torque to the Target Torque of the motor drive (0x6071)
           * 
           * \return true if successful
           * \return false if not
           */
    virtual bool setTorque(int torque);

    /**
           * Returns the current position from the motor drive (0x6064)
           * 
           * \return Position from the motor drive
           */
    virtual int getPos();

    /**
           * Returns the current velocity from the motor drive (0x606C)
           * Returns 0 if NODEID is 5 or 6: ankles. They have no OD entry.
           * \return Velocity from the motor drive
           */
    virtual int getVel();

    /**
           * Returns the current torque from the motor drive (0x6077)
           * Returns 0 if NODEID is 5 or 6: ankles. They have no OD entry.
           * \return Torque from the motor drive
           */
    virtual int getTorque();

    // Drive State Modifiers
    /**
           * \brief Changes the state of the drive to "ready to switch on". 
           * 
           * This is equivalent to setting bits 2 and 3 of Control Word (0x6064) to 1.
           * See also the CANopen Programmer's Manual (from Copley Controls)
           * 
           * \return true if operation successful
           * \return false if operation unsuccessful
           */
    virtual bool readyToSwitchOn();

    /**
           * \brief Sets the state of the drive to "enabled"
           * 
           * This is equivalent to setting bits 0, 1, 2, 3 of the control word (0x06064) to 1
           * See also the CANopen Programmer's Manual (from Copley Controls)
           * 
           * \return true if operation successful
           * \return false if operation unsuccessful
           */
    virtual bool enable();

    /**
           * \brief sets the state of the drive to "disabled"
           * 
           * This is equivalent to setting the control word (0x06064) to 0
           * See also the CANopen Programmer's Manual (from Copley Controls)
           * 
           * \return true if operation successful
           * \return false if operation unsuccessful
           */
    virtual bool disable();

    /**
        * \brief Flips Bit 4 of Control Word (0x6041) - A new set point is only confirmed if the transition is from 0 to 1
        * 
        * \return true The control word was previously 0 (i.e. successful set point confirm)
        * \return false The control word was previously 1 (i.e. unsuccessful set point confirm)
        */
    virtual bool posControlConfirmSP();

    /**
        * \brief Get the current state of the drive
        * 
        * \return DriveState 
        */
    virtual DriveState getDriveState();

    // CANOpen
    /**
           * \brief Get returns the CanNode ID
           * 
           * \return int the Node ID 
           */
    int getNodeID();
};

#endif