// =====================================================================
// AegisHome.ino
// Orchestration only. All decision logic lives in the feature
// controllers below — this file wires modules together, threads the
// SensorSnapshot/SystemStatus structs through each cycle, and calls
// update() on each module once per loop, non-blocking.
//
// IMPLEMENTATION ORDER (revised again — see docs/07_CHANGELOG.md v0.5):
//   1. Utilities      — FIRST. SensorManager depends on it (Debouncer/
//                       MovingAverage/Timer), and DisplayManager will
//                       eventually need Timer too. Keeps dependencies
//                       flowing one direction instead of building
//                       "visible" modules ahead of what they rely on.
//   2. Logger
//   3. DisplayManager
//   4. SensorManager
// This supersedes the previous Logger-first ordering — that prioritized
// visible OLED progress, but building SensorManager (or even
// DisplayManager's timing) before Utilities existed would have meant
// writing throwaway inline debounce/timer code that just gets replaced.
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
  // 1. Utilities is a header-only/inline dependency, no begin() of its
  //    own — it's "first" in the sense that SensorManager needs it
  //    working correctly before SensorManager itself is implemented
  // 2. displayManager.begin() + power-up animation
  // 3. Remaining module begin() calls
  // 4. Run boot diagnostic checklist (thermistor/LDR/IR/servo/OLED),
  //    calling displayManager.renderBootStep() for each; LOG_ERROR any
  //    failures before continuing
  // 5. displayManager.renderReady()
  // 6. stateManager transitions BOOT -> NORMAL
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
