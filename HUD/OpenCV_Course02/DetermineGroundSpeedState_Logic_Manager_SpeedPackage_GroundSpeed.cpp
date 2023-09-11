//
//  DetermineGroundSpeedState_Logic_Manager_SpeedPackage_GroundSpeed.cpp
//  
//
//  Created by Bernardo Lorenzini on 11/09/23.
//

#include "DetermineGroundSpeedState_Logic_Manager_SpeedPackage_GroundSpeed.hpp"


#include "consts.h"
#include "sensors.h"

//* DetermineGroundSpeedState determines the state of Ground Speed
//* 1: normal state (GS III)
//* 2: not valid ( XXX)
GroundSpeedStateEnum_DisplayTypes DetermineGroundSpeedState_LogicManager_SpeedPackage_GroundSpeed(
//  * ground speed validity (bool
kcg_bool bGroundSpeedValid,
//  * Ground Speed (knots )
                                                                                                  
kcg_real rGroundSpeed)


{
//  ground speed state
  
  if ((999.100 >= rGroundSpeed) & (rGroundSpeed >= 0.000) & bGroundSpeedValid) {
    eGroundSpeedState = GROUND_SPEED_NORMAL_STATE_DisplayTypes;
  }
  else {
    eGroundSpeedState = GROUND_SPEED_NOT_VALID_STATE_DisplayTypes;
  }
  return eGroundSpeedState;
}
