# AegisHome — System Requirements Specification (SRS)
**Document Version:** 1.0 | **Status:** Reviewed | **Platform:** ESP8266 NodeMCU

---

## 1. Executive Summary

> AegisHome was designed as a modular embedded smart home controller with an emphasis on reliability, deterministic behavior, and maintainable firmware architecture. Rather than maximizing the number of peripherals, the project focuses on engineering practices such as finite state machines, non-blocking scheduling, structured diagnostics, and subsystem isolation.

AegisHome is a modular embedded smart-home controller built on ESP8266 NodeMCU. It monitors temperature, ambient light, and motion; controls a servo-actuated door with a welcome sequence; and provides an arm/disarm security mode with alarm response. The system is designed as a maintainable, extensible firmware platform (event-driven, FSM-based) rather than a single-purpose demo — the goal is a v1 product foundation that a v2 could extend with cloud connectivity, additional zones, or additional sensor types without a rewrite.

## 2. Problem Statement

Most hobbyist "smart home" boards are monolithic sketches: one `loop()`, global state, `delay()`-driven timing, and no separation between sensing, decision logic, and actuation. This makes them impossible to test, extend, or debug once complexity grows beyond a handful of `if` statements. AegisHome addresses this by building the same functional scope (temperature response, automatic lighting, intrusion detection, door control, security arming, alarms) on top of a proper module boundary and a non-blocking scheduler, so the codebase remains legible and extensible as features are added.

## 3. Goals

- G1: Deliver all six mandatory features with deterministic, testable behavior.
- G2: Non-blocking firmware — no feature may block another (e.g., door servo movement must not stall motion detection).
- G3: Modular boundaries such that any single subsystem (e.g., DoorController) can be unit-reasoned-about or swapped without touching unrelated modules.
- G4: Documented, reproducible hardware build (pin matrix, power budget, wiring philosophy).
- G5: A codebase and document set that stands on its own as a portfolio artifact — readable by someone who never saw the assignment brief.

## 4. Out of Scope (v1)

Explicitly excluded to protect the 70–80 hour budget and avoid feature bloat:
- Cloud/mobile app integration (MQTT/web dashboard deferred to roadmap, only added if time remains after core hardening).
- Persistent multi-user authentication/access control.
- Multi-room / multi-node mesh (single-node system only).
- Camera-based detection or any ML-based inference.
- OTA firmware updates.
- Battery/UPS backup power design.

Each exclusion exists because it would consume disproportionate engineering time relative to the grading/portfolio value it adds at this stage — not because it's technically inappropriate for the platform.

## 5. Stakeholders

| Stakeholder | Interest |
|---|---|
| Student (you) | Portfolio-quality deliverable, grading criteria, learning outcomes |
| LaunchED reviewers | Assignment compliance, demo clarity |
| Future self (v2 dev) | Codebase must be extensible without archaeology |
| Technical recruiter (reader) | Evidence of engineering judgment, not just working code |

## 6. Functional Requirements

| ID | Requirement |
|---|---|
| FR-1 | System shall sample thermistor temperature at a configurable interval (default 2s) and trigger a "fan" LED + 2s buzzer pulse when temperature exceeds a configurable threshold. |
| FR-2 | System shall sample LDR light level and automatically enable lighting LEDs when ambient light falls below a configurable threshold, with hysteresis to prevent flicker at the boundary. |
| FR-3 | System shall detect motion via IR sensor and respond according to current SystemState (NORMAL: log/indicate only; SECURITY_ARMED: trigger ALARM state). |
| FR-4 | System shall support a doorbell button that, on press, actuates the servo to a 90° "open" position, flashes a welcome LED, and returns the servo after a configurable dwell time. |
| FR-5 | System shall support arming/disarming Security Mode via a dedicated button (or long-press pattern), with a debounced, unambiguous state transition. |
| FR-6 | In ALARM state, system shall drive the buzzer with a distinct non-blocking pattern (not steady tone) and the red LED, until disarmed or a timeout/re-arm condition is met. |
| FR-7 | All sensor reads shall be debounced/filtered to reject transient noise (e.g., moving average for thermistor/LDR, debounce for IR/buttons). |
| FR-8 | System shall expose current state (mode, last trigger, sensor readings) via at least one observable channel (OLED and/or serial log) at all times. |

## 7. Non-Functional Requirements

| ID | Requirement |
|---|---|
| NFR-1 | No `delay()` calls longer than ~10ms anywhere in the main execution path; all timing via `millis()`. |
| NFR-2 | Loop iteration time shall remain low enough that IR motion and button debounce are never missed (target: main loop <10ms typical). |
| NFR-3 | Each module shall be independently compilable/testable in isolation (header defines a clear public API). |
| NFR-4 | Firmware shall recover cleanly from a watchdog reset without requiring manual re-arm of security state (state persisted or safely defaulted). |
| NFR-5 | Codebase shall contain zero magic numbers in logic files — all thresholds/pins/timings in `config.h`. |
| NFR-6 | System shall be maintainable by a third party using only the FDS and inline comments (no tribal knowledge required). |

## 8. Engineering Constraints

- ESP8266 has **exactly one ADC channel (A0)** — this is the single most consequential hardware constraint on the whole project and must be resolved in the HDS (see Section 9 there), not glossed over.
- Several NodeMCU GPIOs are boot-strapping pins (D3/GPIO0, D4/GPIO2, D8/GPIO15) with restrictions on idle/boot-time state — device pin assignment must respect these.
- Servo current draw during stall/movement can brown-out the 3.3V regulator if powered from the NodeMCU's onboard rail — must be addressed in the power architecture.
- Time budget is 70–80 hours total across hardware, firmware, docs, and video — this constrains how much of the "optional" feature list is realistic.

## 9. Acceptance Criteria

A build is considered v1-complete when:
1. All FR-1 through FR-8 are demonstrable live, without code changes between demos.
2. The system runs for a 30-minute soak test with no lockups, no missed motion events, no false alarm triggers from sensor noise.
3. Security arm/disarm and alarm trigger/clear are demonstrated back-to-back without a reset.
4. Pin matrix in the HDS matches the physical build exactly.
5. All three documents (SRS/HDS/FDS) and README are present and internally consistent with the shipped code.

## 10. Use Cases

**UC-1 — Nightfall Lighting:** Ambient light drops → LDR reading crosses threshold → LightingController enables LEDs without operator action.

**UC-2 — Visitor at Door:** Visitor presses doorbell → DoorController rotates servo to open position, flashes welcome LED → after dwell time, servo returns to closed position.

**UC-3 — Arming for the Night:** User double-presses security button → SecurityManager transitions NORMAL → SECURITY_ARMED → OLED/LED confirms armed state.

**UC-4 — Intrusion While Armed:** IR sensor detects motion while SECURITY_ARMED → SecurityManager transitions to ALARM → AlarmManager drives buzzer pattern + red LED → user disarms via button → system returns to NORMAL.

**UC-5 — Overheat Event:** Thermistor crosses threshold → TemperatureController enables "fan" LED and pulses buzzer for 2s → auto-clears when temperature drops below threshold minus hysteresis band.

## 11. Future Roadmap (Post-v1)

- v1.1: OLED status dashboard (readings, mode, uptime).
- v1.2: Wi-Fi connectivity + local web dashboard (read-only telemetry).
- v1.3: MQTT publish for integration with Home Assistant / Node-RED.
- v2.0: Multi-zone support (multiple IR/door pairs), persistent config over EEPROM/LittleFS, OTA updates.
- v2.x: RTOS migration path (see FDS §"Future RTOS Migration") if task count/complexity grows.

## 12. Risk Analysis

| Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|
| Single ADC forces LDR/thermistor time-sharing, adding latency | High | Medium | Software-multiplexed sampling with defined read order; documented in HDS |
| Servo brownout resets NodeMCU mid-demo | Medium | High | Separate servo power rail + decoupling capacitor (HDS §Power Architecture) |
| False alarm triggers from IR sensor noise near boundary conditions | Medium | Medium | Debounce + minimum-hold-time before state transition |
| Scope creep from optional features consuming core-feature hardening time | Medium | High | Optional features explicitly time-boxed and cut first if behind schedule |
| Boot-pin misuse causes unreliable boot/flash behavior | Low-Medium | High | GPIO allocation matrix cross-checked against ESP8266 boot constraints before wiring |

---

## 13. Design Review

**As Embedded Engineer:** The non-blocking requirement (NFR-1/NFR-2) is correctly elevated to a first-class requirement rather than a style preference — this is what actually separates this from a typical student board. Approved.

**As Robotics Engineer:** Servo behavior (FR-4) needs an explicit "return-to-safe-position on boot" requirement — added implicitly via NFR-4 but should be called out. *(Resolved: NFR-4 covers state recovery; DoorController defaults to closed on init per FDS.)*

**As IoT Architect:** Good discipline deferring Wi-Fi/MQTT to roadmap rather than bolting it on for buzzword value — a single-ADC, single-core MCU doing real-time sensing plus a web server without justification is a common overreach. Approved as scoped.

**As Technical Recruiter:** This reads like a real spec, not an assignment writeup — Section 4 (Out of Scope) in particular signals judgment, since most student docs never say no to anything. Approved.

No unresolved major flaws remain. SRS is final for v1.
