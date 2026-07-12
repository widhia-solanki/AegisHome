#ifndef AEGISHOME_SYSTEMSTATEMANAGER_H
#define AEGISHOME_SYSTEMSTATEMANAGER_H

#include <Arduino.h>

enum class SystemState {
  BOOT,
  NORMAL,
  SECURITY_ARMED,
  ALARM
};

// Sole owner of the FSM. No other module may mutate state directly —
// all transitions go through requestTransition().
class SystemStateManager {
public:
  void begin();
  SystemState current() const;

  // Returns false if the transition is invalid for the current state.
  bool requestTransition(SystemState next);

private:
  // TODO: implement — SystemState state_, transition validity table
};

#endif // AEGISHOME_SYSTEMSTATEMANAGER_H
