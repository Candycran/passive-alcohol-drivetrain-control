# Day 06 GitHub Commits

## Commit 1 — dynamics model

After adding the MATLAB script and Simulink documentation:

```bash
git add matlab simulink docs/day06_design.md docs/day06_manual_simulink_build.md docs/day06_verification.md docs/day06_codex_master_prompt.md

git commit -m "feat: add vehicle dynamics and speed feedback model"

git push
```

## Commit 2 — simulation evidence

After running MATLAB/Simulink and saving plots/screenshots:

```bash
git add results data

git commit -m "test: verify controlled stop with vehicle speed feedback"

git push
```
