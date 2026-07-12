# AegisHome — Wiring Diagram (Breadboard-Ready)

Two-breadboard layout per HDS §10: **Input Subsystem (Sensor Interface Board)** kept physically separate from **Output Subsystem (Actuator Interface Board)** to isolate analog sensor leads from high-current switching noise (servo/buzzer/LEDs).

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
                                    calibration needed)

D5  ────────────────────────────  IR motion sensor OUT
3.3V/5V (per module spec) ──────  IR motion sensor VCC
GND ────────────────────────────  IR motion sensor GND

D6  ────────────────────────────  Doorbell push button (one leg)
GND ────────────────────────────  Doorbell push button (other leg)
                                   (INPUT_PULLUP in firmware — no external
                                    resistor required)

D7  ────────────────────────────  Security arm push button (one leg)
GND ────────────────────────────  Security arm push button (other leg)
                                   (INPUT_PULLUP in firmware)

D1  ────────────────────────────  OLED SCL
D2  ────────────────────────────  OLED SDA
3.3V ───────────────────────────  OLED VCC
GND ────────────────────────────  OLED GND
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

D9 or spare digital pin ────────  Status/Welcome LED anode (via 220Ω resistor)
GND ────────────────────────────  Status/Welcome LED cathode

D8  ────────────────────────────  Alarm (red) LED anode (via 220Ω resistor)
GND ────────────────────────────  Alarm (red) LED cathode
                                   ⚠ Verify GPIO15/D8 reads LOW at boot
                                     BEFORE connecting this LED (see Pin
                                     Allocation Matrix, Pre-Power-On Checklist)

D3  ────────────────────────────  Buzzer positive lead
GND ────────────────────────────  Buzzer negative lead
                                   ⚠ Buzzer must not sink current until
                                     setup() completes (GPIO0/D3 boot rule)
```

## Physical Layout Notes

- Route Output Subsystem wiring (servo, buzzer, LEDs) away from the Input Subsystem's analog thermistor leads — high dI/dt switching on the Output Subsystem is the primary EMI source (HDS §11).
- Keep the servo's decoupling capacitor leads short; place it as close to the servo's own power pins as physically possible on the breadboard, not back near the NodeMCU.
- Mount the OLED where it's visible on camera for the demo video — a practical requirement, not just an electrical one.
- Label both breadboards ("A — SENSE", "B — ACT") with tape before wiring — this alone prevents a large fraction of last-minute mis-wires during a timed capstone build.

## Assembly Order (matches HDS §13 Hardware Validation Plan)

1. Power NodeMCU alone, no peripherals — confirm clean boot.
2. Wire Input Subsystem sensing components one at a time, verifying each with Serial output before adding the next.
3. Wire Output Subsystem servo on its isolated rail — run open/close cycles, confirm no resets.
4. Add buzzer + LEDs — verify boot-pin-safe states hold under load (Pre-Power-On Checklist).
5. Full integration — both boards connected, run the 30-minute soak test.
