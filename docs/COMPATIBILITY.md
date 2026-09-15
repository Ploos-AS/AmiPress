# AmiPress Compatibility

## Target families

| Target | Baseline intent |
|---|---|
| 68000 | First-class baseline |
| 68020 | First-class |
| 68040 | First-class |
| 68060 | First-class |

## Operating-system targets

| OS level | Goal |
|---|---|
| Kickstart 1.3 / AmigaOS 1.3 | Core and printer integration where APIs permit |
| AmigaOS 2.x | Full classic baseline |
| AmigaOS 3.1 | Full classic baseline |
| AmigaOS 3.2 | Full classic baseline |

Newer UI components are optional and must not become dependencies of the core.

## Compatibility principle

AmiPress should degrade gracefully. If a target lacks an API or colour depth
needed for a particular feature, that feature should be disabled or reduced
rather than making the entire publishing operation fail.
