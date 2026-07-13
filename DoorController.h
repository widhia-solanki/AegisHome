#ifndef AEGISHOME_DOORCONTROLLER_H
#define AEGISHOME_DOORCONTROLLER_H

#include <Arduino.h>
#include "config.h"
#include "Types.h"
#include "Utilities.h"

// Owns servo motion + welcome LED sequence. Non-blocking timed return.
// Does not read the security button. Reads from a SensorSnapshot.
class DoorController {
public:
  void begin();
  void update(const SensorSnapshot& snapshot);

  bool isOpen() const;
  ErrorCode getServoFault() const;

private:
  // TODO: implement — servo object/wrapper, Timer for dwell time,
  // isOpen_ bool, fault flag, boot-time confirmatory sweep result
};

#endif // AEGISHOME_DOORCONTROLLER_H
