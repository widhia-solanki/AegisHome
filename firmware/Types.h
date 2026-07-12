#ifndef AEGISHOME_TYPES_H
#define AEGISHOME_TYPES_H

#include <Arduino.h>
#include "Enums.h"

// ---------------------------------------------------------------------
// SensorSnapshot — one consistent view of all sensor/input state for a
// single loop iteration. SensorManager produces exactly one of these
// per update() call; every controller reads from the same snapshot
// rather than calling live getters, so no controller can observe a
// value mid-change relative to another controller in the same cycle.
// ---------------------------------------------------------------------
struct SensorSnapshot {
  float temperatureC        = 0.0f;
  bool  dark                = false;
  bool  motionDetected       = false; // edge-triggered, true only on the cycle it fires
  bool  doorbellPressed      = false; // edge-triggered
  bool  securityButtonPressed = false; // edge-triggered

  ErrorCode thermistorFault = ErrorCode::NONE;
  ErrorCode irFault         = ErrorCode::NONE;
  ErrorCode ldrFault        = ErrorCode::NONE;
};

// ---------------------------------------------------------------------
// SystemStatus — one consolidated render-ready view of system state.
// DisplayManager consumes only this; it does not need to know about
// TemperatureController, LightingController, SecurityManager, or
// DoorController individually.
// ---------------------------------------------------------------------
struct SystemStatus {
  SystemState state = SystemState::BOOT;
  bool fanActive     = false;
  bool lightingOn    = false;
  bool alarmActive   = false;
  bool doorOpen      = false;
  ErrorCode error    = ErrorCode::NONE; // highest-severity active fault, if any
};

#endif // AEGISHOME_TYPES_H
