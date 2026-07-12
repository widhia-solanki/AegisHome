# AegisHome — GitHub Repository Plan

This is a deliverable in its own right: recruiters and reviewers typically open the README before opening any code file. The repo structure below is designed to be skimmable in under a minute and to demonstrate engineering process, not just a working demo.

## Repository Structure

```
aegishome/
├── README.md                 # entry point — see template below
├── LICENSE                   # MIT recommended for a portfolio project
├── docs/
│   ├── SRS.md
│   ├── HDS.md
│   ├── FDS.md
│   ├── pin_allocation_matrix.md
│   ├── decision_log.md       # extracted/expanded from FDS §19a
│   └── testing_report.md     # extracted/expanded from FDS §14 test log
├── hardware/
│   ├── block_diagram.png
│   ├── wiring_diagram.png
│   └── photos/
│       ├── breadboard_build.jpg
│       ├── final_assembly.jpg
│       └── demo_screenshots/
├── firmware/
│   └── (module structure per FDS §3)
└── media/
    └── demo_video_link.md    # link to the LinkedIn/Drive demo video
```

## README Template

```markdown
# AegisHome — Modular Embedded Smart Home Controller

> AegisHome was designed as a modular embedded smart home controller with an
> emphasis on reliability, deterministic behavior, and maintainable firmware
> architecture. Rather than maximizing the number of peripherals, the project
> focuses on engineering practices such as finite state machines, non-blocking
> scheduling, structured diagnostics, and subsystem isolation.

## Demo
[Link to demo video]

## Features
- Non-blocking, event-driven firmware (no `delay()`-driven logic)
- Finite state machine: NORMAL / SECURITY_ARMED / ALARM
- Graceful degradation on sensor fault (see docs/FDS.md §11d)
- Structured error codes (E01–E05) surfaced on-device, not buried in serial logs
- Boot-time hardware self-check with visible diagnostic sequence

## Architecture
[Embed block diagram image]
Full design rationale: see `docs/SRS.md`, `docs/HDS.md`, `docs/FDS.md`.

## Pin Map
[Embed or link docs/pin_allocation_matrix.md]

## Folder Structure
[Repo tree, as above]

## Photos
[2–4 build photos]

## Testing
[Link to docs/testing_report.md — pass/fail table]

## Future Roadmap
[Pulled from SRS §11 — v1.1 OLED polish, v1.2 Wi-Fi telemetry, v2.0 multi-zone]

## Lessons Learned
- [1–2 honest paragraphs: what was harder than expected, what you'd do
  differently — e.g. the ADC-sharing decision reversal is a good one to
  narrate here, since it shows real engineering judgment in action]

## License
MIT
```

## Why This Structure Matters

- **`docs/` separated from `firmware/`** signals the design process was distinct from — and preceded — the code, which is the opposite of how most student repos are structured (code first, docs as an afterthought if at all).
- **`decision_log.md` as its own file**, not buried in the FDS, gives a reviewer a fast way to see that alternatives were considered without reading three full specs.
- **Photos + demo link near the top of the README**, not the bottom — a recruiter skimming for 30 seconds should see evidence of a real physical build before they decide whether to keep reading.
- **Lessons Learned** is the section most student repos skip entirely, and it's disproportionately valuable — it's the one place where honest reflection (like walking back the multiplexer decision) reads as maturity rather than as an admission of failure.
