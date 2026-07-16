#ifndef AEGISHOME_SYSTEMSTATEMANAGER_H
#define AEGISHOME_SYSTEMSTATEMANAGER_H

#include <Arduino.h>
#include "Enums.h"

// Sole owner of the FSM. No other module may mutate state directly —
// all transitions go through requestTransition(). SystemState itself
// now lives in Enums.h so this header doesn't need to be included just
// to reach the enum (see Enums.h header comment).
class SystemStateManager {
public:
  void begin();
  SystemState current() const;

  // Returns false if the transition is invalid for the current state.
  bool requestTransition(SystemState next);
  
private:
    SystemState state_;
};

#endif // AEGISHOME_SYSTEMSTATEMANAGER_H
