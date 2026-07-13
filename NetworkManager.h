#ifndef AEGISHOME_NETWORKMANAGER_H
#define AEGISHOME_NETWORKMANAGER_H

#include <Arduino.h>
#include "config.h"
#include "Types.h"

// NOTE (v1 scope): NetworkManager is NOT part of the v1 mandatory
// feature set (see docs/01_SRS.md §4 Out of Scope, §11 Future Roadmap
// v1.2). It exists here as a stub so the module boundary is reserved
// in the folder structure ahead of time, per docs/03_FDS.md §16 —
// when built out, it should subscribe to the same SystemStatus/
// SensorSnapshot data the rest of the firmware already produces,
// without requiring changes to any existing controller. Do not wire
// this into AegisHome.ino's setup()/loop() until v1's six mandatory
// features are fully implemented and soak-tested.
class NetworkManager {
public:
  void begin();
  void update();

  bool isConnected() const;

  // Publishes a read-only telemetry snapshot (Wi-Fi/MQTT/local web
  // dashboard — backend undecided, see SRS roadmap v1.2/v1.3).
  void publishStatus(const SystemStatus& status);

private:
  // TODO: implement — connection state, reconnect backoff timer,
  // whichever backend is chosen (plain Wi-Fi web server vs MQTT client)
};

#endif // AEGISHOME_NETWORKMANAGER_H
