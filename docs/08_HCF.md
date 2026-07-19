# AegisHome — Hardware Configuration Freeze (HCF)
**Status:** Active — pin reassignments validated and now synced across all planning documents. Soak test and full sign-off still outstanding.

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
| 1 | OLED I2C address | 0x3C or 0x3D (module-dependent) | ✅ 0x3C | I2C scanner sketch run against the actual module | 2026-07-12 |
| 2 | Buzzer pin | D3 (original) → RX (rejected interim fix) | ✅ **D6 (v1 freeze)** — RX rejected on review: reserves UART for actual serial use rather than repurposing it for an unrelated peripheral | Full GPIO reallocation: doorbell button moved to D3 (INPUT_PULLUP idles HIGH, satisfies GPIO0 boot requirement), freeing D6 for the buzzer | 2026-07-12 |
| 2a | Status/Welcome/Alarm LED pin | D9 (originally planned) | ✅ D8 — merged into a single LED, pattern-differentiated (slow blink=welcome, fast strobe=alarm) | Board inspection confirmed no D9 silkscreen; merge closes the pin-count gap | 2026-07-12 |
| 3 | LDR comparator logic polarity | Unknown — LOW=light or HIGH=light depends on module wiring | ✅ Verified (LOW = dark) | Cover/uncover LDR, read digital pin state directly via Serial | 2026-07-12 |
| 4 | IR motion sensor output logic | Unknown — active-HIGH vs active-LOW varies by module | ✅ Verified | Trigger motion, observed raw digital level via Serial | 2026-07-12 |
| 5 | IR motion sensor retrigger/hold time | Assumed instantaneous | ⬜ pending | Time between motion stopping and pin returning to idle — affects debounce interval tuning | |
| 6 | Servo power source | Planned: dedicated 5V/VIN rail, not NodeMCU 3.3V regulator | ✅ Servo powered from VIN/5V rail with common ground | Door opened/closed repeatedly without resets or instability. (Earlier prototype briefly powered the servo from the 3.3V rail during bring-up — see row 6a below — before correcting to this design.) | 2026-07-19 |
| 6a | Servo power source — bring-up finding | N/A (observation, not a planned item) | ⚠️ Initial prototype powered the servo from the NodeMCU 3.3V rail. Under load, the servo exhibited unreliable behavior. Reverted to the intended VIN/5V rail + common ground design (row 6), which restored stable operation. | Repeated door open/close cycles with the servo powered from the NodeMCU 3.3V rail showed intermittent instability. Repeating the same test with the servo powered from the VIN/5V rail produced stable operation. No multimeter/scope measurement of the actual voltage sag was taken — this is a before/after behavioral observation, not an instrumented one | 2026-07-19 |
| 7 | Servo actual angle range | Planned: 0°–90° | ✅ Sweep passed | Physical sweep test — library angle matches real-world rotation, no binding | 2026-07-12 |
| 8 | Thermistor divider values / plausible range | Planned: standard NTC + fixed resistor divider | ✅ Analog test passed | Read raw ADC across test conditions — monotonic, plausible range confirmed | 2026-07-12 |
| 9 | Thermistor open-circuit / disconnected reading | Unknown | ⬜ pending | Physically disconnect thermistor, record raw ADC value — becomes the E01 fault-detection bound | |
| 10 | Push button pull-up wiring | INPUT_PULLUP (config.h) | ✅ Verified | Confirmed idle-HIGH, press-LOW behavior on both buttons | 2026-07-12 |
| 10a | Push button debounce interval | Planned: 40ms (config.h) | ⬜ pending | Rapid-press test — confirm no double-registers, no missed presses at 40ms | |
| 11 | Boot pin states at power-up (D3/D4/D8) | Planned: D3 HIGH, D4 not-LOW, D8 LOW | ⬜ pending — **re-test required**, D3's function changed in this freeze (doorbell button, not buzzer) | Multimeter reading at each boot pin immediately at power-on, with full circuit attached | |
| 12 | Power supply headroom | Planned: 5V/2A recommended | ⬜ pending | Measure actual current draw under worst-case load | |
| 13 | Common ground continuity | Planned: star-tied, single point | ⬜ pending | Continuity check between every subsystem's ground and the single reference point | |

---

## 4. Downstream Update Checklist

When any row above is marked ⚠️ (validated ≠ planned), the following files must be updated to match reality, and checked off here:

- [x] `firmware/config.h` — **v1 hardware freeze applied** 2026-07-12: buzzer→D6, doorbell button→D3, LED merge→D8. Supersedes the interim RX-based fix.
- [x] `docs/05_Pin_Allocation_Matrix.md` — **synced** — matches `firmware/config.h` exactly, pin for pin, including the rejected-RX and D3-boot-failure history in the Conflict Resolution Summary.
- [x] `docs/06_Wiring_Diagram.md` — **synced** — pin numbers updated throughout; wiring philosophy (star ground, servo decoupling, two-board separation, assembly order) unchanged, only pin numbers moved. Added an explicit note that only one LED needs to be physically wired now, not two.
- [ ] Hardware diagrams (SVG/image assets) — still show original D3/D9 plan, needs sync
- [x] `docs/02_HDS.md` — **synced** — §1, §2, §4, §5, §7, §8, §9, §10, §12, §13 all updated; §14 Engineering Review Log extended with a Pass 3 entry documenting the post-bring-up reallocation and the rejected RX fix, so the document's own history matches what actually happened, not just the final state.

A discrepancy that only gets fixed in `config.h` but not in the documents is technical debt hiding in plain sight — the whole point of the HCF is that the documents and the code never drift apart silently. Three of the four downstream documents are now current as of this sync pass; only the SVG/image hardware diagrams remain outstanding.

## 5. Freeze Sign-Off

The hardware configuration is considered **frozen** for v1 implementation only when:

1. Every row in §3 is either ✅ or ⚠️-with-resolution — none left ⬜ pending. **Not yet met** — rows 5, 9, 10a, 11, 12, 13 remain pending. (Row 6 is now resolved as of this update — see row 6/6a; row 12, power supply headroom, is deliberately still pending since current draw hasn't actually been measured.)
2. Every checkbox in §4 triggered by a ⚠️ row is checked. **Nearly met** — only the SVG/image hardware diagrams remain unsynced.
3. A full 30-minute soak test (per SRS Acceptance Criteria) has been run **after** the freeze, not before — the freeze should reflect the hardware the soak test actually ran on. **Not yet run.**

Sign-off date: _______________

Once frozen, any further hardware change (different sensor module, different pin, different power source) is a deliberate v1.1+ revision — not a silent edit to this document. Re-open the HCF explicitly, log the change under a "Post-Freeze Amendments" section (add one here if that day comes), and re-run the affected validation tests.
