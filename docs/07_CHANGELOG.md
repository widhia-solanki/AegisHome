# AegisHome — Changelog

All notable structural/architectural changes to the firmware skeleton are logged here, session by session. This is a design-process artifact as much as a code history — it's meant to show a reviewer how the architecture actually evolved, not just its final state.

## v0.1 — Firmware skeleton
- Initial module folder structure created: `config.h`, `SensorManager`, `SystemStateManager`, `TemperatureController`, `LightingController`, `DoorController`, `SecurityManager`, `AlarmManager`, `DisplayManager`, `Utilities`.
- Each module: header + empty `.cpp`, no logic implemented yet.
- `AegisHome.ino` created as pure orchestration shell.

## v0.2 — Architecture review pass
- **`.ino` vs `main.cpp`**: considered switching to PlatformIO's `main.cpp` convention for cleaner C++ semantics (no hidden prototypes). Decision: **keep `.ino`** — the project targets Arduino IDE for the internship submission, and introducing PlatformIO tooling now is scope creep with no payoff for this deliverable.
- **`SensorSnapshot` introduced**: `SensorManager` now produces exactly one `SensorSnapshot` per loop cycle instead of exposing live getters. All feature controllers (`TemperatureController`, `LightingController`, `DoorController`, `SecurityManager`) now take `const SensorSnapshot&` rather than a `SensorManager` reference. This closes a subtle class of bug where one controller could read an updated sensor value while another still saw the prior one, if `SensorManager`'s internal state changed mid-cycle.
- **`SystemStatus` introduced**: `DisplayManager` now consumes a single `SystemStatus` struct (state, fanActive, lightingOn, alarmActive, doorOpen, error) instead of referencing every feature controller individually. `DisplayManager` no longer needs to know these controllers exist.
- **`Config` namespace**: all tunable constants in `config.h` moved into `namespace Config` instead of bare globals. `ErrorCode` and the new `Severity` enum are left outside the namespace since they're types used throughout, not tunable values.
- **`Severity` enum added** (`INFO`, `WARNING`, `ERROR`, `FATAL`) — future-proofs the error/logging system for a point where not every logged event is a hard fault.
- **`Logger.h` added**: `LOG_INFO/WARN/ERROR/FATAL` macros wrapping `Serial.println()` today, structured so a future MQTT/network logging sink can be swapped into `Logger::write()` without touching any call site.
- **`EdgeDetector` utility — considered, postponed.** Would generalize edge-triggered event detection independent of `Debouncer`'s own timing. Not needed while `Debouncer` already covers every current digital input; revisit if event-detection logic starts duplicating across `SensorManager`.
- **`BootManager` — considered, postponed.** Boot init, diagnostics, animation, and fault handling are arguably their own subsystem already. Kept inline in `setup()` for v1 to avoid adding a module before there's enough boot-time complexity to justify it; flagged as the likely v1.1 refactor.

## v0.3 — Hardware-validated pin fixes, reprioritization, Logger implemented
- **`config.h` corrected against real hardware constraints**: `D9`/`D10` are not real Dx-labeled GPIOs on a standard NodeMCU — status/welcome LED reassigned to `D8`, **merged with the alarm LED** (one physical LED, pattern-differentiated) to close the resulting pin-count gap. Buzzer moved off `D3` (GPIO0, a boot-strapping pin that caused issues in testing) to `RX` (GPIO3) — safe since AegisHome never receives serial data, only transmits via `Serial.println` on TX.
- **These pin values are provisional pending `docs/08_HCF.md` confirmation against the actual board** — `config.h` now carries inline comments pointing back to the specific HCF rows that need to be validated and checked off.
- **Module implementation order reprioritized**: originally planned bottom-up (`Utilities` → `SensorManager` → `DisplayManager`). Changed to `Logger` → `DisplayManager` → `SensorManager` → `Utilities`, to get a real OLED boot screen visible quickly rather than optimizing purely for architectural build order.
- **`Logger` fully implemented** (first non-stub module in the codebase) — `[LEVEL][TAG] message` format to Serial, gated by `Config::DEBUG_LOG_ENABLED`. Every other module will call `LOG_INFO/WARN/ERROR/FATAL` instead of touching `Serial` directly.
- `AegisHome.ino` updated to call `LOGGER_BEGIN()` and log a boot-start message; remaining `setup()`/`loop()` bodies still TODO, in the new priority order.

## v0.4 — RX/buzzer approach rejected; full v1 GPIO freeze
- **RX-for-buzzer rejected on review.** Technically functional, but wrong as a long-term architectural choice: UART pins are special-purpose, and repurposing RX for an unrelated peripheral trades away future flexibility (interactive debugging, external serial devices, alternate firmware update workflows) for a one-off pin-count fix.
- **Full GPIO reallocation performed instead of another patch.** Key move: the doorbell button was placed on D3 (GPIO0) instead of the buzzer — a button wired `INPUT_PULLUP` idles HIGH by default, which naturally satisfies GPIO0's "must be HIGH at boot" requirement, whereas the buzzer's boot-time behavior was exactly what caused the original D3 failure. This freed D6 (a plain GPIO with no boot role) for the buzzer.
- **v1 hardware freeze pin map**: A0 thermistor · D0 LDR · D1 OLED SCL · D2 OLED SDA · D3 doorbell button · D4 servo · D5 IR · D6 buzzer · D7 security button · D8 indicator LED (merged status/welcome/alarm).
- **HCF updated with real validation results**: OLED address, thermistor analog range, both buttons' pull-up wiring, servo sweep, LDR polarity, and IR polarity are now all ✅ verified with dates. Boot-pin power-up states (row 11) explicitly flagged for re-test since D3's function changed from buzzer to button in this freeze — a passing result under the old assignment doesn't carry over.

## v0.6 — Servo power source: bring-up finding confirms the HDS's own risk analysis
- **Hardware bring-up:** Initial prototype powered the servo from the NodeMCU 3.3V rail. During testing, the servo exhibited unreliable behavior under load. The design was reverted to the intended architecture with the servo powered from the VIN/5V rail and a common ground, restoring stable operation.
- This is logged as a bring-up **finding**, not just a planning decision that was followed — the HDS (§3, Power Architecture) called out servo-on-3.3V brownout as the single most common failure mode in NodeMCU+servo projects before any hardware existed; this is the point where that risk was actually observed, not merely anticipated. `docs/08_HCF.md` row 6a records the same finding with the specific test performed (repeated open/close cycles, behavioral before/after comparison — no multimeter/scope measurement of voltage sag was taken, since none was available).
- `docs/08_HCF.md` row 6 (servo power source) updated from pending to ✅ validated on this basis. Row 12 (power supply headroom) remains deliberately pending — a stable servo is not the same claim as a measured current draw, and the HCF should not conflate the two.

## Planned (not yet done)
- v0.3 — Implement `Utilities` (`Debouncer`, `MovingAverage`, `Timer`) — first per the bottom-up integration strategy (FDS §15).
- v0.4 — Implement `SensorManager` against real hardware.
- v0.5 — Implement `SystemStateManager` FSM transition table.
- v0.6 — Implement remaining feature controllers.
- v0.7 — Full integration + 30-minute soak test (SRS Acceptance Criteria).
