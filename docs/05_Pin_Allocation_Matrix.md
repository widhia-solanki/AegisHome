# AegisHome — Final GPIO Allocation Matrix
**Status:** All conflicts resolved. Approved for build.

## Matrix

| Function | NodeMCU Pin | GPIO# | Direction | Signal Type | Boot Pin? | Notes |
|---|---|---|---|---|---|---|
| Thermistor | A0 | ADC0 | Input | Analog | No | Sole occupant of the only ADC channel |
| LDR (comparator DO) | D0 | GPIO16 | Input | Digital | No | LM393 onboard comparator; no PWM/interrupt on this pin, not needed for a digital level read |
| IR motion sensor | D5 | GPIO14 | Input | Digital | No | Polled + debounced in SensorManager |
| Doorbell button | D6 | GPIO12 | Input | Digital, `INPUT_PULLUP` | No | Debounced |
| Security arm button | D7 | GPIO13 | Input | Digital, `INPUT_PULLUP` | No | Debounced |
| Servo (door) | D4 | GPIO2 | Output | PWM | **Yes** — must not be LOW at boot | Servo library only actively drives after `setup()`; verified in hardware validation |
| Status/Welcome LED | D9 (RX) *or* spare digital | GPIO3 | Output | Digital | No (see note) | If RX is repurposed, Serial RX is sacrificed — acceptable since diagnostics use TX/print-only logging (§ see FDS §12). If Serial RX is needed during bring-up, reassign to a spare pin post-I2C allocation. |
| Alarm (red) LED | D8 | GPIO15 | Output | Digital | **Yes** — must be LOW at boot | Wiring must not pull this pin high passively; verify with multimeter before first power-on |
| Buzzer | D3 | GPIO0 | Output | PWM (tone) | **Yes** — must be HIGH at boot | Firmware must not drive LOW until `setup()` completes; buzzer circuit must not sink current at power-up |
| OLED SDA | D2 | GPIO4 | I/O | I2C | No | Wire library default-compatible |
| OLED SCL | D1 | GPIO5 | I/O | I2C | No | Wire library default-compatible |

## Conflict Resolution Summary

| Conflict (original) | Resolution |
|---|---|
| Thermistor + LDR both require the single ADC (A0) | LDR moved to digital comparator output (D0); thermistor is sole ADC user. No multiplexing hardware added. |
| OLED I2C (D1/D2) collided with LED pin assignment | LEDs reassigned off D1/D2 entirely; OLED has exclusive use of the I2C pins. |
| Welcome LED vs. dedicated pin budget | Welcome LED merged into the existing status LED (same physical LED, distinct blink pattern) rather than allocating a new pin. |
| Boot pins (D3/D4/D8) risk unreliable boot if misused | Each boot pin is assigned only to a function that is naturally safe/idle at power-on (buzzer defaults silent-HIGH, servo signal idles pre-`setup()`, alarm LED wiring pulled low) — confirmed in Hardware Validation Plan (HDS §13) before first full power-up. |

## Unused / Reserved Pins

| Pin | Reserved For |
|---|---|
| D10 (RX) or equivalent spare | v1.1 expansion (e.g., second button, additional indicator) |
| SPI pins (if unused by other peripherals) | v2.0 expansion path (I2C GPIO expander, additional zone) |

## Pre-Power-On Checklist (derived from this matrix)

1. Confirm GPIO0 (D3) reads HIGH with buzzer disconnected, then again with buzzer connected — must remain HIGH.
2. Confirm GPIO15 (D8) reads LOW with alarm LED disconnected, then again with LED connected — must remain LOW.
3. Confirm GPIO2 (D4) is not held LOW by the servo signal line at rest.
4. Confirm no pin is double-wired (cross-check this matrix against the physical breadboard before applying power).
