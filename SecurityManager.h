#ifndef AEGISHOME_SECURITYMANAGER_H
#define AEGISHOME_SECURITYMANAGER_H

#include <Arduino.h>
#include "Types.h"
#include "SystemStateManager.h"

// Owns arm/disarm logic and hands motion events to SystemStateManager.
// Does not directly drive the buzzer. Refuses to arm if IR sensor is
// faulted (see docs/03_FDS.md §11d). Reads from a SensorSnapshot.
class SecurityManager {
public:
  void begin();
  void update(const SensorSnapshot& snapshot, SystemStateManager& stateManager);

  bool isArmed() const;
  bool lastArmRequestRejected() const;

private:
    bool armed_;
    bool armRejected_;
};

#endif // AEGISHOME_SECURITYMANAGER_H
