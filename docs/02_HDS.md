# AegisHome — Hardware Design Specification (HDS)
**Document Version:** 1.1 | **Status:** Reviewed — Approved for Build, GPIO sections synced to v1 Hardware Configuration Freeze

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
   [Analog In]            [Digital In]  [PWM/Digital Out]  [I2C Bus]
   Thermistor (A0)         IR / Buttons   Servo/LED/Buzzer   OLED

Power:
  USB 5V -> NodeMCU onboard 3.3V regulator -> logic + sensors
  USB 5V -> DIRECT to servo (separate rail, common ground with NodeMCU)
```

*(Note: the "analog mux stage" shown in the original first-pass diagram was rejected — see §4 below. Thermistor is the sole analog input; LDR moved to a digital comparator output.)*

## 2. Subsystem Breakdown

| Subsystem | Components | Interface |
|---|---|---|
| Sensing | Thermistor (analog), LDR (digital comparator) | A0 (analog) + GPIO (digital) — no shared/multiplexed pin |
| Motion/Input | IR motion sensor, 2x push button | Digital GPIO, polled + debounced |
| Actuation | Servo (door) | PWM |
| Indication | Single indicator LED (status/welcome/alarm, merged), Buzzer | Digital |
| Display | OLED (SSD1306) | I2C |
| Controller | ESP8266 NodeMCU | — |

## 3. Power Architecture

- **Primary supply:** USB 5V into NodeMCU's onboard regulator, producing 3.3V for logic, sensors, and LED drive.
- **Servo supply:** Powered from the **5V USB rail directly**, NOT through the NodeMCU's onboard 3.3V regulator. The regulator is rated for tens of mA of headroom beyond the ESP8266's own draw; a servo's stall current (200–500mA typical for a micro servo) will brown out the 3.3V rail and reset the chip mid-motion. This is the single most common failure mode in NodeMCU+servo projects and is treated here as a hard design rule, not a suggestion. Validated via sweep test — no resets observed.
- **Common ground:** All rails (5V servo, 3.3V logic) must share a common ground plane/rail — floating grounds between the servo supply and NodeMCU are a guaranteed source of erratic PWM behavior.
- **Decoupling:** 100–470µF electrolytic capacitor across the servo's 5V/GND supply lines, physically close to the servo, to absorb current spikes from the motor and prevent voltage sag from propagating into the shared ground.

### Power Budget (approximate, worst case)

| Load | Current |
|---|---|
| ESP8266 (Wi-Fi TX peak) | ~170–300mA |
| Servo (stall) | ~500mA |
| Indicator LED (single, w/ resistor) | ~20mA |
| Buzzer | ~30mA |
| OLED | ~20–30mA |
| Sensors (thermistor/LDR/IR) | <10mA |
| **Total worst case (servo + WiFi TX simultaneous)** | **~750mA–950mA** |

A standard 5V/1A USB supply is marginal under worst-case simultaneous load; a 5V/2A supply is recommended headroom, especially once the OLED and Wi-Fi are both active. *(Note: LED load revised down from the original ~60mA/3-LED estimate — the hardware freeze consolidated status/welcome/alarm onto a single physical LED. This row remains pending final measurement per HCF row 12.)*

## 4. GPIO Allocation Matrix — v1 Hardware Freeze

**This section supersedes the original first-pass matrix entirely.** It is synced to `firmware/config.h` and `docs/05_Pin_Allocation_Matrix.md`, pin for pin. See `docs/08_HCF.md` for the full validation trail and `docs/07_CHANGELOG.md` v0.3–v0.5 for how each reassignment below was decided.

**On the ADC conflict (unchanged from the original design review):** the original plan (analog mux switching both sensors onto shared A0) was rejected on review before any hardware was built — it adds a transistor/switch IC, an extra failure point, and an extra thing to explain in a demo video, for a benefit (simultaneous analog sampling) this system never needed. The LDR module's onboard LM393 comparator digital output is used instead, freeing A0 exclusively for the thermistor. This part of the design held up unchanged through hardware bring-up.

**On the buzzer/LED pin conflicts (discovered during hardware bring-up, not anticipated in the original design review):** two problems surfaced that the original matrix didn't anticipate — D9 (planned for the status/welcome LED) does not exist as a real Dx-labeled GPIO on a standard NodeMCU, and the buzzer failed to boot reliably when wired to D3 (GPIO0, a boot-strapping pin). An interim fix moving the buzzer to RX (GPIO3) was considered and **rejected on review** — it works, but repurposes a UART pin for an unrelated peripheral, trading away future interactive-debugging/firmware-update flexibility for a one-off fix. The final resolution was a full reallocation: the doorbell button moved to D3 instead (its `INPUT_PULLUP` idle-HIGH state naturally satisfies GPIO0's boot requirement, which the buzzer could never do), freeing D6 — a plain GPIO with no boot role — for the buzzer. The status/welcome/alarm LED functions were merged onto a single physical LED on D8, closing the pin-count gap left by D9's absence.

| Function | Pin (Dx) | GPIO# | Type | Notes |
|---|---|---|---|---|
| Thermistor | A0 | ADC0 | Analog | Sole occupant of the ADC — no sharing, no multiplexing. Validated. |
| LDR (digital comparator output) | D0 | GPIO16 | Digital in | Uses LDR module's onboard LM393 comparator; polarity validated (LOW = dark) |
| OLED SCL | D1 | GPIO5 | I2C | Address 0x3C validated |
| OLED SDA | D2 | GPIO4 | I2C | |
| **Doorbell button** | **D3** | **GPIO0** | Digital in, pull-up | **Boot pin** — must be HIGH at boot. `INPUT_PULLUP` idles HIGH by default, satisfying this requirement — this is why the button lives here and not the buzzer. See §5. |
| Servo (door) | D4 | GPIO2 | PWM out | **Boot pin** — must be HIGH or floating at boot; servo library only drives after `setup()`. Sweep-tested, validated. See §5. |
| IR motion sensor | D5 | GPIO14 | Digital in | Safe GPIO, no boot role. Polarity validated. |
| **Buzzer** | **D6** | **GPIO12** | Digital out (non-blocking pattern) | Safe GPIO, no boot role. Reassigned here after D3 (boot failure) and RX (rejected on review) were both tried first. |
| Security arm button | D7 | GPIO13 | Digital in, pull-up | Safe GPIO. Pull-up wiring validated. |
| **Indicator LED (merged status/welcome/alarm)** | **D8** | **GPIO15** | Digital out | **Boot pin** — must be LOW at boot. One physical LED, pattern-differentiated, instead of separate LEDs on separate pins — this is what closes the pin budget after D9 turned out not to exist. See §5. |

**Net change from the original design:** three reassignments (doorbell button → D3, buzzer → D6, three LED roles merged onto D8) were forced by hardware bring-up discoveries that couldn't have been caught in desk review — module silkscreen realities (no D9) and boot-pin behavior under real current draw (buzzer sinking current at boot on D3). This is exactly the gap the HCF process (§13, and `docs/08_HCF.md`) exists to catch.

## 5. Boot Pin Considerations

ESP8266 NodeMCU has three GPIOs with boot-strapping roles: **GPIO0 (D3)**, **GPIO2 (D4)**, **GPIO15 (D8)**. Their state at power-on/reset determines boot mode (flash vs. UART download) and, if driven incorrectly by external hardware, can prevent the board from booting at all or send it into the wrong mode.

- **GPIO0 (D3, doorbell button — reassigned from buzzer):** Must read HIGH at boot for normal flash boot. The buzzer was originally planned here and failed during hardware bring-up — an active buzzer circuit apparently sank enough current at power-up to violate this requirement before the MCU took control. The doorbell button, wired `INPUT_PULLUP`, idles HIGH by default with no active current draw at boot, which is why it was moved here instead. **This boot-pin check must be re-run under the new assignment** — a prior passing result under the old buzzer wiring does not carry over (see `docs/08_HCF.md` row 11).
- **GPIO2 (D4, servo):** Must not be LOW at boot. Servo signal lines idle high or floating on most hobby servos before a PWM signal is applied, which is compatible — confirmed during hardware validation via sweep test, not merely assumed.
- **GPIO15 (D8, indicator LED — merged status/welcome/alarm):** Must be LOW at boot. This is the one requiring the most care — a naive active-high LED with a pull-up resistor would violate this. Wiring must ensure GPIO15 has a pull-down (many NodeMCU dev boards already include one) and the LED circuit doesn't fight it. Unaffected by the LED-merge decision — the merge changed how many logical roles this one pin serves, not its electrical boot behavior.

**Engineering rule applied (reaffirmed after hardware bring-up, not just at design time):** every boot pin is assigned only to a function that is naturally safe/idle at power-on. This rule caught a real problem — the buzzer violated it in practice, not just in theory — and the fix (moving the buzzer off D3 entirely, twice, until landing on a genuinely safe reallocation) is a demonstration of the rule working as intended, not evidence it was wrong. The rejected RX interim fix is worth noting here too: it would have "worked" electrically, but violated a different engineering principle (don't repurpose special-function pins for unrelated peripherals when you don't have to) — the boot-pin rule and the "respect special-function pins" rule pointed to the same final answer.

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
| ADC | Thermistor | Single channel, sole occupant — no sharing |
| PWM | Servo | ESP8266 PWM is software-driven at ~1kHz default; adequate for this load |
| Digital out | Buzzer (non-blocking pattern), indicator LED | Plain digital toggling, not PWM tone, for the buzzer pattern (see FDS AlarmManager) |
| I2C | OLED | Standard Wire library, 400kHz fast mode. Address 0x3C validated. |
| UART | Serial diagnostics/logging | USB-UART for development. **Deliberately reserved for actual serial use** — the buzzer was considered for RX and rejected specifically to keep this interface available for interactive debugging and future firmware workflows. |
| GPIO (digital) | IR sensor, buttons | Debounced/filtered in SensorManager |

## 8. Failure Modes & Electrical Risks

| Failure Mode | Cause | Mitigation |
|---|---|---|
| Brownout reset during door open | Servo stall current sags 3.3V rail | Separate 5V servo rail + decoupling cap (§3). Validated via sweep test. |
| Boot loop / flash-mode boot | Boot-pin (D3/D4/D8) driven incorrectly at power-up | GPIO allocation respects boot constraints (§5) — **this failure mode actually occurred** during bring-up (buzzer on D3) and was resolved by reallocation, not just anticipated on paper |
| Noisy thermistor reading | Single-sample ADC read susceptible to transient noise | Moving-average filter over N samples in SensorManager (software fix, no added hardware) |
| Ground loop induced sensor noise | Servo/LED high-current paths sharing ground trace with analog sensor reference | Star-ground topology: analog sensor ground returns separately to the supply ground point, not daisy-chained through the servo's ground path |
| Buzzer/LED EMI on IR sensor | High dI/dt switching near IR receiver | Physical separation on breadboard; keep buzzer wiring away from IR sensor leads |
| UART repurposed for peripheral use, losing debug capability | Would have resulted from the rejected RX-buzzer interim fix | Avoided by design — buzzer reallocated to D6 instead; UART kept free for actual serial use |

## 9. Wiring Philosophy

- One shared ground rail on the breadboard; all subsystem grounds return to it directly (star topology), not chained sensor-to-sensor.
- Thermistor divider wired directly to A0, no shared/switched circuitry — the simplest correct wiring for the sole analog input in the system.
- Every button gets a hardware pull-up (or the ESP8266's internal `INPUT_PULLUP`) — no floating digital inputs anywhere in the design. This same property is what made the doorbell button the right fit for D3 once the buzzer failed there.
- Servo signal wire kept short and routed away from the buzzer/LED high-current lines to minimize PWM jitter from induced noise.
- Special-function pins (UART, boot pins) are treated as reserved-unless-necessary — the rejected RX-for-buzzer fix is the concrete example of this principle being applied under real time pressure, not just stated as an ideal.

## 10. Physical Layout Recommendations

- Two-breadboard layout: one for the "sensing + logic" side (thermistor/LDR/IR/buttons, close to NodeMCU), one for the "actuation" side (servo/buzzer/LED), physically separated to keep noisy actuator wiring away from analog sensor leads.
- NodeMCU mounted centrally with short jumper runs to both breadboards.
- OLED mounted for visibility during the demo video — a practical, not just electrical, consideration.
- Only one LED to physically wire on the actuation side — see `docs/06_Wiring_Diagram.md` for the note on why a second "alarm LED" should not be added even though earlier planning documents implied one.

## 11. EMI / Noise Considerations

- Servo PWM and buzzer switching are the two most likely noise sources; both are digital (not analog), so the main risk is radiated/conducted noise into the shared 3.3V rail rather than direct signal corruption.
- Decoupling capacitor at the servo rail (§3) does double duty here — it also suppresses high-frequency noise injection into the shared ground.
- Thermistor/LDR divider values chosen to keep source impedance low enough that ambient noise pickup is a small fraction of the signal swing.

## 12. Expandability

The GPIO matrix intentionally leaves the following unused/available for v1.1+: UART (RX/TX), deliberately kept free rather than consumed by the buzzer, plus any spare digital pins after final wiring. Future zones (e.g., a second IR sensor for a different room) would require either GPIO expansion (I2C GPIO expander, e.g., MCP23017) or a second ESP8266 node — documented as the v2.0 path in the SRS roadmap rather than solved prematurely here.

## 13. Hardware Validation Plan

1. Power-on test with no sensors/actuators attached — confirm clean boot, correct boot-pin states measured with multimeter/scope. **Re-run this step under the final D3 (button)/D8 (LED) assignment** — the version that ran under D3=buzzer is no longer representative of the shipped hardware.
2. Add thermistor + LDR — verify independent, stable readings across the full expected range (ice water to warm hand; light to covered). Complete — analog test passed.
3. Add servo on isolated rail — run 50 open/close cycles, confirm no resets, measure 3.3V rail during stall with scope/multimeter. Sweep passed; sustained stall/brownout test still pending (see HCF row 6).
4. Add buzzer (D6)/LED (D8) — confirm boot-pin-safe defaults hold under full load at power-up. Pending re-test per step 1.
5. Add IR sensor + buttons — debounce verification (rapid press test, motion at boundary distance). Pull-up wiring and polarity validated; debounce interval timing itself still pending (see HCF row 10a).
6. Full integration soak test — 30 minutes continuous operation cycling through all states. Not yet run — required after the freeze is fully signed off (see HCF §5).

---

## 14. Engineering Review Log

**Pass 1 (initial matrix):** ADC conflict (thermistor+LDR both on A0) and I2C/LED pin collision (D1/D2) identified — **rejected**, sent back for revision.

**Pass 2 (pre-bring-up):** ADC conflict resolved via the LDR's digital comparator output — no multiplexing hardware. I2C collision resolved by reassigning OLED to D1/D2 and merging welcome-LED into the existing status LED rather than adding a pin. Boot-pin usage audited individually (§5) and confirmed each boot pin only drives a function that is safely idle at power-on. Servo brownout risk addressed with a dedicated power rail and decoupling capacitor. **Verdict at the time:** no unresolved conflicts remained on paper; approved for implementation.

**Pass 3 (post-bring-up, this revision):** Hardware bring-up surfaced two problems the desk review couldn't have caught — D9 doesn't exist as a real GPIO on the actual board, and the buzzer violated the boot-pin rule in practice on D3. An interim fix (buzzer → RX) was proposed, then **rejected on review** for repurposing a special-function pin without necessity. Final resolution: doorbell button ↔ buzzer swap (D3 ↔ D6), LED roles merged onto D8. Every affected section of this document (§1, §2, §4, §5, §7, §8, §9, §10, §12, §13) has been synced to match `firmware/config.h` exactly.

**Verdict:** No unresolved conflicts remain. Hardware design — including the post-bring-up reallocation — is **approved for implementation**. Full 30-minute soak test and boot-pin re-verification under the final assignment remain outstanding before the HCF can be formally signed off (see `docs/08_HCF.md` §5).
