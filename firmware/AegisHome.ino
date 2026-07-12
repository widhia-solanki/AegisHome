// =====================================================================
// AegisHome.ino
// Orchestration only. All decision logic lives in the feature
// controllers below — this file wires modules together, threads the
// SensorSnapshot/SystemStatus structs through each cycle, and calls
// update() on each module once per loop, non-blocking.
//
// IMPLEMENTATION ORDER (revised — see docs/07_CHANGELOG.md v0.3):
//   1. Logger        — DONE. Everything else depends on it for debugging.
//   2. DisplayManager — NEXT. Priority: real OLED boot screen, visible
//                       progress over architectural completeness.
//   3. SensorManager
//   4. Utilities      — pulled in as SensorManager needs it, not before.
// This is a deliberate reprioritization away from the original
// bottom-up plan (Utilities first) toward visible, demoable progress.
//
// NOTE (postponed — not v1): boot init/diagnostics/animation currently
// lives inline in setup() below. If it keeps growing, promoting it to
// its own BootManager is the right v1.1 refactor — not necessary yet.
// =====================================================================

#include "config.h"
#include "Types.h"
#include "Logger.h"
#include "Utilities.h"
#include "SensorManager.h"
#include "SystemStateManager.h"
#include "TemperatureController.h"
#include "LightingController.h"
#include "DoorController.h"
#include "SecurityManager.h"
#include "AlarmManager.h"
#include "DisplayManager.h"

SensorManager sensors;
SystemStateManager stateManager;
TemperatureController temperatureController;
LightingController lightingController;
DoorController doorController;
SecurityManager securityManager;
AlarmManager alarmManager;
DisplayManager displayManager;

void setup() {
  LOGGER_BEGIN();
  LOG_INFO("BOOT", "AegisHome starting");

  // TODO: implement boot sequence per FDS §11b —
  // 1. displayManager.begin() + power-up animation (NEXT priority)
  // 2. begin() remaining modules in bring-up order (Utilities pulled in
  //    as SensorManager needs it)
  // 3. Run boot diagnostic checklist (thermistor/LDR/IR/servo/OLED),
  //    calling displayManager.renderBootStep() for each; LOG_ERROR any
  //    failures before continuing
  // 4. displayManager.renderReady()
  // 5. stateManager transitions BOOT -> NORMAL
}

void loop() {
  // TODO: implement per FDS §7 (Task Scheduling Strategy) —
  // 1. sensors.update() — refreshes the single SensorSnapshot for this cycle
  // 2. const SensorSnapshot& snapshot = sensors.getSnapshot();
  // 3. Feed `snapshot` into temperatureController.update(), 
  //    lightingController.update(), doorController.update(),
  //    securityManager.update(snapshot, stateManager) — every controller
  //    reads the SAME snapshot instance, so nothing observes a
  //    mid-cycle-changed value relative to another controller
  // 4. alarmManager.update(stateManager.current())
  // 5. Build a SystemStatus from the controllers' current output
  //    (isFanActive(), isLightingOn(), isOpen(), isArmed(), etc.) and
  //    pass ONE SystemStatus instance into displayManager.update()
  // No delay() calls anywhere in this function.
}
