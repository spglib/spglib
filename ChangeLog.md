<!-- Keep a Changelog guide -> https://keepachangelog.com -->

<!-- Main summary of releases. No need to include Pull Requests, these are handled by release workflow -->

# Spglib Changelog

All notable changes to this project are documented here. More details can be found in the
GitHub release pages and in the git history.

<!-- SPHINX-START -->

## \[Unreleased\]

### Fixes (magnetic space group)

- Fix to check internal primitive symmetry search to avoid SEGV

### C Interface

- Fix a potential segmentation fault in `spacegroup.c:iterative_search_hall_number`

### Python API

- Add `spglib.get_magnetic_spacegroup_type_from_symmetry` for getting a magnetic space-group type information from magnetic symmetry operations

## v2.4.0 (11 Apr. 2024)

### Main Changes

- Separated the CMake sub-projects to be buildable as standalone projects with pre-installed Spglib C library: test,
  python, fortran
- Bumped minimum CMake version to 3.20
  (Note: previously the project was not tested for the minimum CMake version)
- Migrated the example and package test to the ctest test-suite
- Properly mark all deprecated functions where possible
- Fix excessive debug/warning message prints
- Debug and warning messages are controlled by:
  - Environment variable `SPGLIB_DEBUG`: Define any value to enable printing of debug messages
  - Environment variable `SPGLIB_WARNING`: Set to `OFF` to disable warning printing
  - CMake option `SPGLIB_DEBUG`, `SPGLIB_WARNINGS`: Disable the compilation of these messages all-together
- Expanded Python distribution to more MacOS variants, including MacOS-14 (M1)

### C Interface

- Mark deprecated api with `[[deprecated]]` and pre-C23 equivalents
- Explicitly mark public API for export

### Python API

- Fixed the Python module build and installation in the pure CMake environment (without scikit-build-core)
- Use importlib to search and load the bundled spglib C library
- Support editable installs
- [\[#420\]](https://github.com/spglib/spglib/pull/420) Raise `TypeError` when a given `cell` is invalid
- [\[#420\]](https://github.com/spglib/spglib/pull/420) Add type annotation for `cell`

### CI

- [\[#425\]](https://github.com/spglib/spglib/pull/425) - Update to codecov v4
- [\[#402\]](https://github.com/spglib/spglib/pull/402) - Bump artifact actions to v4
- [\[#402\]](https://github.com/spglib/spglib/pull/402) - Build and inspect python sdist
- [\[#431\]](https://github.com/spglib/spglib/pull/431) - Test CMake versions
- [\[#459\]](https://github.com/spglib/spglib/pull/459) - Fix various CI issues
- [\[#459\]](https://github.com/spglib/spglib/pull/459) - Building for MacOS 14 (M1)

## v2.3.1 (10 Feb. 2024)

### Fortran API

- [\[#421\]](https://github.com/spglib/spglib/pull/421) - Allow importing Fortran API when library/consumer compilers differ
- [\[#421\]](https://github.com/spglib/spglib/pull/421) - Fix building Fortran API from exported `spglib_f08.F90`

### Fixes

- [\[#423\]](https://github.com/spglib/spglib/pull/423) - Update DynamicVersion to v0.4.1. Fixes CMake build when git repository does not have a tag
- [\[#426\]](https://github.com/spglib/spglib/pull/426) - Fix Windows installation path

### CI

- [\[#422\]](https://github.com/spglib/spglib/pull/422) - Update to ci-build-wheel v2.16

### Documentation

- [\[#408\]](https://github.com/spglib/spglib/pull/408) doc: Add homebrew and spack badges

## v2.3.0 (27 Jan. 2024)

### Features

- Version is now calculated dynamically from commit information
  - `version` is the `X.Y.Z` version format of Spglib
  - `version_full` is the full version formatted like `guess-next-dev` from [setuptools-scm]
  - `commit` is the commit used when building the Spglib library

### Fixes (layer group)

- [\[#378\]](https://github.com/spglib/spglib/pull/378) - fix(database): add missing unique axis choices
- [\[#379\]](https://github.com/spglib/spglib/pull/379) - fix #377 (symmetry): fix matrix pattern for layer groups

### Fixes (magnetic space group)

- [\[#382\]](https://github.com/spglib/spglib/pull/382) - Fix comparison of translation parts in MSG type identification

### CMake interface

- Exporting `Spglib_VERSION_FULL` and `Spglib_COMMIT` CMake variables

### C interface

- Added `spg_get_verison`, `spg_get_verison_full`, `spg_get_commit`

### Python interface

- [\[#376\]](https://github.com/spglib/spglib/pull/376) Dropped Python 3.7 support
- [\[#386\]](https://github.com/spglib/spglib/pull/386) - Drop ASE Atoms-style input
- Deprecating `get_version` in favor of `__version__` and `get_spg_version`
- Added `spg_get_version`, `spg_get_version_full`, `spg_get_commit` to get the version/commit of the spglib C library
  that is being used by the binding
- `__version__` now reports the version of the python interface

### Fortran interface

- [\[#396\]](https://github.com/spglib/spglib/pull/396) - feat: Reorganize Fortran interface
  - Added `spg_get_version`, `spg_get_version_full`, `spg_get_commit` to get the version/commit of the spglib C library
    that is being used by the binding
  - Added `version`, `version_full`, `commit` variables containing the version/commit of the Fortran bindings

### Documentation

- [\[#361\]](https://github.com/spglib/spglib/pull/361) - ci: Add doc-test
- [\[#387\]](https://github.com/spglib/spglib/pull/387) - Merge and migrate Python API documents into docstring
- [\[#391\]](https://github.com/spglib/spglib/pull/391) - Fix quick for autodoc2 render plugin
- [\[#401\]](https://github.com/spglib/spglib/pull/401) - fix: RTD build

### Refactoring

- [\[#271\]](https://github.com/spglib/spglib/pull/271) - Fully dynamic version from git-tag
- [\[#392\]](https://github.com/spglib/spglib/pull/392) - Replace linter and formatter with ruff and mypy

### CI

- [\[#356\]](https://github.com/spglib/spglib/pull/356) - Use downstream tmt tests
- [\[#362\]](https://github.com/spglib/spglib/pull/362) - ci: Reorganize CI
- [\[#393\]](https://github.com/spglib/spglib/pull/393) - ci: Enable testing for Fedora 38

## v2.2.0 (6 Dec. 2023)

This minor release includes update of crystallographic databases to adopt the latest editions of *International Tables for Crystallography*:

<details>
  <summary>Table of the new Hall symbols for space groups in spglib</summary>

| hall_number | spg_database (previous) | spg_database (new) |
| ----------- | ----------------------- | ------------------ |
| 40          | A -2yac                 | A -2yab            |
| 43          | C -2ybc                 | C -2yac            |
| 46          | B -2bc                  | B -2ab             |
| 49          | A -2ac                  | A -2ab             |
| 52          | C -2xbc                 | C -2xac            |
| 55          | B -2xbc                 | B -2xab            |
| 91          | -A 2yac                 | -A 2yab            |
| 94          | -C 2ybc                 | -C 2yac            |
| 97          | -B 2bc                  | -B 2ab             |
| 100         | -A 2ac                  | -A 2ab             |
| 103         | -C 2xbc                 | -C 2xac            |
| 106         | -B 2xbc                 | -B 2xab            |
| 191         | A 2 -2c                 | A 2 -2b            |
| 192         | B 2 -2c                 | B 2 -2a            |
| 193         | B -2c 2                 | B -2a 2            |
| 194         | C -2b 2                 | C -2a 2            |
| 195         | C -2b -2b               | C -2a -2a          |
| 196         | A -2c -2c               | A -2b -2b          |
| 203         | A 2 -2ac                | A 2 -2ab           |
| 204         | B 2 -2bc                | B 2 -2ab           |
| 205         | B -2bc 2                | B -2ab 2           |
| 206         | C -2bc 2                | C -2ac 2           |
| 207         | C -2bc -2bc             | C -2ac -2ac        |
| 208         | A -2ac -2ac             | A -2ab -2ab        |
| 304         | -C 2bc 2                | -C 2ac 2           |
| 305         | -C 2bc 2bc              | -C 2ac 2ac         |
| 306         | -A 2ac 2ac              | -A 2ab 2ab         |
| 307         | -A 2 2ac                | -A 2 2ab           |
| 308         | -B 2 2bc                | -B 2 2ab           |
| 309         | -B 2bc 2                | -B 2ab 2           |
| 316         | -C 2b 2                 | -C 2a 2            |
| 317         | -C 2b 2b                | -C 2a 2a           |
| 318         | -A 2c 2c                | -A 2b 2b           |
| 319         | -A 2 2c                 | -A 2 2b            |
| 320         | -B 2 2c                 | -B 2 2a            |
| 321         | -B 2c 2                 | -B 2a 2            |
| 322         | C 2 2 -1bc              | C 2 2 -1ac         |
| 323         | -C 2b 2bc               | -C 2a 2ac          |
| 324         | C 2 2 -1bc              | C 2 2 -1ac         |
| 325         | -C 2b 2c                | -C 2a 2c           |
| 326         | A 2 2 -1ac              | A 2 2 -1ab         |
| 327         | -A 2a 2c                | -A 2a 2b           |
| 328         | A 2 2 -1ac              | A 2 2 -1ab         |
| 329         | -A 2ac 2c               | -A 2ab 2b          |
| 330         | B 2 2 -1bc              | B 2 2 -1ab         |
| 331         | -B 2bc 2b               | -B 2ab 2b          |
| 332         | B 2 2 -1bc              | B 2 2 -1ab         |
| 333         | -B 2b 2bc               | -B 2b 2ab          |
| 440         | P 31 2c (0 0 1)         | P 31 2 (0 0 4)     |
| 442         | P 32 2c (0 0 -1)        | P 32 2 (0 0 2)     |
| 472         | P 61 2 (0 0 -1)         | P 61 2 (0 0 5)     |
| 474         | P 62 2c (0 0 1)         | P 62 2 (0 0 4)     |
| 475         | P 64 2c (0 0 -1)        | P 64 2 (0 0 2)     |
| 515         | F -4c 2 3               | F -4a 2 3          |
| 524         | -F 4c 2 3               | -F 4a 2 3          |
| 527         | F 4d 2 3 -1cd           | F 4d 2 3 -1ad      |
| 528         | -F 4cvw 2vw 3           | -F 4ud 2vw 3       |

</details>

<details>
  <summary>Table of the new H-M symbols for space groups in spglib</summary>

| hall_number | spg_database (previous) | spg_database (new) |
| ----------- | ----------------------- | ------------------ |
| 494         | P m 3                   | P m -3             |
| 495         | P n 3                   | P n -3             |
| 496         | P n 3                   | P n -3             |
| 497         | F m 3                   | F m -3             |
| 498         | F d 3                   | F d -3             |
| 499         | F d 3                   | F d -3             |
| 500         | I m 3                   | I m -3             |
| 501         | P a 3                   | P a -3             |
| 502         | I a 3                   | I a -3             |

</details>

<details>
  <summary>Table of the new Hall symbols for layer groups in spglib</summary>

| layer group hall number | spg_database (previous) | spg_database (new) |
| ----------------------- | ----------------------- | ------------------ |
| 62                      | c -2b -2b               | c -2a -2a          |
| 63                      | c -2b 2                 | c -2a 2            |
| 81                      | -c 2b 2                 | -c 2a 2            |

</details>

In addition, this minor release includes a range of improvements across documentation, crystallographic database fixes, magnetic space group fixes, refactoring of the C codebase, enhancements to the Fortran and Julia interfaces, and continuous integration (CI) improvements.

### Documentation

- [\[#369\]](https://github.com/spglib/spglib/pull/369) - Minor update of variable.md
- [\[#355\]](https://github.com/spglib/spglib/pull/355) - Fix example wrong results in definition.md
- [\[#343\]](https://github.com/spglib/spglib/pull/343) - Fix BNS number in API example
- [\[#257\]](https://github.com/spglib/spglib/pull/257) - Improve release documentation
- [\[#332\]](https://github.com/spglib/spglib/pull/332) - Clarify definition of primitive_lattice for MSG

### Fixes (crystallographic database)

- [\[#367\]](https://github.com/spglib/spglib/pull/367) - Fix arithmetic crystal class for -6m2P and -62mP
- [\[#360\]](https://github.com/spglib/spglib/pull/360) - Fix typo for arithmetic crystal class 6/mmmP
- [\[#317\]](https://github.com/spglib/spglib/pull/317) - fix #155 refactor(database): standardize Hall symbols

### Fixes (magnetic space group)

- [\[#371\]](https://github.com/spglib/spglib/pull/371) - Fix order of primitive basis for get_magnetic_symmetry
- [\[#349\]](https://github.com/spglib/spglib/pull/349) - Check pointer to magnetic dataset before free

### C codebase Refactoring

- [\[#347\]](https://github.com/spglib/spglib/pull/347) - Minor simplification of trim_cell
- [\[#339\]](https://github.com/spglib/spglib/pull/339) - Increase max attempts for reduced cells

### Fortran interface

- [\[#357\]](https://github.com/spglib/spglib/pull/357) - fix: Fortran pkg-config file
- [\[#342\]](https://github.com/spglib/spglib/pull/342) - Tests for noncollinear case and spg_get_symmetry_with_site_tensors of fortran interface
- [\[#337\]](https://github.com/spglib/spglib/pull/337) - Add Fortran interface and Fortran interface tests
- [\[#333\]](https://github.com/spglib/spglib/pull/333) - Add magnetic dataset support in Fortran

### Julia interface

- [\[#341\]](https://github.com/spglib/spglib/pull/341) - fix: Julia packaging issues
- [\[#340\]](https://github.com/spglib/spglib/pull/340) - Add "Julia interface" to interface.md

### CI and releasing

- [\[#358\]](https://github.com/spglib/spglib/pull/358) - fix: windows-ci

## v2.1.0 (10 Sep. 2023)

This minor release includes a lot of improvements in build system and CI.

### Documentation and examples

- [\[#242\]](https://github.com/spglib/spglib/pull/242) - Update examples in C and Fortran
- [\[#245\]](https://github.com/spglib/spglib/pull/245) - Readthedocs
- [\[#246\]](https://github.com/spglib/spglib/pull/246) - Clean up and documentation fix
- [\[#253\]](https://github.com/spglib/spglib/pull/253) - Update documentation link
- [\[#263\]](https://github.com/spglib/spglib/pull/263) - Redirect GitHub pages to Read the Docs
- [\[#265\]](https://github.com/spglib/spglib/pull/265) - Clarify non-collinear magmoms in Python interface
- [\[#283\]](https://github.com/spglib/spglib/pull/283) - Document behavior for type-II magnetic crystal structure
- [\[#322\]](https://github.com/spglib/spglib/pull/322) - Add citation info

### Fixes (layer group)

- [\[#199\]](https://github.com/spglib/spglib/pull/199) - Fix a bug in layer groups.
- [\[#201\]](https://github.com/spglib/spglib/pull/201) - Add layer group python interface
- [\[#209\]](https://github.com/spglib/spglib/pull/209) - fix(layergroup): fix unique axis of monocli system
- [\[#288\]](https://github.com/spglib/spglib/pull/288) - Initial refactor for layer group implementation

### Fixes (magnetic space group)

- [\[#267\]](https://github.com/spglib/spglib/pull/267) - Validate type of MSG

### C codebase Refactoring

- [\[#223\]](https://github.com/spglib/spglib/pull/223) - Rename periodic_niggli_reduce to niggli_reduce_periodic
- [\[#258\]](https://github.com/spglib/spglib/pull/258) - Change `SPGCONST` to `const`
- [\[#278\]](https://github.com/spglib/spglib/pull/278) - Revive compiler warnings
- [\[#281\]](https://github.com/spglib/spglib/pull/281) - Fix message and remove redundant lines
- [\[#284\]](https://github.com/spglib/spglib/pull/284) - Ensure thread-safety
- [\[#293\]](https://github.com/spglib/spglib/pull/293) - Refactor debug-warning rules

### Fortran interface

- [\[#216\]](https://github.com/spglib/spglib/pull/216) - Fortran spg get symmetry with site tensors
- [\[#224\]](https://github.com/spglib/spglib/pull/224) - Change installation location of spglib_f08.mod
- [\[#226\]](https://github.com/spglib/spglib/pull/226) - Add fortran tests and clean cmake
- [\[#227\]](https://github.com/spglib/spglib/pull/227) - Quick fix: Fix fortran cmake target
- [\[#230\]](https://github.com/spglib/spglib/pull/230) - Add fortran interface test
- [\[#232\]](https://github.com/spglib/spglib/pull/232) - Add more fortran wrapper tests

### Build system improvement

- [\[#210\]](https://github.com/spglib/spglib/pull/210) - Improve cmake build
- [\[#215\]](https://github.com/spglib/spglib/pull/215) - Fix pre-commit
- [\[#233\]](https://github.com/spglib/spglib/pull/233) - Cmake refactoring
- [\[#260\]](https://github.com/spglib/spglib/pull/260) - Fix #191 again
- [\[#266\]](https://github.com/spglib/spglib/pull/266) - Add rpm spec file
- [\[#270\]](https://github.com/spglib/spglib/pull/270) - Fix codecov package dependency
- [\[#272\]](https://github.com/spglib/spglib/pull/272) - Fedora packaging maintenance
- [\[#274\]](https://github.com/spglib/spglib/pull/274) - \[Temp\] Disable intel toolchain
- [\[#279\]](https://github.com/spglib/spglib/pull/279) - Various cmake cleanups
- [\[#300\]](https://github.com/spglib/spglib/pull/300) - tests: Refactor testing framework
- [\[#302\]](https://github.com/spglib/spglib/pull/302) - Silence C warning "arrays with different qualifiers"
- [\[#304\]](https://github.com/spglib/spglib/pull/304) - Set C standard to C11
- [\[#309\]](https://github.com/spglib/spglib/pull/309) - Add pytests to ctest

### Python packaging improvement

- [\[#203\]](https://github.com/spglib/spglib/pull/203) - Add setup.cfg for python interface
- [\[#214\]](https://github.com/spglib/spglib/pull/214) - Temporary fix for scikit-build on macs
- [\[#218\]](https://github.com/spglib/spglib/pull/218) - Move to `pyproject.toml` build
- [\[#229\]](https://github.com/spglib/spglib/pull/229) - Minor fix of pyproject.toml
- [\[#268\]](https://github.com/spglib/spglib/pull/268) - Refactor python optional dependencies
- [\[#269\]](https://github.com/spglib/spglib/pull/269) - Python: fix packaging

### CI and releasing

- [\[#241\]](https://github.com/spglib/spglib/pull/241) - Improvre pre-commit and github action
- [\[#250\]](https://github.com/spglib/spglib/pull/250) - Use tag format for PyPi action
- [\[#254\]](https://github.com/spglib/spglib/pull/254) - Include an autoreleaser
- [\[#275\]](https://github.com/spglib/spglib/pull/275) - Change PyPI publishing to `Trusted publishing`
- [\[#280\]](https://github.com/spglib/spglib/pull/280) - fix: Hotfix Fedora CI
- [\[#285\]](https://github.com/spglib/spglib/pull/285) - ci: Use container with pre-installed toolchains
- [\[#286\]](https://github.com/spglib/spglib/pull/286) - Add windows and macos CI
- [\[#287\]](https://github.com/spglib/spglib/pull/287) - Refactor Github CI
- [\[#294\]](https://github.com/spglib/spglib/pull/294) - ci: Silence codecov until all coverage tests are uploaded
- [\[#306\]](https://github.com/spglib/spglib/pull/306) - ci: Add concurrency to GH actions
- [\[#307\]](https://github.com/spglib/spglib/pull/307) - ci: Switch to native pip instead of conda
- [\[#315\]](https://github.com/spglib/spglib/pull/315) - Various fixes
- [\[#319\]](https://github.com/spglib/spglib/pull/319) - Fix python 3.12 CI
- [\[#320\]](https://github.com/spglib/spglib/pull/320) - Fix target branch for packit
- [\[#321\]](https://github.com/spglib/spglib/pull/321) - Fix: build wheel workflow

### Misc

- [\[#207\]](https://github.com/spglib/spglib/pull/207) - Add benchmark for `get_symmetry_dataset`

## V2.0.2 (6 Nov. 2022)

- Fix segmentation fault in `spgms_get_symmetry_with_site_tensors` with high symprec [\[#195\]](https://github.com/spglib/spglib/pull/195)
- Fix possible segmentation fault in `get_magnetic_dataset` with high symprec [\[#196\]](https://github.com/spglib/spglib/pull/196)

## V2.0.1 (31 Aug. 2022)

- Fix magnetic tolerance for judging type-II MSG (#187)

<!-- SPHINX-END -->

## Old changelog

2022-08-09 Atsushi Togo <atz.togo@gmail.com>

```
* This ChangeLog is obsolete.

New releases are presented at https://spglib.github.io/spglib/releases.html.
Detailed changes are found in git-log.
```

2022-05-06 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.16.5

Bug fix of issue #157.
```

2022-05-05 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.16.4

Maintenance release including many minor fixes and improvements.
```

2021-12-8 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.16.3

Just to deploy source code to PyPI.
```

2021-08-10 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.16.2

Collection of fixes and improvements.

* PR #130 by odidev

Github workflow to build PyPI wheels.

* PR #124 by site-g

Fix wyckoff position database.

* PR #120 by site-g

Fix Delaunay reduction.

* PR #119 by dmt4

Improvements of fortran interface.

* PR #117 by Jonas-Finkler

Added standardize_cell to fortran interface.
```

2021-01-29 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.16.1

Bug fix of fortran wrapper.
```

2020-07-30 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.16

Minor update. Symmetry operations in spg_database.c for
Hall numbers 212, 213, 214 are re-ordered to start by identity
operation.
```

2020-04-28 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.15.1

Critical bug fix for collinear spin symmetry.

* src/spin.c (get_operations):

Collinear spin symmetry was wrongly handled. This was fixed at
commit bf954039.
```

2020-04-17 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.15

'crystallographic_orbits' was added to SpglibDataset by Lauri Himanen.
Rust interface was implemented by Seaton Ullberg.
```

2019-07-30 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.14.1

Version number increment just to release pypi package.
```

2019-07-30 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.14

Improvement against cyclic point change over applying
standardizaion repeatedly. More specifically, when we have an
input crystal structure, we standardize it by spglib then we may
get another structure. We apply the standardization again to this
standardized structure and we get another structure. Some people
may expect after several iteration, the input and output
structures become same. This version provides some improvement on
this. But unfortunately it could not be perfect, for example, this
can be not working for crystal structures having special metrics
given in the table of Euclidean normalizer section of ITA.
```

2019-07-02 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.13

* src/spacegroup.c (match_hall_symbol_db_ortho, match_hall_symbol_db_monocli):

Fix Issue#57 of SeekPath: Standardizations of choices of a,b,c axes
for Cmma (67), Ccca(68), Ibca(73), Imma(74) were fixed. For
monoclinic systems, lengths of non-unique axes are now checked by a
exhaustive way.

* src/kpoint.c (check_mesh_symmetry):

Issue#79: Treatment of ir-kpoint search for special cases with three fold
rotation (e.g., hexagonal or primitive FCC).
```

2019-02-06 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.12.2

Minor release for memory leak fix (PR#71).
```

2019-01-30 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.12.1

* src/spglib.h:

Bug fix of PR #70. #include <stddef.h> is inserted. In principle,
this line is necessary to compile.
```

2019-01-29 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.12.0

After this minor version, addition of new features will be stopped
until version 2.0 will be released.

* src/sitesym_database.c (ssmdb_get_site_symmetry_symbol):

Experimental support of site symmetry symbols.

* src/pointgroup.c:

Comparisons of basis vector lengths are more carefully done for
letting them work for different computer architectures to treat
very small numerical different that is close to the machine
precision.

* src/mathfunc.c (mat_Dmod1):

Similarly, MOD-Z operation (bring values x inside the rage of 0<=
x < 1) is now done considering the difference of machine
precision.

* src/kpoint.c (kpt_get_irreducible_reciprocal_mesh):

To enable dense k-point mesh sampling, integer types of variables
that represent grid point indices were changed from int to size_t.
But to support current functions with the same API, adapters were
written for these functions (having 'dense' in their function
names until v2.0 will be released) and currently they are working
in the same way as before. Finally the API change will be realized
at the v2.0
```

2018-12-07 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.11.2

* src/refinement.c (find_similar_bravais_lattice):

Bug fix. Again, origin shift was still not correctly computed when
the standardized basis vectors are rotated in this method to find
most similar choice of basis vectors to the idealized ones. This
bug affected to 'origin_shift' and 'std_positions' in spglib
dataset and the functions 'spg_standardize_cell',
'spg_find_primitive', and 'spg_refine_cell'. Although there
existed this bug, tests unfortunately passed. So a new test to
check Wyckoff positions consistency was added to find possible
bugs related to origin shift because this bug was found by getting
wrong Wyckoff positions reported by a user.
```

2018-11-12 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.11.1

* src/refinement.c (find_similar_bravais_lattice):

Bug fix. Origin shift was not correctly computed when the
standardized basis vectors are rotated in this method to find most
similar choice of basis vectors to the idealized ones. This bug
affected to 'origin_shift' and 'std_positions' in spglib dataset
and the functions 'spg_standardize_cell', 'spg_find_primitive',
and 'spg_refine_cell'.
```

2018-11-09 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.11.0

* src/refinement.c (find_similar_bravais_lattice):

This looks for a
similar choice of standardized basis vectors to that of idealized
standardized basis vectors among those generated by proper
symmetry operations.

* src/refinement.c (measure_rigid_rotation):

This computes a rigid rotation matrix introduced
in the idealization step. The rotation is made to agree a-axes
and a-b planes between systems before and after idealization.
```

2018-08-01 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.10.4

Collection of minor fixes.
A text on spglib algorithm is prepared for this version. This
text will be available soon at arXiv. To be consistent between
text and code, the code refactoring was made.
```

2018-01-13 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.10.3

* src/overlap.c:

New algorithm to search translation part of space group operation
is implemented by Michael Lamparski to improving the search speed
especially for large unit cell systems. For this, new files
'overlap.*' are added and so the version number is incremented.
```

2017-12-13 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.10.2

* src/Makefile.am:

Automake didn't work due to the lack of determination.{c,h}
lines. This was fixed.
```

2017-10-27 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.10.1

* src/spglib.c (get_dataset):

A large refactoring was made to replace the outer-most iteration
loop of symmetry search algorithm to a new file 'determination.c'.

* src/spglib.c (spg_get_schoenflies,spg_get_international):

These routines had accessed directly to a function close to
spacegroup.c. Now these are replaced by passing through
'get_dataset'.
```

2017-10-22 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.10.0

'mapping_to_primitive' and 'std_mapping_to_primitive' in the
spglib dataset are put available on the spglib
document. 'spg_get_hall_number_from_symmetry' function
('get_hall_number_from_symmetry' method for python) is also
available on the spglib document.
```

2017-10-02 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.9.10

Many minor fixes and updates after many months from v1.9.9.

* src/symmetry.c (search_pure_translations):

Pure translation search is made faster using a property of
group, which is significantly effective for supercells with large
multiplicity.
```

2016-12-14 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.9.9

* src/kpoint.c:

'get_ir_reciprocal_mesh' was supported to work with mesh numbers
that are consistent with symmetry. Now this function somehow
supports when mesh numbers are symmetrically broken.
```

2016-12-09 Atsushi Togo <atz.togo@gmail.com>

```
* src/symmetry.c:

static variable of 'angle_tolerance', 'sym_set_angle_tolerance',
'sym_get_angle_tolerance' were removed toward letting it
thread-safe. Now 'angle_tolerance' is passed as an argument to
many functions in spglib.
```

2016-11-02 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.9.8

* src/cell.c (cel_is_overlap_with_same_type):

'cel_is_overlap_with_same_type' is implemented to check
overlapping of atoms along with checking atom type
agreement. Using this, atoms that have about the same position but
with different types can be separately handled.
```

2016-10-19 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.9.6

Collection of minor fixes.

* src/cell.c (cel_set_cell):

Reduced atomic positions of the input cell are initialized by
their moving in the range between -0.5 and 0.5. The cell shape is
kept unchanged but this may need to be considered to handle some
extreme case in the future.
```

2016-09-05 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.9.5
* src/spglib.h (SpglibError):

'spg_get_error_code' and 'spg_get_error_message' were made to
handle a few types of internal errors.

* src/spglib.c, src/cell.c (cel_any_overlap_with_same_type):

To check overlap of atoms, 'cel_any_overlap' and
'cel_any_overlap_with_same_type' were implemented. The former
doesn't check atomic type but the later does. Currently this check
is done before symmetry search and the later is used for
it. Therefore overlap of different types of atoms is allowed.
```

2016-05-06 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.9.4

* src/spglib.h (SpglibDataset):

The member of SpglibDataset, 'setting', is renamed to 'choice',
and 'pointgroup_number' is removed.

* src/cell.c (get_overlap_table):

Fix unexpected behavior. This might happen in a rare case with
large tolerance.
```

2016-04-28 Atsushi Togo <atz.togo@gmail.com>

```
* src/spglib.c (spg_get_spacegroup_type):

Arithmetic crystal class number and symbol are returned. Python
interface 'get_spacegroup_type' was created.

* src/spglib.c (get_dataset):
* src/site_symmetry.c (ssm_get_exact_positions, set_exact_location):

Two iterative loops are implemented. The outer one is in
get_dataset, and the inner one is in ssm_get_exact_positions.
Each site-symmetry is checked in set_exact_location. In the inner loop,
symmetry tolerance is controlled until all site-symmetries become
reasonable. If the inner loop finally fails, the symmetry
tolerance in the outer loop is reduced and the symmetry search
starts from the beginning.
```

2016-04-17 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.9.2

* src/symmetry.c (get_lattice_symmetry):

The treatment after the condition, order of point symmetry
becomes larger than 48, is found was bad and it induced
segfault. It is fixed and a better treatment for angle_tolerance
is implemented as an iterative method.

* src/spglib.c (spg_delaunay_reduce):

spg_delaunay_reduce is made as an interface to the Delaunay
reduction.

* src/spacegroup.c (search_spacegroup):

A check if excess symmetry operations are found for a primitive
cell is inserted by watching its point group. This made the
algorithm of database matching safer.
```

2016-04-11 Atsushi Togo <atz.togo@gmail.com>

```
* src/delaunay.c:

lattice.{c,h} are renamed to delaunay.{c.h},
respectively. Function names in them, *get_smallest_lattice, are
changed to *delaunay_reduce.
```

2016-04-06 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.9.1

This is a minor update. Only a small notable change is in the
python wrapper, where for crystal structure, now a tuple format
can be sued.
```

2015-12-20 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.9.0

Pyspglib was considered as a special extension of spglib, but now
it starts to be a part of spglib as a usual extension. Therefore
the package name is changed to spglib, and this python wrapper
will be maintained to work at every update of c-spglib.

Scripts for pypi and conda package environments are prepared in
python wrapper by a lot of help from Paweł T. Jochym.

In previous versions, tests were in the test directory and space
group numbers were checked for many POSCARs by using combination
of ruby-spglib and shell script. Now a similar test is written in
a form of python unit test in the 'python' directory. Previous
'test' directory is renamed to 'ruby' and the POSCARs were moved
under the 'python/test' directory.
```

2015-12-13 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.8.3

* src/version.h:

Made functions to return spglib version number,
'spg_get_major_version()', 'spg_get_minor_version()',
'spg_get_micro_version()' in 'spglib.c'. This version number is
stored in 'version.h'.
```

2015-08-19 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.8.2.2

* src/spglib.c (is_rhombohedral):

Regardless of rhombohedral or hexagonal setting, a primitive
rhombohedral is found when using finding primitive cell functions.
```

2015-08-03 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.8.2.1

* src/spacegroup.c (spa_transform_to_primitive):

Bug fix of transformation matrix with centering. The side to be
multiplied was wrong.

* Version 1.8.2

* k-points and tetrahedron method

k-point grid definitions are separated to 'kgrid.c'.
'kgrid.{c,h}' and 'tetrahedron_method.{c,h}' are moved to a new
git repository of kspclib (https://github.com/atztogo/kspclib).
Tetrahedron method related spglib-APIs are removed.

* Choice of triclinic basis vector directions is implemented and
the definition is written in the document.
```

2015-07-09 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.8.1

In SpglibDataset, definitions of transformation matrix and origin
shift are changed, member names of '*brv_*' are changed to
'*std_*', and the members, pointgroup_number and
pointgroup_symbol, are added.

New function 'spg_standardize_cell' is implemented. This can also
give 'standardized primitive cell' with 'to_primitive =
1'.

Due to change of internal implementation, the order of symmetry
operations obtained can be different from the recently released
versions, but the set is the same.
```

2015-06-25 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.8

Relatively large code refactoring was made. Especially
'primitive.c' was changed a lot.

Write license statement (New BSD) in the source codes.

* src/spglib.c:

A new function 'spg_find_standardized_primitive' is
implemented. This function allows to create a primitive cell from
the obtained standardized conventional unit cell with
transformation using pre-determined choice of centring basis
vectors.
```

2015-06-15 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.7.4

Wyckoff position database for rhombohedral cell was
wrong. Probably this bug was hidden by the previous default choice
of basis vectors but from version 1.7.3 it became revealed.
```

2015-06-14 Atsushi Togo <atz.togo@gmail.com>

```
* src/sitesym_database.c

Bug in database of hP of rhombohedral was fixed. This bug was
affected after version 1.7.3. 'make_Wyckoff_db.py' is fixed and
then the database in 'sitesym_database.c' is recreated by
make_Wyckoff_db.py.
```

2015-04-06 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.7.3
```

2015-04-05 Atsushi Togo <atz.togo@gmail.com>

```
* src/triplet_kpoint.{c,h}:

Code to handle triplets of kpoints was separated from
kpoint.c. Detail of functions was revisited and modified, e.g.,
taking modulo of grid address is now included in
get_grid_point_double_mesh in most cases. This is probably safe
change, but may induce some tricky bug, which has to be
remembered. kpt_get_grid_point should be moved to spglib.c with
calling kpt_get_grid_point_double_mesh someday soon. It is noted
here for not forgetting that get_third_q_of_triplets_at_q in
triplet_kpoint.c might be written better around the point using
mat_modulo_i3. In triplet_kpoint.c, mat_modulo_i3 is used only
here, so eliminating this can move mat_modulo_i3 to kpoint.c as a
local function as before.
```

2015-04-03 Atsushi Togo <atz.togo@gmail.com>

```
* src/spacegroup.c (match_hall_symbol_db_ortho):

Change the convention of lengths of basis vector preference to:
a < b < c for hall number 290 and a < c < b for 291 or
a < b < c for hall number 291 and a < c < b for 290,
to preserve a < b convention found in Parthe et al, Journal of
Alloys and Compounds, 197 (1993) 291-301.
```

2015-03-31 Atsushi Togo <atz.togo@gmail.com>

```
* src/spacegroup.c (match_hall_symbol_db_ortho):

Fix choice of lengths of basis vectors in orthorhombic Pbca. There
are two choices for Pbca. The basis vectors can be chosen so as to
a < b < c or b < a < c, which correspond to these two choices,
respectively. The correspondence can be flipped, i.e., in a case,
a < b < c for hall number 290 and b < a < c for 291, but in some
other case, a < b < c for hall number 291 and b < a < c for 290.
```

2015-03-20 Atsushi Togo <atz.togo@gmail.com>

```
* src/hall_symbol.c (find_hall_symbol):

A critical bug was found in version 1.7.2, where origin shift was
given for the primitive cell but not for the Bravais lattice. This
was fixed. The version 1.7.2 was removed from the download site
and temporary 1.7.3-pre is put on the download site.
```

2015-03-08 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.7.2

* src/niggli.*:

Routine of Niggli cell reduction is imported from
https://github.com/atztogo/niggli.
```

2015-02-24 Atsushi Togo <atz.togo@gmail.com>

```
* src/spacegroup.c (get_symmetry_settings):

Monoclinic lattice vectors are chosen as |a| < |c| with b as the
unique axis. This is achieved by get_Delaunay_reduction_2D.

* src/lattice.c (get_Delaunay_reduction_2D):

2D Delaunay reduction is implemented based on that of 3D.

* src/lattice.c (lat_smallest_lattice_vector_2D):

Create an interface to access 2D Delaunay reduction.
```

2015-02-11 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.7.1

There are minor changes to make functions robust and code clean.

* src/spglib.c

spg(at)_get_symmetry_with_collinear_spin is changed to return
equivalent atoms.
spg_get_international and spg_get_schoenflies call iterative
routine to determine space group type.

* src/primitive.h:

Primitive structure is defined. This is expected to make routines
related to primitive cell comprehensive. Memory for a pointer of
the Primitive variable may be allocated using prm_alloc_primitive
and freed by prm_free_primitive.

* src/spacegroup.c:

spa_get_spacegroup was replaced by iterative routine and now
returns a Primitive structure.
```

2014-11-19 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.7

Space group operations for non-standard setting can be determined
using spg_get_dataset_with_hall_number. To achieve this, many
lines are added and changed. Due to the big change of the code,
there may be bugs, and maybe it's safer to wait for a while to use
this version for mission critical systems.

Two important changes are:

* src/spglib.c (spg_get_dataset_with_hall_number)

In space group type determination, Hall symbol serial number can
be specified.

* src/spglib.h (SpglibDataset)

Crystallographic setting information is stored in
'setting'. Symmetrized and standardized crystal structure is stored
in 'n_brv_atoms', 'brv_lattice', 'brv_types', and 'brv_positions',
which are the same information obtained using 'spg_refine_cell',
except that non-standard setting can be specified in
'spg_get_dataset_with_hall_number'.
```

2014-10-16 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.6.4

* src/refinement.c (refine_cell):

An iterative approach is implemented for refine_cell.
```

2014-09-12 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.6.3

* src/primitive.c:

Atom type is checked in finding a primitive cell. This enables to
handle different types of atoms that locate at the same position.
for the case of an automatically generated crystal structure data
from a fractional occupancy, etc.

* Version 1.6.2

Minor bug fixes.

* src/spglib.c (spg_get_symmetry_from_database):

An API to access space group operations database is implemented.
```

2014-08-04 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.6.1:

* src/{refinement.*,primitive.*,spglib.c}

Fix symmetry finding bug for supercell that has lower point group
than the primitive cell.

* src/spglib.*

New functions of spg_get_spacegroup_type and
spg_get_grid_point. The former offers direct access to space
group type database using hall symbol number. The later performs
converting grid address to grid point index on the reciprocal
mesh.

* pyspglib update.
```

2014-02-17 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.6.0:

* src/spglib.c (spg_get_spacegroup_type):

A function is made for accessing space-group-type database.
```

2014-01-07 Atsushi Togo <atz.togo@gmail.com>

```
* src/tetrahedron_method.*:

Tetrahedron method is implemented. Example is found in the example
directory.
```

2013-09-10 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.5.2:

* kpoint.c (kpt_relocate_BZ_grid_address):

New function kpt_relocate_BZ_grid_address is implemented in
kpoint.c. k-points on the mesh of the reciprocal primitive lattice
are relocated to fit in the Brillouin zone. To treat
translationally equivalent k-points, the memory space of
bz_grid_address[prod(mesh + 1)][3] is required.  bz_map[prod(mesh
* 2 - 1)] is used to map k-points on the expanded k-space where
the shape is same as the reciprocal primitive lattice but the size
along each lattice vector is multiplied by mesh * 2 - 1. BZ zone
is included in this expanded k-space, so the bz_map can hold the
location of relocated k-points. Irreducible k-points are stored
from 0 to index of prod(mesh) - 1 in bz_grid_address in the same
order of grid_address, and the translationally equivalent k-points
are stored after that. Fast indexing of the location of relocated
k-points can be executed using bz_map with similar way to usual
grid_address scheme.

* kpoint.c (kpt_get_BZ_triplets_at_q):

New function kpt_get_BZ_triplets_at_q is implemented using
relocate_BZ_grid_address. This generates a set of triplets that
cleanly fit in BZ.
```

2013-09-10 Atsushi Togo <atz.togo@gmail.com>

```
* kpoint.c:

Remove get_triplets_reciprocal_mesh and
extract_triplets_reciprocal_mesh_at_q.  Add
get_grid_triplets_at_q. This function treats q-points on boundary
correctly.

* spglib.c (spg_get_symmetry):

Previously symmetry operations were obtained by sym_get_operation.
Now it is changed so that they are extracted from spglib_dataset.
```

2013-04-16 Atsushi Togo <atz.togo@gmail.com>

```
* symmetry.c (get_operation):

In some case, get_space_group_operation returns more than 48
symmetry operations for a primitive cell. To avoid this, an
iterative approach, which is just reducing tolerance gradually to
achieve number of symmetry operations <= 48, is implemented.

* symmetry.c:
In the symmetry operation search, rot[] and trans[] were used. Now
they are replaced by Symmetry *symmetry in symmetry.c.
```

2013-04-14 Atsushi Togo <atz.togo@gmail.com>

```
* Fortran interface given by Dimitar Pashov:

The new fortran interface is given by Dimitar Pashov, which is
much safer than the previous one. The new fortran interface, its
example, and makefile are found in the example directory as
spglib_f08.f90, example_f08.f90 and Makefile_f08.

* spglib.h, spglib.c, kpoint.h, kpoint,c:

Symprec parameter was removed from spg_get_stabilized_reciprocal_mesh.
```

2013-01-28 Atsushi Togo <atz.togo@gmail.com>

```
* spglib.c, spglib_f.c:

Pointed out by Michael Rutter that the length of characters of
space-group type symbol (International table) was wrong in
spglib_f.c. That of Schoenflies symbol was also found wrong. The
former and latter were fixed as from 21 to 11 and from 10 to 7,
respectively.
```

2012-10-10 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.3

Rethink algorithms to search symmetry operations for speed-up and
stability. Many small modifications are applied. Some parts of the
codes that look unnecessary were removed but this change might
make the code unstable.

* symmetry.c (get_translation):

Improve OpenMP efficiency. Number of atoms to enable OpenMP
concurrency is set as 1000 (NUM_ATOMS_CRITERION_FOR_OPENMP).
```

2012-10-09 Atsushi Togo <atz.togo@gmail.com>

```
* symmetry.c (sym_get_pure_translation, get_translation):

In 'sym_get_pure_translation', iterative approach to search
pure_translation was employed. But 'sym_get_pure_translation' was
only used from two functions in primitive.c. In these two
functions, tolerance is iteratively reduced when the search
fails. Because there is no need to do iteration twice, the
iteration part in 'sym_get_pure_translation' was removed.

* primitive.{c,h}:

'get_primitive_with_pure_translation' was replaced by
'get_primitive_and_pure_translation'. The former was only used
from 'get_operation' in symmetry.c and the iterative approach was
implemented in 'get_operation'. The iterative part was moved into
'get_primitive_and_pure_translation'.

The Primitive structure was defined in primitive.h. This just
packs Cell * primitive_cell and VecDBL * pure_trans. The Primitive
structure is only used in 'get_operation' in symmetry.c and
'(prm_)get_primitive_and_pure_translation' in primitive.c.
```

2012-10-08 Atsushi Togo <atz.togo@gmail.com>

```
* spacegroup.c (get_hall_number_local_iteration):

REDUCE_RATE to reduce number of symmetry operations is changed
from 0.2 to 0.95. Symmetry operations depend on tolerance
delicately.

* Remove many lines of debug_print:

Now to follow the flow is more important than to see the
detailed behavior of implementation.
```

2012-10-03 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.2.5
* primitive.c:

Separate out get_primitive_with_mapping_table from
prm_get_primitive_with_mapping_table. Rename
prm_get_primitive_with_all to
get_primitive_with_pure_translation.
In get_primitive_with_pure_translation, it was
  if (pure_trans->size > 0)
but not it was fixed to
  if (pure_trans->size > 1)
not to search primitive cell when pure_trans->size == 1.
```

2012-10-01 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.2.4
* kpoint.c (get_ir_reciprocal_mesh):

In spglib, k-point grid is indexed as, for example,

0 1 2 3 4 5 6 -5 -4 -3 -2 -1

for 12 points mesh. Now the different index style such as

0 1 2 3 4 5 -6 -5 -4 -3 -2 -1

is activated by CFLAGS='-DGRID_BOUNDARY_AS_NEGATIVE' before
running configure script at the compilation time.

* symmetry.c (get_operation):

In the iterative reduction of tolerance of primitive cell search,
pure_trans was searched by a fixed tolerance rather than the
reduced one. This is fixed to use the reduced tolerance at each
iteration.
```

2012-08-13 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.2.3
* symmetry.c (is_identity_metric):

To check identity of metric tensors, too small angle induces
numerical size error, so if the dtheta is calculated smaller than
1e-6, automatically set the angles be identical.

* spacegroup.c (get_hall_number_local_iteration):

Implementation of iterative reduction of symmetry tolerance to
search symmetry operation to get Hall symbol is separated as the
function get_hall_number_local_iteration and the tolerance
reduction rate of REDUCE_RATE is changed from 0.2 from 0.95.
```

2012-07-17 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.2.2
* lattice.c (get_centering):

Transformation A-centring monoclinic lattice to C-centring
monoclinic lattice preserving b-axis is not well handled and now
the matrix 'monocli_a2c' is used for it.
```

2012-07-14 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.2.1
* pointgroup.c (laue_one_axis):

Transformation matrix is expected to be always positive, but it
was not correctly implemented.
```

2012-05-28 Atsushi Togo <atz.togo@gmail.com>

```
* refinement.c (reduce_symmetry_in_frame):

Fix memory leak by freeing lattice_trans.
```

2012-04-26 Atsushi Togo <atz.togo@gmail.com>

```
The numbering of the hall symbol is included in the 'struct
SpglibDataset'. This starts from 1 to 530.
```

2012-04-25 Atsushi Togo <atz.togo@gmail.com>

```
* spglib.c, spglib.h:

To find symmetry with angle tolerance, for spg_* functions,
respective spgat_* functions are prepared. In the spgat_*
functions, the last variable is the angle tolerance (in degrees).

* symmetry.c:

The way to find lattice symmetry is changed. Now a new static
variable, 'angle_tolerance', can be used for the tolerance of
angle. Therefore the non-diagonal elements of metric tensor are
directly compared with this angle. 'angle_tolerance' can be
accessed via 'sym_set_angle_tolerance' inside spglib. However the
default behavior does not use the angle tolerance. In the default
behavior, averaged_lattice_vector_length * sin(delta_theta) is
compared with usual 'symprec'. The default behavior is invoked
when 'angle_tolerance' has negative value. These are implemented
in 'is_identity_metric' in symmetry.c.
```

2012-04-12 Atsushi Togo <atz.togo@gmail.com>

```
* lattice.c (get_centering):

Correction matrix for I-centring monoclinic was implemented.
```

2012-03-03 Atsushi Togo <atz.togo@gmail.com>

```
* primitive.c:

The functions of 'prm_get_current_tolerance',
'set_current_tolerance' and the value of static double
current_tolerance were introduced in 'primitive.c'. These are used
to return reduced tolerance value that is iteratively determined
in 'prm_get_primitive_with_mapping_table'.

The iterative technique is used in several parts to obtain
possible primitive cell parameters by reducing symmetry
tolerance. Only in 'get_overlap_table' in 'primitive.c', the
iterative technique is used with reducing and increasing the
tolerance, i.e., in the other parts, only reducing.
```

2012-03-02 Atsushi Togo <atz.togo@gmail.com>

```
* Version 1.1.2
* refinement.c, primitive.c, symmetry.c:

Modifications at many small parts of the code for improve
robustness with large symmetry tolerance values.
```

2012-02-28 Atsushi Togo <atz.togo@gmail.com>

```
* primitive.c (get_overlap_table):

Simplify the algorithm for search overlapping table. The new
algorithm is as follows: Symmetry tolerance monotonically
decreases by REDUCE_RATE = 0.95 when the search is failed. When
number of overlapping atoms is found smaller than the ratio of
number of atoms between original and primitive cells, the symmetry
tolerance is doubled. For this algorithm, 'check_overlap_table'
function was implemented.
```

2012-02-09 Atsushi Togo <atz.togo@gmail.com>

```
* spin.{c,h}:

'spin.{c,h}' are added. Colliear spins on atoms are employed as
additional constraints to search symmetry. The API is
'spg_get_symmetry_with_collinear_spin'. (reduce_symmetry_in_frame):

Bug fix to get refined symmetry operations. When the original cell
is not primitive and the lattice symmetry is lower than that
primitive lattice, the symmetry of primitive has to be
reduced to those belongs to the lower symmetry. But this was not
correctly done.
```

2011-10-05 Atsushi Togo <atz.togo@gmail.com>

```
* spglib.c (spg_get_pointgroup):

'spg_get_pointgroup' is implemented. This is used to obtain point
group symbols.
```

2011-09-30 Atsushi Togo <atz.togo@gmail.com>

```
* site_symmetry.c (get_Wyckoff_notation):

Bug fix of assignment of Wyckoff letters
'sitesym_database.c' is compressed. Now only the first element of
'coordinates' is used to assign a Wyckoff letter.
```

2011-08-09 Atsushi Togo <atz.togo@gmail.com>

```
* symmetry.c (sym_get_pure_translation):

Iterative reduction of symmetry tolerance to find pure
translations is implemented as well as different parts.
```

2011-07-29 Atsushi Togo <atz.togo@gmail.com>

```
* kpoint.c, kpoint.h:

symmetry_kpoint.c and symmetry_kpoint.h are renamed as kpoint.c
and kpoint.h, respectively.
```

2011-07-06 Atsushi Togo <atz.togo@gmail.com>

```
* site_symmetry.c:

A set of equivalent atoms is returned. It is accessible
through spg_get_dataset. (spglib-1.0.7)
```

2011-07-05 Atsushi Togo <atz.togo@gmail.com>

```
* site_symmetry.c, sitesym_database.c, spglib-1.0.6:

Newly added two source files (and also *.h). The part related to
site symmetry in refinement.c is moved to site_symmetry.c. Wyckoff
position determination is implemented in site_symmetry.c and the
database is in sitesym_database.c. The pointers in the structure
SpglibDataset are made free by calling spg_free_dataset. It was
difficult to implement Wyckoff position determination without
Yusuke Seko's formatted crystallographic database. I appreciate his
kind offer. (spglib-1.0.6)

* primitive.c (prm_get_primitive):

The function is modified to return mapping table from input cell
to primitive cell as the first argument.
```

2011-06-18 Atsushi Togo <atz.togo@gmail.com>

```
* symmetry.c:

OpenMP is included to search translations with each rotation.
Compilation with gcc is achieved by '-lgomp' and '-fopenmp'.

* refinement.c:

Performance tuning was made for 'get_refined_symmetry_operations'.
```

2011-06-18 Atsushi Togo <atz.togo@gmail.com>

```
* refinement.c:

In 'ref_get_refined_symmetry_operations', it was forgotten to
consider origin shift to obtain symmetry operations from
database. This is fixed by modifying translations in
'set_translation_with_origin_shift'.
```

2011-06-15 Atsushi Togo <atz.togo@gmail.com>

```
* spglib-1.0.4:

The new version of spglib is released. To the next version,
implementation of Wyckoff positions is planned.
```

2011-06-15 Atsushi Togo <atz.togo@gmail.com>

```
* pyspglib:

pyspglib didn't work correctly. It was fixed.
```

2011-06-15 Atsushi Togo <atz.togo@gmail.com>

```
* spglib.c:

A new function 'spg_get_dataset is implemented. This returns a
'SpglibDataset' type structure that is defined in 'spglib.h'. This
includes refined information of crystal symmetry. For example,
the symmetry operations are derived based on the operations in
database and so they are different from those obtained from
'spg_get_symmetry'. Wyckoff positions are to be implemented.
```

2011-06-13 Atsushi Togo <atz.togo@gmail.com>

```
* spg_database.c:

A static array of 'symmetry_operations' is compressed because it
was very large. It contains symmetry operations of 530 hall
symbols. Each space group operation requires a rotation 3x3 matrix
and a translation vector, therefore 12 elements were implemented
for that. Now it is compressed using ternary numerical system for
rotation and duodecimal system for translation. This is achieved because
each element of rotation matrix can have only one of {-1,0,1}, and
the translation can have one of {0,2,3,4,6,8,9,10} divided by
12. Therefore 3^9 * 12^3 = 34012224 different values can map space
group operations. In principle, octal numerical system can be used
for translation, but duodecimal system is used for the convenient.
The fact that determinant of rotation matrix is -1 or 1 could be
used as a constraint to reduce the number of mapping space. These
may be implemented if more compression is required.
```

2011-06-10 Atsushi Togo <atz.togo@gmail.com>

```
* spglib.c:

spg_show_symmetry was removed.
```

2011-05-17 Atsushi Togo <atz.togo@gmail.com>

```
* primitive.c, etc

A tolerance to find primitive lattice vectors sometimes fails
because found pure translations do not give possible lattice
vectors in which the volume ratio of the input cell and primitive
cell does not match to the number of pure translations. So
iterative tolerance reduction is implemented. The reduced
tolerance should be used to find spacegroup and refined cell
because to hold consistency with symmetry operations to be found
for primitive cell. The reduced tolerance is not used inside
symmetry operation search. The tolerance used to find primitive
cell is stored in a static variable 'tolerance' in primitive.c and
this can be accessed via prm_get_tolerance().

Old 'prm_get_primitive' was removed and new one is that of
'prm_get_primitive_with_pure_trans' renamed. So now pure
translations are always required to call to obtain primitive cell.

When calling 'prm_get_primitive' and if the input cell is already
primitive, primitive->size=0 was returned in the previous
implementation, but in the new implementation, primitive cell that
has smallest lattice vectors is returned.
```

2011-04-21 Atsushi Togo <atz.togo@gmail.com>

```
* refinement.c (refine_cell):

When an input structure is a primitive cell but the lattice is not
smallest, 'refine_cell' didn't work correctly. This is fixed by
using new function of 'prm_get_cell_with_smallest_lattice' in
primitive.c. This function was originally in spacegroup.c as
'get_cell_with_smallest_lattice'. Following this, spacegroup.c,
primitive.c, and primitive.h were modified.
```

2011-03-18 Atsushi Togo <atz.togo@gmail.com>

```
* spglib-1.0.3.1:

A critical bug in the structure refinement in refinement.c was
removed. A tolerance parameter in hall_symbol.c came back to
is_hall_symbol.
```

2011-03-15 Atsushi Togo <atz.togo@gmail.com>

```
* spglib-1.0.3:

Crystal structure refinement by spg_refine_cell is
implemented. Code around tolerance is tuned up, hopefully
becoming more robust with large tolerance.

* refinement.c:

bravais.{c,h} are renamed to refinement.{c,h}.
```

2011-03-14 Atsushi Togo <atz.togo@gmail.com>

```
* debug.h (SPGWARNING):

Debug option of SPGWARNING has been defined. This may be used for
beta-tests.

* bravais.c, spacegrouop.c, hall_symbol.c, lattice.c, primitive.c:

Tolerance parameter has been reconsidered to make finding
spacegroup robust with a loose tolerance value. Tolerance is used
for Cartesian distance, norm squared, or volume. It wold be
avoided for ratio, reduced position or distance.

* bravais.c:

Exact locations of atoms are obtained by get_exact_location in
bravais.c.

* sapcegroup.c:

Iterative search for hall symbol is implemented in
get_hall_number. The tolerance is sequentially reduced until
finding a hall symbol. With the reduced tolerance, symmetry
operations are researched by sym_reduce_operation. The cost of the
reduction of symmetry operations is expected much smaller than that
of the increase of symmetry operations with increasing tolerance.

* hall_symbol.c:

All tolerance parameters were kicked out.

* lattice.c:

get_base_center is modified to remove the tolerance parameter. The
centring check is now based on
integers. lat_smallest_lattice_vector returns 1 or 0 to show if
the lattice has non-zero volume or not.

* primitive.c:

Primitive lattice is made of pure translations. Previously the
primitive lattice vectors are constructed from pure
translations. Currently, it is set such that the primitive lattice
parameters multiplied with 'integers' become the original
lattice parameters.

* mathfunc.c (mat_is_int_matrix):

A macro of INT_PREC (currently set 0.1 ) is used for checking a
value is integer or not in mat_is_int_matrix.
```

2011-02-18 Atsushi Togo <atz.togo@gmail.com>

```
* spglib-1.0.2:

Bug fix in finding lattice point symmetry (symmetry.c).
spg_get_bravais is added and spg_get_international_with_bravais
was removed.
```

2010-12-26 Atsushi Togo <atz.togo@gmail.com>

```
* spglib-1.0.1:

By the help of Pawel T. Jochym, the setup.py in the ASE-interface,
pysgpblib, has been updated for matching the latest spglib.
```

2010-11-08 Atsushi Togo <atz.togo@gmail.com>

```
* spglib-1.0.0:

A lot of source code was re-written for this one month. This
release contains the algorithms of symmetry operation finding and
space-group type determination which are different from the
versions before spglib 0.7.4.
```

2010-11-08 Atsushi Togo <atz.togo@gmail.com>

```
Algorithm to search basis vectors is changed to that of
[Grosse-Kunstleve, Acta Cryst. A55, 383-395 (1999)], which is
implemented in lattice.c. bravais.{c,h}, bravais_virtual.{c,h} are
removed.  spacegroup_type.{c,h} are moved to spacegroup.{c,h}.
```

2010-11-05 Atsushi Togo <atz.togo@gmail.com>

```
* lattice.c:

lattice.c and lattice.h are added. Delaunay reduction is moved
there from bravais.c.
```

2010-11-02 Atsushi Togo <atz.togo@gmail.com>

```
* spglib 1.0-beta:

New release under the modified BSD license.

* pointgroup.c:

Sometimes symmetry operations that don't match point-group type
can be found. In the current implementation, some of non-necessary
operations are abandoned. For this, get_cast_symmetry,
check_pointgroup_condition, ptg_get_symmetry_pointgroup are added.
However still this implementation is not perfect. Maybe another
strategy is required.
```

2010-10-27 Atsushi Togo <atz.togo@gmail.com>

```
* primitive.c:

trim_cell is divided into trim_cell, set_primitive_positions,
get_positions_primitive, get_overlap_table. is_overlap is
modified to check Cartesian distance, and moved to cel_is_overlap
in cell.c.

* symmetry.c (get_translation):

Atomic position overlap check part is modified to use
cel_is_overlap.

* cell.c:

cel_frac_to_cart and cel_cart_to_frac are removed.
```

2010-10-25 Atsushi Togo <atz.togo@gmail.com>

```
* bravais_arc.* is renamed to bravais_virtual.*

The function is_holohedry is removed.

* bravais.c:

brv_set_relative_lattice is added. brv_get_brv_lattice_in_loop is
modified for handling RHOMB case flexibly. get_metric was removed
and moved to mathfunc.c as mat_get_metric.

* pointgroup.c:

The first argument of ptg_get_holohedry is removed, which was used
just for debugging.

* symmetry.c:

get_space_group_operation is separated from
get_operation. sym_get_operation_direct, set_axes and
get_lattice_symmetry are newly added. sym_get_operation_direct is
the function for finding symmetry operations without relying on
Bravais lattice. The lattice symmetry is exhaustively searched
through the matrices whose elements are -1, 0, or 1.

* mathfunc.c:

mat_get_metric and mat_check_identity_matrix_d3 are added.
```

2010-10-22 Atsushi Togo <atz.togo@gmail.com>

```
* bravais.c:

The names of local functions starting with brv_... is changed to
get_brv_.... Add functions get_brv_cubic_primitive,
get_brv_tetra_primitive, get_brv_ortho_primitive to find more
variety of lattice transformation matrices.
```

2010-10-22 Atsushi Togo <atz.togo@gmail.com>

```
The space group type finder is replaced completely. spacegroup.*,
spacegroup_data.*, spacegroup_database.* are
removed. spacegroup_type.*, spg_database.*, and hall_symbol.* are
added. The helper tools to create hall symbol related database are
in database directories.
```

2010-10-09 Atsushi Togo <atz.togo@gmail.com>

```
spglib-0.7.4 is released.

cel_new_cell is renamed to cel_alloc_cell, and cel_alloc_cell
returns a pointer of the Cell structure. cel_delete_cell is renamed
to cel_free_cell.

sym_new_symmetry is renamed to sym_alloc_symmetry, and
sym_alloc_symmetry returns a pointer of the Symmetry
structure. sym_delete_symmetry is renamed to sym_free_symmetry.

The other functions returns Cell and Symmetry structures are
modified to return those pointers.
```

2010-10-05 Atsushi Togo <atz.togo@gmail.com>

```
* symmetry.c (get_index_with_least_atoms):

To search translation, it is efficient if an atom in least atoms
with an atomic type is used for the search origin. This function
looks for it. This is expected to accelerate especially when the
cell is huge and the lattice has high symmetry, but the number of
atoms of some atomic type is few.
```

2010-10-04 Atsushi Togo <atz.togo@gmail.com>

```
* primitive.c (prm_get_primitive_with_pure_trans):

Create new function 'prm_get_primitive_with_pure_trans' to avoid
searching same pure translations many times, which leads some
speed up.
```

2010-09-26 Atsushi Togo <atz.togo@gmail.com>

```
'const' of multidimensional arrays are removed. Just to notify it
should be 'const', 'SPGCONST' replaces those 'const', however it
is defined as nothing in mathfunc.h. This is done because of
avoiding lots of warning in C-compiler of 'gcc: incompatible
pointer type'.

* spglib_f.c:

spglib_f.c is updated and some examples are found in example.f90.
```

2010-04-12 Atsushi Togo <atz.togo@gmail.com>

```
* symmetry.c, mathfunc.c:

To make the translations clean, sym_get_fractional_translation in
symmetry.c and mat_Dmod1 in mathfunc.c are
added. sym_get_fractional_translation is supposed to look for the
translation that is most closest to one of 0, 1/6,
1/4, 1/3, 1/2, 2/3, 3/4, and 5/6. The first implementation
contained a bug and Jorg Meyer solved this.
```

2010-03-25 Atsushi Togo <atz.togo@gmail.com>

```
* bravais_art.c:

Space group of the virtual structures in the test directory
failed. Fixed points are as follows:
P-Cubic, P-Tetra, P-Ortho to Base center monoclinic, which was
collect in the old code, and I-Cubic to F-Orthorhombic.
Cubic to Rhombohedral. To obtain correct principal axes,
'is_rhombo' was implemented.
```

2010-03-22 Atsushi Togo <atz.togo@gmail.com>

```
* spglib.h, spglib.c:

spg_get_bravais_lattice is removed.

spg_get_conventional_symmetry is implemented. This is used to get
symmetry operations for the conventional unit cell. The
conventional unit cell is searched from the primitive cell found
from the input cell, i.e., even if the input cell is not
primitive, the primitive cell is automatically searched and the
conventional cell is determined. The Bravais lattice of the
conventional cell is returned as bravais_lattice. This
bravais_lattice is found including the internal atomic
positions. Therefore artificial cell is expected to be handled
correctly.
```

2010-02-11 Atsushi Togo <atz.togo@gmail.com>

```
* primitive.c:

There was a problem that primitive cell is not well found in naive
cases (inconsistency between tolerances in finding pure
translations and in finding overlapping atoms). David Lonie
implemented an iterative way to adjust symmetry finder tolerance
on the fly.
```

2009-07-15 Atsushi Togo <atz.togo@gmail.com>

```
* bravais.c:

Bug to find bravais lattice in FCC and rhombohedral, when the cell
is a supercell. This is probably ad-hoc fix.
```

2009-05-19 Atsushi Togo <atz.togo@gmail.com>

```
* symmetry_kpoint.c:

Time reversal symmetry is included.

* bravais.c (brv_smallest_lattice_vector):

Delaunay reduction is implemented instead of ABINIT
implementation.
```

2009-03-09 Atsushi Togo <atz.togo@gmail.com>

```
* symmetry.c:

Critical bug for finding symmetry for primitive cell which is
reduced from conventional cell.
```

2009-03-05 Atsushi Togo <atz.togo@gmail.com>

```
* symmetry_kpoint.c:

Functions to look for the irreducible points in reciprocal space
are implemented.
```

2009-02-25 Atsushi Togo <atz.togo@gmail.com>

```
* bravais_art.c:

Hexagonal to Orthorhombic symmetry lowering case is implemented.

* python module:

Python module is implemented. To use them, numpy is
required. Interface for ASE is implemented.
```

2008-08-22 Atsushi Togo <atz.togo@gmail.com>

```
* bravais_art.c:

This file is used for the cases that the lattice has higher
symmetry than the symmetry given by atomic positions. This does
not affect to the symmetry operations that is firstly obtained by
assuming the bravais lattice having high symmetry. Thus this file
works only for changing its bravais lattice. The information about
bravais lattice is used for only determining space group. Even if
the code in this file fails, or let's say, space group
determination fails), we can not say the symmetry operations are
wrong.
```

2008-07-11 Atsushi Togo <atz.togo@gmail.com>

```
* Most of the code

The procedure for searching a Bravais lattice from a primitive
cell is rewritten. The algorithm is also totally changed.

* test directory

For the test of space group search, 'test' directory is
created. Many POSCAR's are prepared in this directory. For running
tests, a small ruby code is written and put into this directory.
```

2008-05-28 Atsushi Togo <atz.togo@gmail.com>

```
* spacegroup_data.c (get_spacegroup_data_special_case):

Bug fix for space group determinations for special cases (Pcc2,
Pbam, Pccn).
```

2008-05-25 Atsushi Togo <atz.togo@gmail.com>

```
* *.c, *.h :

Many codes are re-built. Many functions are changed to
"static". The first three characters of public functions are
re-named using those file names.

* bravais.c :

Monoclinic is possibly improved. Monoclinic is difficult. It may
contain further bugs...
```

2008-05-01 Atsushi Togo <atz.togo@gmail.com>

```
* spacegroup.c (get_proper_rotation_class_6axis):

exit(1) is inserted.

* bravais.c/h:

"lattice_symmetry" is removed from the "Bravais" structure.

* spglib.h:

Misspelled function prototypes of spg_get_symmetry and
spg_check_symmetry are corrected.
```

2008-04-23 Atsushi Togo <atz.togo@gmail.com>

```
* Many patches by Xavier Andrade

For fixing many warning in many codes and gcc-4.3 related problem
in bravais.c.
```

[setuptools-scm]: https://setuptools-scm.readthedocs.io/en/latest/extending/#available-implementations
