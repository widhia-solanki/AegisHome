# AegisHome — Wiring Diagram (Breadboard-Ready)

> **Sync note:** this revision replaces pin assignments from the original planning-stage diagram (buzzer on D3, status/welcome LED on D9/spare) with the v1 Hardware Configuration Freeze actually validated on the board — see `docs/08_HCF.md` and `docs/05_Pin_Allocation_Matrix.md`. Wiring philosophy (star grounding, servo decoupling, two-board separation, assembly order) is unchanged — only the specific pin numbers below moved.

Two-breadboard layout per HDS §10: **Input Subsystem (Sensor Interface Board)** kept physically separate from **Output Subsystem (Actuator Interface Board)** to isolate analog sensor leads from high-current switching noise (servo/buzzer/LED).

## Power Rails

```
USB 5V IN
   │
   ├──> NodeMCU VIN/5V pin ──> onboard regulator ──> 3.3V rail (logic + sensors)
   │
   └──> DIRECT to Output Subsystem 5V rail (servo power) — NOT through NodeMCU regulator
              │
              └── 100–470µF electrolytic cap across 5V/GND, close to servo

Common GND: NodeMCU GND, Input Subsystem GND rail, Output Subsystem GND rail — all star-tied
to ONE common ground point. Do not daisy-chain grounds board-to-board.
```

## Input Subsystem — Sensor Interface Board

```
NodeMCU                         Input Subsystem (breadboard)
--------                        --------------------
A0  ────────────────────────────  Thermistor divider midpoint
3.3V ───────────────────────────  Thermistor divider top
GND ────────────────────────────  Thermistor divider bottom
                                   (fixed resistor + thermistor in series,
                                    standard voltage-divider wiring)

D0  ────────────────────────────  LDR module "DO" (digital out) pin
3.3V ───────────────────────────  LDR module VCC
GND ────────────────────────────  LDR module GND
                                   (LDR module's onboard potentiometer sets
                                    the light/dark threshold — no firmware
                                    calibration needed; polarity validated
                                    LOW = dark)

D1  ────────────────────────────  OLED SCL
D2  ────────────────────────────  OLED SDA
3.3V ───────────────────────────  OLED VCC
GND ────────────────────────────  OLED GND
                                   (address 0x3C validated via I2C scanner)

D3  ────────────────────────────  Doorbell push button (one leg)
GND ────────────────────────────  Doorbell push button (other leg)
                                   (INPUT_PULLUP in firmware — no external
                                    resistor required)
                                   ⚠ D3 = GPIO0, a boot-strapping pin. This
                                     button is DELIBERATELY placed here: its
                                     pull-up idles HIGH, which satisfies
                                     GPIO0's "must be HIGH at boot"
                                     requirement. The buzzer used to be wired
                                     here and failed boot — do not move the
                                     buzzer back to this pin.

D5  ────────────────────────────  IR motion sensor OUT
3.3V/5V (per module spec) ──────  IR motion sensor VCC
GND ────────────────────────────  IR motion sensor GND
                                   (polarity validated)

D7  ────────────────────────────  Security arm push button (one leg)
GND ────────────────────────────  Security arm push button (other leg)
                                   (INPUT_PULLUP in firmware; pull-up wiring
                                    validated)
```

## Output Subsystem — Actuator Interface Board

```
NodeMCU                         Output Subsystem (breadboard)
--------                        --------------------
D4  ────────────────────────────  Servo signal (orange/yellow wire)
5V (direct from USB, NOT
    NodeMCU 3.3V rail) ─────────  Servo VCC (red wire)
GND (common) ───────────────────  Servo GND (brown/black wire)
                                   + 100–470µF cap across servo 5V/GND,
                                     placed physically at the servo
                                   (sweep-tested and validated)

D6  ────────────────────────────  Buzzer positive lead
GND ────────────────────────────  Buzzer negative lead
                                   ⚠ D6 is a plain GPIO with no boot-time
                                     role — this pin was chosen specifically
                                     BECAUSE it carries no boot constraint,
                                     after two other locations were tried
                                     and rejected (see Pin Allocation Matrix
                                     Conflict Resolution Summary): D3 failed
                                     boot, and RX (GPIO3) was rejected on
                                     review for repurposing UART.

D8  ────────────────────────────  Indicator LED anode (via 220Ω resistor)
GND ────────────────────────────  Indicator LED cathode
                                   ⚠ Verify GPIO15/D8 reads LOW at boot
                                     BEFORE connecting this LED (see Pin
                                     Allocation Matrix, Pre-Power-On Checklist)
                                   This is ONE physical LED serving three
                                   roles (status / welcome / alarm),
                                   pattern-differentiated in firmware —
                                   there is no separate "alarm LED" or
                                   "welcome LED" to wire.
```

## Physical Layout Notes

- Route Output Subsystem wiring (servo, buzzer, LED) away from the Input Subsystem's analog thermistor leads — high dI/dt switching on the Output Subsystem is the primary EMI source (HDS §11).
- Keep the servo's decoupling capacitor leads short; place it as close to the servo's own power pins as physically possible on the breadboard, not back near the NodeMCU.
- Mount the OLED where it's visible on camera for the demo video — a practical requirement, not just an electrical one.
- Label both breadboards ("A — SENSE", "B — ACT") with tape before wiring — this alone prevents a large fraction of last-minute mis-wires during a timed capstone build.
- **Only one LED to wire on the Output Subsystem board now**, not two — the earlier plan's separate alarm LED and status/welcome LED were consolidated into a single physical LED during the hardware freeze. Wiring a second LED "just in case" would leave a floating, unused GPIO expectation that doesn't match `firmware/config.h`.

## Assembly Order (matches HDS §13 Hardware Validation Plan)

1. Power NodeMCU alone, no peripherals — confirm clean boot.
2. Wire Input Subsystem sensing components one at a time, verifying each with Serial output before adding the next. Wire the doorbell button (D3) early in this pass specifically so its boot-pin-safe HIGH-idle behavior can be confirmed before anything else shares the board.
3. Wire Output Subsystem servo on its isolated rail — run open/close cycles, confirm no resets.
4. Add buzzer (D6) + indicator LED (D8) — verify boot-pin-safe states hold under load (Pre-Power-On Checklist). This step no longer touches D3; the boot-pin risk that used to live here (buzzer on D3) has moved to step 2's button verification instead.
5. Full integration — both boards connected, run the 30-minute soak test.
