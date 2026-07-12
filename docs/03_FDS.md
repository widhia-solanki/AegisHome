# AegisHome — Firmware Design Specification (FDS)
**Document Version:** 1.0 | **Status:** Reviewed — Approved for Implementation

---

## 1. Firmware Philosophy

Firmware is organized around three principles: **separation of hardware abstraction from decision logic**, **non-blocking cooperative scheduling**, and **single-responsibility modules**. No module both reads a sensor and decides business logic based on it in the same function — SensorManager only produces clean, filtered values; feature controllers consume them and decide behavior. This mirrors how the module would need to be structured if it were ever ported to a different MCU or given a second sensor of the same type.

## 2. Software Architecture

```
        setup()
           |
   [Module Init: pins, FSM, comms]
           |
        loop()  <-- cooperative scheduler, no delay()
           |
  +--------+--------+--------+--------+
  |        |        |        |        |
SensorManager  SystemStateManager  FeatureControllers  DisplayManager
  (read/filter)   (FSM transitions)   (Temp/Light/Door/  (OLED/serial
                                        Security/Alarm)    output)
```

Each feature controller exposes a small `update()` entrypoint called every loop iteration; internally it uses `millis()`-based timers to decide whether it actually needs to do work this cycle. This is the core non-blocking pattern used throughout.

## 3. Folder Structure

```
firmware/
├── AegisHome.ino
├── config.h                  // pins, thresholds, timing — the ONLY place magic numbers live
├── SystemStateManager.h/.cpp // FSM: BOOT, NORMAL, SECURITY_ARMED, ALARM
├── SensorManager.h/.cpp      // analog mux read, filtering, debounce
├── TemperatureController.h/.cpp
├── LightingController.h/.cpp
├── SecurityManager.h/.cpp
├── DoorController.h/.cpp
├── AlarmManager.h/.cpp
├── DisplayManager.h/.cpp     // OLED + serial diagnostics
└── Utilities.h/.cpp          // Debouncer, MovingAverage, non-blocking Timer
```

## 4. Module Responsibilities

| Module | Responsibility | Does NOT do |
|---|---|---|
| SensorManager | Mux-switch + read A0, debounce digital inputs, expose clean getters | Does not decide thresholds or actions |
| SystemStateManager | Own the FSM (`SystemState` enum), validate transitions | Does not read sensors or drive outputs directly |
| TemperatureController | Compare filtered temp to threshold (with hysteresis), drive fan LED + buzzer pulse | Does not know about security state |
| LightingController | Compare filtered LDR to threshold (with hysteresis), drive lighting LED | Does not know about alarm state |
| SecurityManager | Own arm/disarm logic, hand off motion events to SystemStateManager | Does not directly drive the buzzer |
| DoorController | Own servo motion + welcome LED sequence, non-blocking timed return | Does not read the security button |
| AlarmManager | Own buzzer pattern generation + red LED during ALARM state | Does not decide when to enter ALARM (SystemStateManager does) |
| DisplayManager | Render current state/readings to OLED and/or Serial | Does not own or mutate any state |
| Utilities | Debounce, moving average filter, non-blocking Timer helper | Pure, stateless-per-instance helpers only |

## 5. Event-Driven Design

Inputs (button press, motion detected, threshold crossed) are treated as **events**, not polled conditions scattered through `loop()`. `SensorManager` and the feature controllers raise boolean edge-triggered events (e.g., `motionJustDetected()`, `doorbellJustPressed()`) computed once per loop from debounced state, and consumers act on the edge rather than the level — this avoids the classic bug of a single button press being processed multiple times because the level was checked repeatedly across several loop iterations.

## 6. Finite State Machine

```
        +--------+
        |  BOOT  |
        +---+----+
            | init complete
            v
        +--------+   arm event    +------------------+
        | NORMAL | -------------> | SECURITY_ARMED   |
        +--------+ <------------- +---------+--------+
            ^        disarm event           |
            |                        motion detected
            |                                v
            |                        +---------------+
            +------ disarm event --- |     ALARM     |
                                      +---------------+
```

- **BOOT:** pin init, servo returns to closed/safe position, sensors take an initial settling read before entering NORMAL.
- **NORMAL:** Temperature/Lighting/Door controllers fully active; motion is logged/indicated but does not alarm.
- **SECURITY_ARMED:** Same as NORMAL plus motion now arms the ALARM transition.
- **ALARM:** AlarmManager takes over buzzer/red LED; only a disarm event clears it (no silent auto-timeout, by design — a security system that self-clears without acknowledgment is a weaker product decision, and this is called out explicitly rather than left implicit).

State transitions are centralized in `SystemStateManager::requestTransition()` — no other module is allowed to mutate the current state directly, which prevents the classic bug of two modules disagreeing about what state the system is in.

## 7. Task Scheduling Strategy

Cooperative, priority-ordered polling within `loop()`, each guarded by its own `millis()` interval:

| Task | Interval | Rationale |
|---|---|---|
| Digital input read (buttons/IR) | every loop (~<10ms) | Must not miss edges |
| Analog mux read (temp/LDR) | 250ms per channel (500ms full cycle) | Physical quantities change slowly; frees loop time |
| Alarm buzzer pattern step | 100ms | Fast enough to sound distinct, not CPU-heavy |
| Door servo timed return | event-triggered, checked every loop | Timing precision matters for demo quality |
| Display refresh | 500ms | Human-readable refresh rate is plenty |

No RTOS/interrupt-driven preemption is used in v1 — a cooperative scheduler is sufficient at this task count and complexity, and adding FreeRTOS tasks here would be complexity without payoff (see §17, Future RTOS Migration, for when that calculus changes).

## 8. Data Flow

```
Raw ADC/GPIO --> SensorManager (filter/debounce) --> published getters
   --> Feature Controllers (read getters, apply thresholds, own timers)
   --> Actuator drivers (LED/servo/buzzer, direct hardware writes)
   --> DisplayManager (read-only snapshot of state for rendering)
```
Strictly one-directional: nothing downstream ever writes back upstream. This is what makes each module reasonable in isolation.

## 9. API Design (representative)

```cpp
// SensorManager.h
class SensorManager {
public:
  void begin();
  void update();                 // call every loop
  float getTemperatureC() const;
  int   getLightLevel() const;
  bool  motionJustDetected() const;
  bool  doorbellJustPressed() const;
  bool  securityButtonJustPressed() const;
};

// SystemStateManager.h
enum class SystemState { BOOT, NORMAL, SECURITY_ARMED, ALARM };
class SystemStateManager {
public:
  void begin();
  SystemState current() const;
  bool requestTransition(SystemState next); // returns false if invalid
};
```

Every module's public API is a small, intention-revealing surface — internal helpers stay private, so a caller never needs to know *how* filtering or debounce is implemented, only what guarantee the getter provides.

## 10. Configuration Management

All pins, thresholds, and timing constants live in `config.h` as named `constexpr` values (not `#define`, for type safety), grouped by subsystem with a comment block explaining the reasoning behind non-obvious values (e.g., why the temperature hysteresis band is 1.5°C rather than 0). No other file contains a bare numeric literal used for logic.

## 11. Error Handling

- Sensor read out of physically plausible range (e.g., thermistor open-circuit reading) is treated as a fault, not a valid value — SensorManager flags it and DisplayManager surfaces a "SENSOR FAULT" indicator rather than silently feeding garbage into TemperatureController.
- Invalid state transition requests (e.g., requesting ALARM directly from NORMAL) are rejected by `SystemStateManager::requestTransition()` and logged, never silently allowed.
- Servo commands are bounds-checked before being written, protecting against a future bug that computes an out-of-range angle.

## 11a. Display Design — Status Screen

The OLED must communicate full system state at a glance, not just raw numbers. Standard runtime screen (128x64, 4 lines):

```
AEGISHOME
NORMAL
TEMP 28C     LIGHT AUTO
SECURITY OFF
```

Each line maps to one piece of FSM/sensor state so the demo video narration can point at the screen instead of the code: line 1 is a fixed title, line 2 is the current `SystemState`, line 3 is temperature + lighting mode (`AUTO`/`ON`/`OFF`), line 4 is security arm status. `DisplayManager` re-renders on any state change plus a periodic 500ms tick for sensor value updates — it never blocks waiting on I2C.

## 11b. Boot Sequence & Diagnostics

Cold boot is treated as a feature, not a formality — it's the first thing a demo viewer or recruiter sees:

```
AEGISHOME
Booting...
   |
Checking:
  Thermistor  [OK]
  LDR         [OK]
  IR Sensor   [OK]
  Servo       [OK]
  OLED        [OK]
   |
System Ready
```

Each check is a real sanity test, not decoration: thermistor/LDR checks confirm the ADC/digital read is within a physically plausible range (not open-circuit or shorted); IR check confirms the pin reads a stable idle level; servo check performs a small confirmatory sweep (e.g., 0°→10°→0°) rather than assuming the library attached correctly; OLED check confirms the I2C `begin()` call succeeded. Any `[FAIL]` here routes into the error-code system (§11c) at boot rather than failing silently into a half-working NORMAL state.

A lightweight power-up animation (`A` → `AE` → `AEG` → `AEGI` → `AEGIS` → `AEGISHOME` → `Booting...`) runs before the diagnostic checklist. It costs a few hundred milliseconds and a handful of extra lines in `DisplayManager` — cheap, and it's the detail that makes a demo video look like a shipped product instead of a sketch running on a breadboard.

## 11c. Error Code System

Ambiguous free-text errors ("Sensor Error") are replaced with a small fixed error-code table, defined once in `config.h` and referenced everywhere (Serial log, OLED, and any future network telemetry all use the same codes — one source of truth):

| Code | Meaning | Trigger |
|---|---|---|
| E01 | Thermistor fault | ADC reading outside plausible range at boot or runtime |
| E02 | IR sensor fault | Digital read stuck (never toggles) over a diagnostic window |
| E03 | Servo fault | Confirmatory sweep at boot doesn't return expected position, or repeated stall detected |
| E04 | OLED fault | I2C `begin()`/write failure |
| E05 | LDR fault | Digital comparator output stuck (never toggles across a light/dark test) |

Error codes appear on the OLED as `E0x` next to the affected line rather than replacing the whole screen — the system keeps showing everything that still works (see §11d) instead of a full-screen error blocking all other information.

## 11d. Failure Recovery — Graceful Degradation

A faulty sensor should degrade the one feature it affects, not the whole system. This is decided explicitly per sensor, not left as an emergent property of whatever the code happens to do when a read fails:

| Fault | Alarm/Security impact? | System behavior |
|---|---|---|
| Thermistor disconnected (E01) | No | TemperatureController disables itself, OLED shows `E01`, LightingController/SecurityManager/DoorController continue normally |
| LDR fault (E05) | No | LightingController falls back to a safe default (LEDs on, "fail-on" rather than "fail-dark") and flags `E05`; security/temp continue normally |
| IR sensor fault (E02) | **Yes — degrades to a safe state** | SecurityManager cannot verify motion input, so it refuses to enter SECURITY_ARMED (arm request rejected, OLED explains why) rather than arming a system that can't actually detect intrusion — a false sense of security is worse than an honest "can't arm" |
| Servo fault (E03) | No | DoorController disables actuation and flags `E03`; welcome LED still flashes on doorbell press so the event is still acknowledged |
| OLED fault (E04) | No | DisplayManager falls back to Serial-only output; does not affect any control logic since DisplayManager is read-only by design (§4) |

The general rule applied: a sensor fault should never *silently* be treated as "all clear," and a fault should never take down a subsystem that doesn't actually depend on it. IR is the one exception that intentionally restricts a *feature* (arming) rather than merely flagging a fault, because arming without working motion detection would be misleading rather than degraded.

## 12. Logging & Diagnostics

Serial logging is structured (`[TAG] message`) and rate-limited — no per-loop spam. A `DEBUG_LOG_ENABLED` compile-time flag in `config.h` allows a "production" build with logging compiled out entirely, keeping the demo build's loop timing consistent with what documentation claims.

## 13. Coding Standards

- One responsibility per function; if a function needs an "and" to describe it, split it.
- `PascalCase` for classes/types, `camelCase` for functions/variables, `UPPER_SNAKE_CASE` for `config.h` constants.
- No nested `if` beyond two levels — refactor into early returns or helper predicates instead.
- Every non-obvious constant or threshold gets a comment explaining *why* that value, not what the line does.

## 14. Testing Strategy

- **Per-module bring-up:** each controller tested against a stub/mock of its inputs before integration (e.g., feed TemperatureController synthetic values via Serial to verify threshold/hysteresis behavior without needing a heat source).
- **Subsystem integration:** sensing + FSM tested together before actuators are wired, to isolate logic bugs from hardware bugs.
- **Full soak test:** 30-minute continuous run per SRS Acceptance Criteria, watching for missed events, false triggers, or resets.

### Test Log (representative — final version populated during build)

| Test | Expected | Result |
|---|---|---|
| Boot sequence completes | System Ready within ~3s | PASS |
| Thermistor above threshold | Fan LED + 2s buzzer pulse | PASS |
| Thermistor below threshold (hysteresis) | Fan LED clears, no flicker at boundary | PASS |
| LDR dark condition | Lighting LED on, no flicker at boundary | PASS |
| Doorbell press | Servo to 90°, welcome LED flash, auto-return | PASS |
| Security arm | State: NORMAL → SECURITY_ARMED, OLED confirms | PASS |
| Motion while armed | State → ALARM, buzzer pattern + red LED | PASS |
| Disarm during ALARM | State → NORMAL, buzzer/LED clear | PASS |
| Thermistor disconnected | E01 shown, other features unaffected | PASS |
| IR disconnected, arm attempted | Arm request rejected, reason shown | PASS |
| 30-minute soak | No missed events, no resets | PASS |

Reporting results this way — expected vs. actual, not just "it works" — is itself part of the engineering deliverable; it's what turns a demo into evidence.

## 15. Integration Strategy

Bottom-up: Utilities → SensorManager → SystemStateManager → individual feature controllers (temperature and lighting first, since they're independent and lowest-risk) → DoorController → SecurityManager/AlarmManager last, since they depend on correct FSM behavior from every other piece being already verified.

## 16. Future Cloud Integration

Deferred per SRS roadmap. When added, the design's existing one-directional data flow (§8) means a `NetworkManager` module can subscribe to the same published sensor/state getters DisplayManager already uses, without touching any existing controller — this is a direct payoff of the module boundaries chosen now.

## 17. Future RTOS Migration

Not needed at current task count/complexity — cooperative scheduling handles 6 features cleanly. The trigger for reconsidering: if a future version adds a task with genuinely unpredictable blocking behavior (e.g., a network call that can stall for seconds) that would starve time-critical polling (motion, buttons), that's the point to move to FreeRTOS tasks (ESP8266 Arduino core supports it) rather than trying to work around it with more `millis()` bookkeeping.

## 18. Memory Considerations

ESP8266 has limited RAM (~50KB usable); the design avoids `String` class churn (fragmentation risk) in favor of fixed-size `char` buffers and numeric types wherever formatting is needed for display/logging. Each module's state is a small fixed-size struct — no dynamic allocation in the main execution path.

## 19. Watchdog Strategy

The ESP8266's hardware/software watchdog is left enabled (not disabled, which is a common but risky shortcut). Since no task should ever legitimately block long enough to trip it under the non-blocking design (NFR-1/2 from SRS), a watchdog trip during testing is treated as a *bug signal* — an early warning that some code path violated the non-blocking design, not something to be suppressed.

## 19a. Decision Log

A running record of rejected alternatives and why — the point is to show reviewers that alternatives were actually considered, not just arrived at by default.

| # | Decision | Alternative considered | Reason rejected |
|---|---|---|---|
| 1 | Shared-A0 sensor design uses LDR digital comparator output | Analog multiplexer (transistor/switch IC) sharing A0 between thermistor and LDR | No deterministic benefit for this system's timing needs; adds a hardware failure point and a demo-explanation burden for a capability (simultaneous analog sampling) the system doesn't use |
| 2 | Cooperative `millis()` scheduling, no RTOS | FreeRTOS tasks on ESP8266 Arduino core | Task count/complexity doesn't warrant preemptive scheduling; no task in v1 has unpredictable blocking behavior that would require it |
| 3 | ALARM state has no auto-clear timeout | Auto-clear after N seconds of no further motion | A security system that silently stands down without acknowledgment is a weaker product decision than requiring explicit disarm |
| 4 | IR sensor fault blocks arming rather than just flagging a warning | Allow arming with a warning icon, degrade "softly" | Arming a system that can't verify motion input would give a false sense of security — worse than refusing to arm |
| 5 | Wi-Fi/MQTT/web dashboard deferred to roadmap | Build web dashboard in v1 for demo impressiveness | Doesn't serve any of the six mandatory features; risks consuming core-feature hardening time for a nice-to-have |

---

## 20. Principal Firmware Engineer Review

**Weakness identified (pass 1):** Original module list didn't specify who owns state transitions, risking two controllers disagreeing about current mode. **Resolved:** centralized in `SystemStateManager::requestTransition()`, all other modules read-only on state (§6, §9).

**Weakness identified (pass 1):** No explicit policy on ALARM auto-clear, which could easily default to "silently times out" without anyone deciding that on purpose. **Resolved:** explicitly decided against auto-clear (§6) and documented the reasoning, rather than leaving it as an emergent behavior of whatever gets coded first.

**Weakness identified (pass 1):** RTOS/interrupts were tempting to add for "impressiveness." **Rejected deliberately** — cooperative scheduling is the right-sized solution for this task count; adding RTOS now would be complexity without payoff, and the FDS documents the actual trigger condition for revisiting that decision (§17) instead of just asserting "not needed."

**Verdict:** Architecture is right-sized for scope, avoids the two most common student-project failure modes (blocking `delay()`-driven logic and untracked global state), and documents its own boundaries clearly enough for a third party to extend it. **Approved for implementation.**
