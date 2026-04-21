# Monument / SNES Text Audit Prompt

Repository: `C:\Users\Admin\Documents\snes-homebrew\snes-Celeste`

Current rollback baseline:
- `HEAD` is commit `39ea890`
- This commit intentionally reverts the last explicit monument char background draw attempt
- Start from this exact source state and do not assume any later monument experiment was correct

Task:
Audit the current SNES monument text rendering path and fix the remaining "random dots / specks in text tiles" issue without reintroducing crashes or layering more renderer-core churn.

Required working style:
1. Move slowly and iteratively.
2. Prefer proof over intuition.
3. Do not stack multiple speculative fixes in one step.
4. After every code change, rebuild and rerun a minimal stability check before making another change.
5. If a change affects shared text rendering core behavior, justify why that blast radius is necessary. Default to monument-local fixes.
6. If a change causes crash/regression, revert it immediately and return to the last known-good commit before trying another approach.

Primary goal:
- Remove the random blue/black dots that appear inside or around monument text glyphs on SNES.

Non-goals unless strictly required:
- Do not redesign the font.
- Do not retune monument placement/layout.
- Do not change room loading, player init, or unrelated object rendering.
- Do not rewrite the whole BG1 text system unless the audit proves that is the root cause.

Known context:
- The codebase has become brittle around the monument/text path.
- Recent renderer-core mutations have repeatedly caused crashes.
- Shared text-core edits are high risk.
- Monument-local call-site changes are preferred over global renderer changes.

Current verified baseline facts:
- `make COMPILER=llvm-mos` passes on `39ea890`
- `scripts/run_mesen_main_smoke.ps1 -Compiler llvm-mos -TimeoutSeconds 120` passes on `39ea890`
- The remaining visible issue is random dots/specks in monument text during runtime

Likely code areas to inspect first:
- `src/mainBankZero.c`
  - `monumentDrawCharHelper`
  - `monumentClearHelper`
  - `monumentUpdate`
- `src/port/snes.c`
  - `port_drawTextPico8N`
  - `port_drawTextN`
  - `port_drawTextWithColorsLen`
  - `bg1TextSetPixelColorNoPublish`
  - `bg1TextFillRectNoPublish`
  - `bg1TextHideRect`
  - `bg1TextBuildDmaSlot`
  - map/slot dirty publication logic

Suspected failure class:
- Partial background coverage or transparency leakage in the SNES BG1 text cell pipeline
- Specifically, monument text may be using a path where the glyph is drawn but some covered pixels/cells remain transparent, leaking room/background pixels through as dots

Required audit sequence:
1. Reproduce from `39ea890` only.
2. Confirm the exact current monument draw path on SNES, including:
   - which draw function is called
   - what background width is actually filled
   - what cells/slots become dirty
   - whether the cover mask fully covers the character cell or only the glyph box
3. Compare the current monument path to the older stable SNES text behavior, but do not blindly copy old experimental commits.
4. Prefer the smallest monument-local fix that makes the affected drawn area fully deterministic.
5. Rebuild and rerun smoke after every source change.
6. If possible, add a monument-specific verification path only after the harness itself is trustworthy.

Hard constraints:
- Use `apply_patch` for edits.
- Do not use `git reset --hard`.
- Do not revert unrelated user changes.
- Do not commit debug garbage, temporary probes, or unrelated build artifacts.

Acceptance bar:
- The monument no longer shows random dot/speck leakage in runtime testing.
- The change does not crash during monument interaction.
- `make COMPILER=llvm-mos` passes.
- `scripts/run_mesen_main_smoke.ps1 -Compiler llvm-mos -TimeoutSeconds 120` passes.

Suggested reporting format for each iteration:
- Hypothesis:
- Exact file/function changed:
- Why this is lower-risk than prior attempts:
- Build result:
- Smoke result:
- Runtime monument result:
- If failed, exact rollback point:
