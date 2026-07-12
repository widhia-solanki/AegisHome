# AegisHome — Hardware Configuration Freeze (HCF)
**Status:** Template — to be filled in during hardware bring-up, one row at a time, per HDS §13 Hardware Validation Plan.

---

## 1. Purpose

Every other document (SRS, HDS, FDS, Pin Allocation Matrix) describes the **planned** design. None of them describe what the physical hardware in front of you actually does — and those two things are never guaranteed to match. Module silkscreens lie, comparator modules ship with inverted logic depending on the batch, I2C addresses vary between OLED manufacturers, and a servo that "should" run fine off the NodeMCU's 3.3V rail may not.

The HCF exists to close that gap. It is the **only** document `config.h` should be written against once bring-up starts — not the HDS, not the Pin Allocation Matrix. Those documents describe intent; this one describes verified fact. When intent and fact disagree, the HCF wins, and the disagreement itself gets logged here rather than silently overwritten.

This is also, frankly, the difference between a project that *looks* like it was engineered and one that actually was: anyone can copy a wiring diagram. Verifying and recording what your specific components actually do is the part that can't be faked or skipped.

## 2. How to Use This Document

- Fill in the **Validated** column only after physically testing the item — never from assumption, datasheet-guessing, or "it should be this."
- If validated behavior **matches** the plan: mark ✅ and note the confirming test.
- If validated behavior **differs** from the plan: mark ⚠️, record the actual value, and note the downstream files that need updating (`config.h`, wiring diagram, etc.) — then actually update them, and check them off in §4.
- Leave rows blank/pending until tested. Do not pre-fill "expected to match" — an untested row is a risk, not a fact, and should look like one.
- Timestamp each row when validated (even just a session date) so the freeze has a build history, not just a final state.

---

## 3. Hardware Configuration Table

| # | Item | Planned (per docs) | Validated | Test Performed | Date |
|---|---|---|---|---|---|
| 1 | OLED I2C address | 0x3C or 0x3D (module-dependent) | ⬜ pending — `config.h` currently assumes 0x3C, confirm with I2C scanner | I2C scanner sketch run against the actual module | |
| 2 | Buzzer pin | D3 (Pin Allocation Matrix) | ⚠️ Moved to RX (GPIO3) — D3 caused issues during testing | Root cause not yet isolated (likely GPIO0 boot-pin contention); RX chosen since Serial receive is unused | 2026-07-12 |
| 2a | Status/Welcome/Alarm LED pin | D9 (originally planned) | ⚠️ D9 does not exist as a real Dx GPIO on standard NodeMCU — merged into a single LED on D8, pattern-differentiated (slow blink=welcome, fast strobe=alarm) instead of two separate pins | Board inspection — no D9 silkscreen present | 2026-07-12 |
| 3 | LDR comparator logic polarity | Unknown — LOW=light or HIGH=light depends on module wiring (some invert around the LM393) | ⬜ pending | Cover/uncover LDR, read digital pin state directly via Serial | |
| 4 | IR motion sensor output logic | Unknown — active-HIGH vs active-LOW varies by module | ⬜ pending | Trigger motion, observe raw digital level and hold time via Serial | |
| 5 | IR motion sensor retrigger/hold time | Assumed instantaneous | ⬜ pending | Time between motion stopping and pin returning to idle — affects debounce interval tuning | |
| 6 | Servo power source | Planned: dedicated 5V/VIN rail, not NodeMCU 3.3V regulator | ⬜ pending | Run 50 open/close cycles on VIN, scope/multimeter the 3.3V logic rail during stall — confirm no sag/reset | |
| 7 | Servo actual angle range | Planned: 0°–90° | ⬜ pending | Physical sweep test — confirm library angle matches real-world rotation, no mechanical binding | |
| 8 | Thermistor divider values / plausible range | Planned: standard NTC + fixed resistor divider | ⬜ pending | Read raw ADC at known temperatures (ice water, ambient, warm hand) — confirm monotonic, plausible range for fault-detection bounds (FDS §11) | |
| 9 | Thermistor open-circuit / disconnected reading | Unknown | ⬜ pending | Physically disconnect thermistor, record raw ADC value — this becomes the E01 fault-detection bound | |
| 10 | Push button debounce interval | Planned: 40ms (config.h) | ⬜ pending | Rapid-press test — confirm no double-registers, no missed presses at 40ms | |
| 11 | Boot pin states at power-up (D3/D4/D8) | Planned: D3 HIGH, D4 not-LOW, D8 LOW | ⬜ pending | Multimeter reading at each boot pin immediately at power-on, with full circuit attached | |
| 12 | Power supply headroom | Planned: 5V/2A recommended | ⬜ pending | Measure actual current draw under worst-case load (servo + Wi-Fi TX if applicable) | |
| 13 | Common ground continuity | Planned: star-tied, single point | ⬜ pending | Continuity check between every subsystem's ground and the single reference point | |

---

## 4. Downstream Update Checklist

When any row above is marked ⚠️ (validated ≠ planned), the following files must be updated to match reality, and checked off here:

- [x] `firmware/config.h` — pin/threshold/address constants (updated 2026-07-12, buzzer→RX, LED merge→D8)
- [ ] `docs/05_Pin_Allocation_Matrix.md` — still shows original D3/D9 plan, needs sync
- [ ] `docs/06_Wiring_Diagram.md` — still shows original D3/D9 plan, needs sync
- [ ] Hardware diagrams (SVG/image assets) — still show original D3/D9 plan, needs sync
- [ ] `docs/02_HDS.md`, if the discrepancy reveals a design assumption that was wrong, not just a component quirk

A discrepancy that only gets fixed in `config.h` but not in the documents is technical debt hiding in plain sight — the whole point of the HCF is that the documents and the code never drift apart silently.

## 5. Freeze Sign-Off

The hardware configuration is considered **frozen** for v1 implementation only when:

1. Every row in §3 is either ✅ or ⚠️-with-resolution — none left ⬜ pending.
2. Every checkbox in §4 triggered by a ⚠️ row is checked.
3. A full 30-minute soak test (per SRS Acceptance Criteria) has been run **after** the freeze, not before — the freeze should reflect the hardware the soak test actually ran on.

Sign-off date: _______________

Once frozen, any further hardware change (different sensor module, different pin, different power source) is a deliberate v1.1+ revision — not a silent edit to this document. Re-open the HCF explicitly, log the change under a "Post-Freeze Amendments" section (add one here if that day comes), and re-run the affected validation tests.
