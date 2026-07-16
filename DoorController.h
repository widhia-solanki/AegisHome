#ifndef AEGISHOME_DOORCONTROLLER_H
#define AEGISHOME_DOORCONTROLLER_H

#include <Arduino.h>
#include "config.h"
#include "Types.h"
#include "Utilities.h"
#include <Servo.h>

// Owns servo motion + welcome LED sequence. Non-blocking timed return.
// Does not read the security button. Reads from a SensorSnapshot.
class DoorController {
public:
  void begin();
  void update(const SensorSnapshot& snapshot);

  bool isOpen() const;
  ErrorCode getServoFault() const;

private:
    Servo doorServo_;

    Timer doorTimer_{Config::DOOR_DWELL_TIME_MS};

    bool isOpen_;

    ErrorCode servoFault_;
  };

#endif // AEGISHOME_DOORCONTROLLER_H
