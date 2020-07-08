/**
 * \brief  The <code>Drive</code> class is used to interface with a CANOpen motor drive. According to the CiA402 standard
 * 
 * This class enables low level functions to the system. It does limited error 
 * checking. 
 * 
 * \file Drive.h
 * \author Justin Fong
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
 * \brief Supported drive control modes
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
 * \brief Supported drive states
 * 
 */
enum DriveState {
    DISABLED = 0,           /**< 0 */
    READY_TO_SWITCH_ON = 1, /**< 1 */
    ENABLED = 2,            /**< 2 */
};

/**
 * \brief Commonly-used Object Dictionary (OD) entries for CiA402 Drives
 * 
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
 *  Stores the desired velocity, acceleration and deceleration values of a motor profile.
 * 
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
    INCORRECT_NUM_CONFIRMATION = 0,
};

/**
 * @ingroup Robot
 * \brief Abstract class describing a Drive used to communicate with a CANbus device. Note that many functions are implemented according to the CiA 402 Standard (but can be overridden)
 * 
 */
class Drive {
   protected:
    /**
     * \brief The CANopen Node ID used to address this drive on the CAN bus
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
     * \return std::vector<std::string> 
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
     * \return std::vector<std::string> 
     */

    std::vector<std::string> generateRPDOConfigSDO(std::vector<OD_Entry_t> items, int PDO_Num, int UpdateTiming);

    /**
     * \brief Generates the list of SDO commands required to configure position control in CANopen motor drive
     * 
     * NOTE: More details on params and profiles can be found in the CANopne CiA 402 series specifications:
     *           https://www.can-cia.org/can-knowledge/canopen/cia402/
     * 
     * \param positionProfile describing motorProfile parameters for position control
     * \return std::vector<std::string> representing a generated list of SDO configuration commands for position control
     * \sa motorProfile
     */

    std::vector<std::string> generatePosControlConfigSDO(motorProfile positionProfile);

    /**
     * \brief Generates the list of SDO commands required to configure velocity control in CANopen motor drive
     * 
     * NOTE: More details on params and profiles can be found in the CANopne CiA 402 series specifications:
     *           https://www.can-cia.org/can-knowledge/canopen/cia402/
     * 
     * \param velocityProfile describing motorProfile parameters for velocity control
     * \return std::vector<std::string> representing a generated list of SDO configuration commands for velocity control
     * \sa motorProfile
     */
    std::vector<std::string> generateVelControlConfigSDO(motorProfile velocityProfile);

    /**
     * \brief Generates the list of SDO commands required to configure torque control in CANopen motor drive 
     * 
     * NOTE: More details on params and profiles can be found in the CANopne CiA 402 series specifications:
     *           https://www.can-cia.org/can-knowledge/canopen/cia402/ 
     * 
     * \return std::vector<std::string> representing a generated list of SDO configuration commands for torque control
     */
    std::vector<std::string> generateTorqueControlConfigSDO();

    /**
     * \brief Sends correctly formatted SDO messages
     * 
     * \param messages 
     * \return sdoReturnCode_t representing the number of successfully processed messages (returned OK)
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
     * \brief Current state of the drive - Initialised in DISABLED state
     * 
     */
    DriveState driveState = DISABLED;

    /**
     * \brief Current control mode of the drive - Initialised in UNCONFIGURED control mode
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
     * \param NodeID The CANopen Node ID of this drive on the CAN bus
     */
    Drive(int NodeID);

    /**
     * \brief Destroy the Drive object
     * 
     */
    virtual ~Drive(){};

    /**
     * \brief Initialises the drive, providing the SDO start message
     * 
     * \return true if successful
     * \return false if unsuccessful
     */
    virtual bool Init() = 0;

    /**
    * \brief Initialises a standard set of PDOs for the use of the drive as shown in the tables below:
    * 
    *   <b>TPDO</b>
    * 
    *   TPDO | Mapping Address | Usage
    *   ---- | ---- | ----
    *   TPDO1: COB-ID 180+{NODE-ID} | Status Word (0x6041) | Send on Internal %Event Trigger
    *   TPDO2: COB-ID 280+{NODE-ID} | Actual Position (0x6064), Actual Velocity (0x606C) | Sent every SYNC Message
    *   TPDO3: COB-ID 380+{NODE-ID} | Actual Torque (0x607C) | Sent every SYNC MEssage
    * 
    *   <b>RPDO</b>
    * 
    *   RPDO | Mapping Address | Usage
    *   ---- | ---- | ----
    *   RPDO3: COB-ID 300+{NODE-ID} | Target Position (0x607A) | Applied immediately when received
    *   RPDO4: COB-ID 400+{NODE-ID} | Target Velocity (0x60FF) | Applied immediately when received    
    *   RPDO5: COB-ID 500+{NODE-ID} | Target Torque (0x6071) | Applied immediately when received       
    * 
    * 
    * \return true if successful
    * \return false if unsuccessful
    */

    virtual bool initPDOs();

    /**
     * \brief Sets the drive to position control with the provided %motorProfile parameters using SDO messages
     * 
     * Note: Should be overloaded to allow parameters to be set
     * 
     * \param posControlMotorProfile representing the %motorProfile parameters for position control
     * 
     * \return true if successful
     * \return false if unsuccessful
     * \sa motorProfile
     */
    virtual bool initPosControl(motorProfile posControlMotorProfile) = 0;

    /**
     * \brief Sets the drive to velocity control with the provided %motorProfile parameters using SDO messages
     * 
     * Note: Should be overloaded to allow parameters to be set
     * 
     * \param velControlMotorProfile representing the %motorProfile parameters for velocity control
     * \return true if successful
     * \return false if unsuccessful
     * \sa motorProfile
     */
    virtual bool initVelControl(motorProfile velControlMotorProfile) = 0;

    /**
     * \brief Sets the drive to torque control with the provided %motorProfile parameters using SDO messages
     * 
     * Note: Should be overloaded to allow parameters to be set
     * 
     * \return true if successful
     * \return false if unsuccessful
     */
    virtual bool initTorqueControl() = 0;

    /**
     * \brief Updates the internal representation of the state of the drive 
     * 
     * \return int representing the current value of the drive status word (0x6041)
     */
    virtual int updateDriveStatus();

    /**
     * \brief Writes the desired position to the Target Position entry of the motor drive (0x607A)
     * 
     * \param position the desired drive position
     * \return true if successful
     * \return false if unsuccessful
     */
    virtual bool setPos(int position);

    /**
     * \brief Writes the desired velocity to the Target Velocity entry of the motor drive (0x60FF)
     * 
     * \param velocity the desired drive velocity
     * \return true if successful
     * \return false if unsuccessful
     */
    virtual bool setVel(int velocity);

    /**
     * \brief Writes the desired torque to the Target Torque entry of the motor drive (0x6071)
     * 
     * \param torque the desired drive torque
     * \return true if successful
     * \return false if unsuccessful
     */
    virtual bool setTorque(int torque);

    /**
     * \brief Gets the current position from the motor drive (0x6064)
     * 
     * \return int representing the current position of the drive 
     */
    virtual int getPos();

    /**
     * \brief Gets the current velocity from the motor drive (0x606C)
     * 
     * \return int representing the current velocity of the drive. 
     * Returns 0 if Node ID is 5 or 6 as these have no OD entry.
     */
    virtual int getVel();

    /**
     * \brief Gets the current torque from the motor drive (0x6077)
     * 
     * \return int representing the current torque of the drive
     * Returns 0 if Node ID is 5 or 6 as these have no OD entry.
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

    //CANopen
    /**
     * \brief Gets the CANopen Node ID of the drive
     * 
     * \return int representing the CANopen Node ID
     */
    int getNodeID();
    /**
        * \brief changes whether the set point is immediate changed, or the drive waits for the last set point to complete.
        *     Drive must be in the ENABLED state. 
        * 
        *  \return true if successful
        *  \return false if unsuccessful (drive not in the ENABLED State)
        */
    virtual bool changeSetPointImmediately(bool immediate);
};

#endif