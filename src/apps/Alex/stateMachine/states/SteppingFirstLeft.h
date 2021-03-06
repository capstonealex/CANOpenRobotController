/**
 * /file SteppingFirstLeft.h
 * /author William Campbell
 * /brief Concrete implementation of ExoTestState
 * /version 0.1
 * /date 2020-06-23
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef SteppingFirstLeft_H_INCLUDED
#define SteppingFirstLeft_H_INCLUDED

#include "ExoTestState.h"

/**
 * \brief Initialisation State for the ExoTestMachine (implementing ExoTestState)
 * 
 * State holds until event triggers its exit, and runs initPositionControl on exit 
 * Control of transition is independent of this class and is defined in ExoTestMachine.
 * 
 */
class SteppingFirstLeft : public ExoTestState {
   public:
    void entry(void);
    void during(void);
    void exit(void);
    SteppingFirstLeft(StateMachine *m, AlexRobot *exo, AlexTrajectoryGenerator *tg, const char *name = NULL) : ExoTestState(m, exo, tg, name){};
};

#endif