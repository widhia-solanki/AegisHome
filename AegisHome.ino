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

void setup()
{
    LOGGER_BEGIN();

    LOG_INFO("BOOT", "AegisHome starting");

    displayManager.begin();
    sensors.begin();
    stateManager.begin();
    temperatureController.begin();
    lightingController.begin();
    doorController.begin();
    securityManager.begin();
    alarmManager.begin();

    displayManager.renderBootStep("OLED", displayManager.getOledFault() == ErrorCode::NONE);
    delay(Config::BOOT_DIAGNOSTIC_STEP_MS);

    displayManager.renderBootStep("Servo", doorController.getServoFault() == ErrorCode::NONE);
    delay(Config::BOOT_DIAGNOSTIC_STEP_MS);

    displayManager.renderBootStep("Thermistor", true);
    delay(Config::BOOT_DIAGNOSTIC_STEP_MS);

    displayManager.renderBootStep("LDR", true);
    delay(Config::BOOT_DIAGNOSTIC_STEP_MS);

    displayManager.renderBootStep("IR", true);
    delay(Config::BOOT_DIAGNOSTIC_STEP_MS);

    displayManager.renderReady();

    stateManager.requestTransition(SystemState::NORMAL);

    LOG_INFO("BOOT", "Initialization complete");
}

void loop()
{
    sensors.update();

    const SensorSnapshot& snapshot = sensors.getSnapshot();

    temperatureController.update(snapshot);
    lightingController.update(snapshot);
    doorController.update(snapshot);
    securityManager.update(snapshot, stateManager);

    alarmManager.update(stateManager.current());

    SystemStatus status;

    status.state = stateManager.current();
    status.fanActive = temperatureController.isFanActive();
    status.lightingOn = lightingController.isLightingOn();
    status.alarmActive = (stateManager.current() == SystemState::ALARM);
    status.doorOpen = doorController.isOpen();

    if (snapshot.thermistorFault != ErrorCode::NONE)
        status.error = snapshot.thermistorFault;
    else if (snapshot.ldrFault != ErrorCode::NONE)
        status.error = snapshot.ldrFault;
    else if (snapshot.irFault != ErrorCode::NONE)
        status.error = snapshot.irFault;
    else if (doorController.getServoFault() != ErrorCode::NONE)
        status.error = doorController.getServoFault();
    else if (displayManager.getOledFault() != ErrorCode::NONE)
        status.error = displayManager.getOledFault();
    else
        status.error = ErrorCode::NONE;

    displayManager.update(status);
}
