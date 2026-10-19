---
name: sdk-changelog
description: Use when generating a customer-facing CHANGELOG between two SDK release tags. Covers how to collect commits from the main repo, changed submodules, and embedded repos (non-submodule git repos like components/wireless/wl80211/src), and how to format the result for external audiences (no commit IDs, no Jira tickets, no internal paths).
---

# SDK CHANGELOG Generator

## Overview

Produce a clean, customer-facing CHANGELOG from a git tag range. The output is an **overview for end users** — no internal identifiers, no implementation noise.

## Workflow

### 1. Collect main repo commits

```bash
git log --format="%H|%ad|%an|%s" --date=short <FROM_TAG>..HEAD
```

Filter out pure bookkeeping commits (`chore: record submodules`) — they carry no user-visible information.

### 2. Find changed submodules

```bash
# hashes at old tag
git ls-tree <FROM_TAG> --full-tree -r | grep "^160000" > /tmp/sub_old.txt
# hashes at HEAD
git ls-tree HEAD --full-tree -r | grep "^160000" > /tmp/sub_new.txt
```

Diff the two files to find submodules whose commit pointer changed.

### 3. Collect submodule commits

For each changed submodule (if checked out locally):

```bash
git -C <submodule_path> log --format="%H|%ad|%an|%s" --date=short <OLD_HASH>..<NEW_HASH>
```

### 4. Collect embedded repo commits

Some directories are standalone git repos but **not** tracked as git submodules (e.g., Google 
repo-managed projects with symlinked `.git` worktrees). These won't appear in `git ls-tree` 
and must be handled separately.

**Known embedded repos in this SDK:**
| Path | Description |
|---|---|
| `components/wireless/wl80211/src` | wl80211 WiFi driver core source |

Since the parent repo doesn't record the embedded repo's commit, use the `<FROM_TAG>` date to
find the corresponding old commit:

```bash
FROM_DATE=$(git log -1 --format=%ad --date=iso-strict <FROM_TAG>)

# For each embedded repo:
EMB_REPO=components/wireless/wl80211/src
OLD_HASH=$(git -C "$EMB_REPO" log -1 --before="$FROM_DATE" --format=%H)
NEW_HASH=$(git -C "$EMB_REPO" rev-parse HEAD)

if [ -n "$OLD_HASH" ] && [ "$OLD_HASH" != "$NEW_HASH" ]; then
  git -C "$EMB_REPO" log --format="%H|%ad|%an|%s" --date=short $OLD_HASH..$NEW_HASH
fi
```

### 5. Merge and categorize

Combine all commits (main repo + submodules + embedded repos), deduplicate by subject, then group into:

| Category | Commit prefixes |
|---|---|
| **New Features** | `feat:`, `feat(*):`  |
| **Bug Fixes** | `fix:`, `fix(*):` |
| **Improvements** | `chore:`, `refactor:`, perf/size wins |
| **Documentation** | `docs:` — omit unless relevant to users |

---

## Output Template

```markdown
## [VERSION or Unreleased] — since vX.Y.Z (DATE_FROM → DATE_TO)

### New Features

- **[Chip/Component]**
  - One-line user-visible description
  - ...

- **[Another area]**
  - ...

### Bug Fixes

- **[Area]**
  - Fixed [symptom] in [context]
  - ...

### Improvements

- Reduced code size of ...
- Refactored ... for better maintainability
```

---

## Rules for Customer-Facing Output

| Remove | Keep |
|---|---|
| Commit hashes | Chip/platform names (BL616, BL618DG…) |
| Jira / internal ticket IDs | Feature names and affected peripherals |
| Submodule/embedded repo paths | User-visible behavior changes |
| Author names | Security hardening (summarized) |
| `chore: record submodules` entries | Size/performance improvements |
| Internal code symbols | |

- Group by **functional area**, not by submodule or file
- Use plain language; avoid C macro names and file paths
- One bullet = one user-visible change (merge duplicates across submodules)
- Omit doc fixes unless they correct wrong behavior described in docs

---

## Quick Example

Given commits like:
```
feat: add timeout interrupt for 616CL
fix(lp): fix miss lp restore keyram for sec 256   # also in macsw submodule
fix: harden DHCP packet bounds checks             # in wlan/linux_driver submodule
```

Output:
```markdown
### New Features
- **BL616CL**: Added timeout interrupt support

### Bug Fixes
- **Low Power**: Fixed missing LP keyram restore for SEC 256 configuration
- **WiFi**: Hardened DHCP packet bounds checks
```

Note: the LP fix appears in both main repo and macsw submodule — output it **once**.
