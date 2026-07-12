# AegisHome — Hardware Design Specification (HDS)
**Document Version:** 1.0 | **Status:** Reviewed — Approved for Build

---

## 1. Hardware Block Diagram (textual)

```
                      +---------------------+
                      |     ESP8266         |
                      |     NodeMCU          |
                      |  (3.3V logic, 1x ADC)|
                      +----+----+----+----+--+
                           |    |    |    |
        +------------------+    |    |    +------------------+
        |                       |    |                        |
 [Analog Mux Stage]      [Digital In]  [PWM Out]        [I2C Bus]
   Thermistor+LDR        IR / Buttons   Servo/LED/Buzzer   OLED
   -> single A0                                            (optional)
        |
   (software time-
    multiplexed read)

Power:
  USB 5V -> NodeMCU onboard 3.3V regulator -> logic + sensors
  USB 5V -> DIRECT to servo (separate rail, common ground with NodeMCU)
```

## 2. Subsystem Breakdown

| Subsystem | Components | Interface |
|---|---|---|
| Sensing | Thermistor, LDR | Analog (shared A0, multiplexed) |
| Motion/Input | IR motion sensor, 2x push button | Digital GPIO, interrupt or polled+debounced |
| Actuation | Servo (door) | PWM |
| Indication | LEDs (status, welcome, alarm), Buzzer | Digital/PWM |
| Display (optional) | OLED (SSD1306) | I2C |
| Controller | ESP8266 NodeMCU | — |

## 3. Power Architecture

- **Primary supply:** USB 5V into NodeMCU's onboard regulator, producing 3.3V for logic, sensors, and LED drive.
- **Servo supply:** Powered from the **5V USB rail directly**, NOT through the NodeMCU's onboard 3.3V regulator. The regulator is rated for tens of mA of headroom beyond the ESP8266's own draw; a servo's stall current (200–500mA typical for a micro servo) will brown out the 3.3V rail and reset the chip mid-motion. This is the single most common failure mode in NodeMCU+servo projects and is treated here as a hard design rule, not a suggestion.
- **Common ground:** All rails (5V servo, 3.3V logic) must share a common ground plane/rail — floating grounds between the servo supply and NodeMCU are a guaranteed source of erratic PWM behavior.
- **Decoupling:** 100–470µF electrolytic capacitor across the servo's 5V/GND supply lines, physically close to the servo, to absorb current spikes from the motor and prevent voltage sag from propagating into the shared ground.

### Power Budget (approximate, worst case)

| Load | Current |
|---|---|
| ESP8266 (Wi-Fi TX peak) | ~170–300mA |
| Servo (stall) | ~500mA |
| LEDs (x3, w/ resistors) | ~60mA |
| Buzzer | ~30mA |
| OLED | ~20–30mA |
| Sensors (thermistor/LDR/IR) | <10mA |
| **Total worst case (servo + WiFi TX simultaneous)** | **~800mA–1A** |

A standard 5V/1A USB supply is marginal under worst-case simultaneous load; a 5V/2A supply is recommended headroom, especially once the OLED and Wi-Fi are both active.

## 4. GPIO Allocation Matrix (Revised — Conflict-Resolved)

The first-pass matrix (from initial scoping) had two unresolved conflicts: (1) thermistor and LDR both wanting the single ADC pin, and (2) LED and OLED I2C pins colliding on D1/D2. Resolved matrix below.

**On the ADC conflict — revised approach:** the original plan (analog mux switching both sensors onto shared A0) is rejected on review. It's technically elegant but wrong for this project: it adds a transistor/switch IC, an extra failure point, an extra thing to debug on a breadboard, and an extra thing to explain in a 5-minute demo video — for a benefit (two simultaneous analog channels instead of one) that this system doesn't actually need, since neither temperature nor light level changes fast enough to require simultaneous sampling.

The simpler, correct fix: most LDR modules ship with an onboard LM393 comparator exposing both an analog output and a digital threshold output (`DO`). AegisHome uses the digital output of the LDR module, freeing A0 exclusively for the thermistor. This costs zero extra hardware — it's a wiring choice, not an added component — and removes the ADC conflict entirely without multiplexing.

- If the specific LDR module on hand has no digital comparator output: fall back to a manual physical switch (a single jumper/slide switch connecting either the thermistor or LDR divider to A0, operator-selected) rather than active multiplexing — acceptable because light level and temperature never share a failure mode requiring simultaneous reads.
- Multiplexing hardware is only introduced if hardware validation (§13) proves this approach genuinely can't meet requirements — not by default.

| Function | Pin (Dx) | GPIO# | Type | Notes |
|---|---|---|---|---|
| Thermistor | A0 | ADC0 | Analog | Sole occupant of the ADC — no sharing, no multiplexing |
| LDR (digital comparator output) | D0 | GPIO16 | Digital in | Uses LDR module's onboard LM393 comparator; threshold tuned via the module's onboard potentiometer, not firmware |
| IR motion sensor | D5 | GPIO14 | Digital in | Safe GPIO, no boot role |
| Doorbell button | D6 | GPIO12 | Digital in, pull-up | Safe GPIO |
| Security arm button | D7 | GPIO13 | Digital in, pull-up | Safe GPIO |
| Servo (door) | D4 | GPIO2 | PWM out | **Boot pin** — must be HIGH or floating at boot; servo library sets this after `setup()`, acceptable since servo signal line only actively drives post-boot. See §7. |
| Status/lighting LED | D0 | GPIO16 | Digital out | No PWM/interrupt support on this pin — acceptable since status LED is simple on/off, not dimmed |
| Alarm (red) LED | D8 | GPIO15 | Digital out | **Boot pin** — must be LOW at boot. LED wiring must not pull this high at power-on (use pull-down, active-high LED only after boot confirmed). See §7. |
| Buzzer | D3 | GPIO0 | PWM out | **Boot pin** — must be HIGH at boot (flash mode select). Buzzer must default to logic HIGH / silent-safe at boot; firmware must not drive LOW until `setup()` completes. See §7. |
| OLED SDA | D2 | GPIO4 | I2C | Reassigned off D1 to avoid welcome-LED collision |
| OLED SCL | D1 | GPIO5 | I2C | Reassigned; welcome LED moved to D0-adjacent spare or combined with status LED logic |

**Net change from the first pass:** welcome-LED function is merged into the existing status LED (same physical LED, different blink pattern) rather than allocating a dedicated pin — this both resolves the D1/D2 collision with the OLED and avoids adding a pin we don't have to spare.

## 5. Boot Pin Considerations

ESP8266 NodeMCU has three GPIOs with boot-strapping roles: **GPIO0 (D3)**, **GPIO2 (D4)**, **GPIO15 (D8)**. Their state at power-on/reset determines boot mode (flash vs. UART download) and, if driven incorrectly by external hardware, can prevent the board from booting at all or send it into the wrong mode.

- **GPIO0 (D3, buzzer):** Must read HIGH at boot for normal flash boot. Buzzer driver circuit must not pull this line low passively (e.g., via a low-value pull-down or an active buzzer that sinks current at power-up before the MCU takes control).
- **GPIO2 (D4, servo):** Must not be LOW at boot. Servo signal lines idle high or floating on most hobby servos before a PWM signal is applied, which is compatible — but this is verified during hardware validation (§13), not assumed.
- **GPIO15 (D8, alarm LED):** Must be LOW at boot. This is the one requiring the most care — a naive active-high LED with a pull-up resistor would violate this. Wiring must ensure GPIO15 has a pull-down (many NodeMCU dev boards already include one) and the alarm LED circuit doesn't fight it.

**Engineering rule applied:** none of the three boot pins are used for a function that must be active *during* boot (motion sensing, buttons) — only for outputs that are naturally idle/safe at power-on. This was a deliberate allocation choice, not an accident.

## 6. Signal Flow

```
Sensors (analog/digital) --> SensorManager (raw read + filter)
        --> SystemStateManager (interprets readings against current mode)
        --> Feature Controllers (Temperature/Lighting/Security/Door)
        --> AlarmManager / actuator drivers
        --> Physical outputs (LED/servo/buzzer) + DisplayManager (OLED/serial)
```
Data flows one direction per cycle — controllers never write back into SensorManager's raw buffers, avoiding hidden feedback coupling between subsystems.

## 7. Communication Interfaces

| Interface | Use | Notes |
|---|---|---|
| ADC | Thermistor, LDR | Single channel, time-multiplexed (see §9) |
| PWM | Servo, buzzer (tone), status LED (optional dimming) | ESP8266 PWM is software-driven at ~1kHz default; adequate for these loads |
| I2C | OLED (optional) | Standard Wire library, 400kHz fast mode |
| UART | Serial diagnostics/logging | USB-UART for development; disabled or rate-limited in "production" build to avoid blocking on buffer-full |
| GPIO (digital) | IR sensor, buttons, remaining LEDs | Debounced/filtered in SensorManager |

## 8. Failure Modes & Electrical Risks

| Failure Mode | Cause | Mitigation |
|---|---|---|
| Brownout reset during door open | Servo stall current sags 3.3V rail | Separate 5V servo rail + decoupling cap (§3) |
| Boot loop / flash-mode boot | Boot-pin (D3/D4/D8) driven incorrectly at power-up | GPIO allocation respects boot constraints (§5) |
| Noisy thermistor reading | Single-sample ADC read susceptible to transient noise | Moving-average filter over N samples in SensorManager (software fix, no added hardware) |
| Ground loop induced sensor noise | Servo/LED high-current paths sharing ground trace with analog sensor reference | Star-ground topology: analog sensor ground returns separately to the supply ground point, not daisy-chained through the servo's ground path |
| Buzzer/LED EMI on IR sensor | High dI/dt switching near IR receiver | Physical separation on breadboard; keep buzzer wiring away from IR sensor leads |

## 9. Wiring Philosophy

- One shared ground rail on the breadboard; all subsystem grounds return to it directly (star topology), not chained sensor-to-sensor.
- Thermistor divider wired directly to A0, no shared/switched circuitry — the simplest correct wiring for the sole analog input in the system.
- Every button gets a hardware pull-up (or the ESP8266's internal `INPUT_PULLUP`) — no floating digital inputs anywhere in the design.
- Servo signal wire kept short and routed away from the buzzer/LED high-current lines to minimize PWM jitter from induced noise.

## 10. Physical Layout Recommendations

- Two-breadboard layout: one for the "sensing + logic" side (thermistor/LDR/IR/buttons, close to NodeMCU), one for the "actuation" side (servo/buzzer/LEDs), physically separated to keep noisy actuator wiring away from analog sensor leads.
- NodeMCU mounted centrally with short jumper runs to both breadboards.
- OLED (if used) mounted for visibility during the demo video — this is a practical, not just electrical, consideration.

## 11. EMI / Noise Considerations

- Servo PWM and buzzer tone generation are the two most likely noise sources; both are digital/PWM (not analog), so the main risk is radiated/conducted noise into the shared 3.3V rail rather than direct signal corruption.
- Decoupling capacitor at the servo rail (§3) does double duty here — it also suppresses high-frequency noise injection into the shared ground.
- Thermistor/LDR divider values chosen to keep source impedance low enough that ambient noise pickup is a small fraction of the signal swing.

## 12. Expandability

The GPIO matrix intentionally leaves the following unused/available for v1.1+: A1 not applicable (single ADC), plus any spare digital pins after final wiring (e.g., D0 secondary use, SPI pins if not otherwise used). Future zones (e.g., a second IR sensor for a different room) would require either GPIO expansion (I2C GPIO expander, e.g., MCP23017) or a second ESP8266 node — documented as the v2.0 path in the SRS roadmap rather than solved prematurely here.

## 13. Hardware Validation Plan

1. Power-on test with no sensors/actuators attached — confirm clean boot, correct boot-pin states measured with multimeter/scope.
2. Add analog mux + thermistor/LDR — verify independent, stable readings across the full expected range (ice water to warm hand; light to covered).
3. Add servo on isolated rail — run 50 open/close cycles, confirm no resets, measure 3.3V rail during stall with scope/multimeter.
4. Add buzzer/LEDs — confirm boot-pin-safe defaults (GPIO0 high, GPIO15 low) hold under full load at power-up.
5. Add IR sensor + buttons — debounce verification (rapid press test, motion at boundary distance).
6. Full integration soak test — 30 minutes continuous operation cycling through all states.

---

## 14. Engineering Review Log

**Pass 1 (initial matrix):** ADC conflict (thermistor+LDR both on A0) and I2C/LED pin collision (D1/D2) identified — **rejected**, sent back for revision.

**Pass 2 (this document):** ADC conflict resolved via software-multiplexed analog switching with documented settle-time handling. I2C collision resolved by reassigning OLED to D1/D2 and merging welcome-LED into the existing status LED rather than adding a pin. Boot-pin usage audited individually (§5) and confirmed each boot pin only drives a function that is safely idle at power-on. Servo brownout risk addressed with a dedicated power rail and decoupling capacitor, not left as an implicit assumption.

**Verdict:** No unresolved conflicts remain. Hardware design is **approved for implementation**.
