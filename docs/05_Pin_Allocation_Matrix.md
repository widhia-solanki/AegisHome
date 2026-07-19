# AegisHome — Final GPIO Allocation Matrix
**Status:** Synced to firmware v1 Hardware Configuration Freeze (docs/08_HCF.md) and `firmware/config.h`. Approved for build.

> **Sync note:** This matrix previously reflected an earlier plan (buzzer on D3, status/welcome LED on D9). Both were superseded during hardware bring-up — see `docs/08_HCF.md` rows 2/2a and `docs/07_CHANGELOG.md` v0.3–v0.5 for the full reasoning. This revision matches `firmware/config.h` exactly, pin for pin.

## Matrix

| Function | NodeMCU Pin | GPIO# | Direction | Signal Type | Boot Pin? | Notes |
|---|---|---|---|---|---|---|
| Thermistor | A0 | ADC0 | Input | Analog | No | Sole occupant of the only ADC channel — validated |
| LDR (comparator DO) | D0 | GPIO16 | Input | Digital | No | LM393 onboard comparator; polarity validated (LOW = dark) |
| OLED SCL | D1 | GPIO5 | I/O | I2C | No | Wire library default-compatible; address 0x3C validated |
| OLED SDA | D2 | GPIO4 | I/O | I2C | No | Wire library default-compatible |
| **Doorbell button** | **D3** | **GPIO0** | Input | Digital, `INPUT_PULLUP` | **Yes** — must be HIGH at boot | **Reassigned here during hardware freeze.** A push button wired `INPUT_PULLUP` idles HIGH by default, which naturally satisfies GPIO0's boot requirement — this is *why* the button (not the buzzer) belongs on this specific pin. Boot-pin state re-test still pending, see HCF row 11. |
| Servo (door) | D4 | GPIO2 | Output | PWM | **Yes** — must not be LOW at boot | Servo library only actively drives after `setup()`; sweep-tested and validated |
| IR motion sensor | D5 | GPIO14 | Input | Digital | No | Polled + debounced in SensorManager; polarity validated |
| **Buzzer** | **D6** | **GPIO12** | Output | Digital (non-blocking pattern, not PWM tone) | No | **Reassigned here during hardware freeze**, after two prior locations were rejected: D3 (original plan — failed due to GPIO0 boot-pin contention) and RX/GPIO3 (interim fix — rejected on review for reserving UART for actual serial use rather than an unrelated peripheral). D6 is a plain GPIO with no boot role, freed by moving the doorbell button to D3. |
| Security arm button | D7 | GPIO13 | Input | Digital, `INPUT_PULLUP` | No | Debounced; pull-up wiring validated |
| **Indicator LED** | **D8** | **GPIO15** | Output | Digital | **Yes** — must be LOW at boot | **Merged pin**: status, welcome, and alarm indication all share this single physical LED, pattern-differentiated (solid/slow blink = welcome/lighting, fast strobe = alarm) rather than three separate LEDs on three separate pins. Wiring must not pull this pin high passively — verify with multimeter before first power-on. |

## Conflict Resolution Summary

| Conflict (original) | Resolution |
|---|---|
| Thermistor + LDR both require the single ADC (A0) | LDR moved to digital comparator output (D0); thermistor is sole ADC user. No multiplexing hardware added. |
| OLED I2C (D1/D2) collided with LED pin assignment | LEDs reassigned off D1/D2 entirely; OLED has exclusive use of the I2C pins. |
| Welcome LED vs. dedicated pin budget | Welcome, status, and alarm LED functions all merged into one physical LED on D8, pattern-differentiated, instead of allocating separate pins for each. |
| Buzzer failed on D3 (GPIO0 boot-pin contention) | Interim fix moved buzzer to RX (GPIO3) — **rejected on review**: repurposing UART for an unrelated peripheral trades away future flexibility (interactive debugging, external serial devices, alternate firmware update workflows) for a one-off pin-count fix. **Final fix:** doorbell button moved to D3 instead (a pulled-up button idles HIGH, satisfying GPIO0's boot requirement, which the buzzer could not do), freeing D6 — a plain GPIO with no boot role — for the buzzer. |
| Boot pins (D3/D4/D8) risk unreliable boot if misused | Each boot pin is assigned only to a function that is naturally safe/idle at power-on: D3 doorbell button (pull-up idles HIGH), D4 servo (idles pre-`setup()`), D8 indicator LED (wiring pulled low). Boot-pin power-up states must be **re-verified** under this final assignment — a prior pass under the old D3=buzzer assignment does not carry over now that D3's function has changed (see HCF row 11). |

## Unused / Reserved Pins

| Pin | Reserved For |
|---|---|
| TX/RX (UART) | Reserved for actual serial use — debugging, alternate firmware workflows. Deliberately NOT repurposed for the buzzer (see Conflict Resolution Summary above). |
| SPI pins (if unused by other peripherals) | v2.0 expansion path (I2C GPIO expander, additional zone) |

## Pre-Power-On Checklist (derived from this matrix)

1. Confirm GPIO0 (D3) reads HIGH with the doorbell button disconnected, then again with it connected via `INPUT_PULLUP` — must remain HIGH. **(Re-test required — this pin's function changed from buzzer to button in the final freeze; see HCF row 11.)**
2. Confirm GPIO15 (D8) reads LOW with the indicator LED disconnected, then again with it connected — must remain LOW.
3. Confirm GPIO2 (D4) is not held LOW by the servo signal line at rest.
4. Confirm no pin is double-wired (cross-check this matrix against the physical breadboard before applying power).
5. Confirm the buzzer on D6 draws no current at boot — D6 has no boot-pin role, so this is a sanity check rather than a hard requirement, but worth a quick multimeter pass given the buzzer's history on this project.
