# Summary of releases

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
- [\[#272\]](https://github.com/spglib/spglib/pull/272) - Fedora packaging maintainence
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

<!-- - [[#219]](https://github.com/spglib/spglib/pull/219) - Sync `pyproject.toml` build in rc branch to develop -->

## V2.0.2 (6 Nov. 2022)

- Fix segmentation fault in `spgms_get_symmetry_with_site_tensors` with high symprec [\[#195\]](https://github.com/spglib/spglib/pull/195)
- Fix possible segmentation fault in `get_magnetic_dataset` with high symprec [\[#196\]](https://github.com/spglib/spglib/pull/196)

## V2.0.1 (31 Aug. 2022)

- Fix magnetic tolerance for judging type-II MSG (#187)

## Release v2.0 and future plan

This release contains functions to search magnetic space group types which are
provided as experimental features. The behaviours of these magnetic related
functions (`*_magnetic_*`) with respect to the tolerance parameter (`symprec` or
{ref}`variables_mag_symprec`) may be changed in the future.

We are planning to separate irreducible k-points search from spglib. At spglib
version 4.0, those functions will be removed if we succeed to provide an
alternative package (hopefully with better functionalities) until then.

### C functions and structures

#### `SpglibSpacegroupType` structure

- Add `hall_number` member at version 2.0
- Used as return value of `spg_get_spacegroup_type` and `spg_get_spacegroup_type_from_symmetry`

#### `spg_get_spacegroup_type_from_symmetry`

- New at version 2.0
- Replacement of `spg_get_hall_number_from_symmetry`

#### `spg_get_symmetry_with_site_tensors`

- Experimental: new at version 2.0

#### `spg_get_magnetic_dataset`

- Experimental: new at version 2.0

#### `spg_get_magnetic_symmetry_from_database`

- Experimental: new at version 2.0

#### `spg_free_magnetic_dataset`

- Experimental: new at version 2.0

#### `spg_get_magnetic_spacegroup_type_from_symmetry`

- Experimental: new at version 2.0

#### `spg_get_hall_number_from_symmetry`

- Deprecated at version 2.0
- Will be removed at version 3.0

#### `spgat_get_symmetry_with_collinear_spin`

- Deprecated at version 2.0
- Will be removed at version 3.0
- Will be replaced by `spgms_get_symmetry_with_collinear_spin`

#### `spg_get_ir_reciprocal_mesh`

- Plan to make it deprecated at version 3.0

#### `spg_get_stabilized_reciprocal_mesh`

- Plan to make it deprecated at version 3.0

### Python interface

#### `get_symmetry`

- get_symmetry with `is_magnetic=True` is deprecated at version 2.0. Use
  `get_magnetic_symmetry` for magnetic symmetry search.
- As of version 2.0, the behavior of `get_symmetry` with zero magmoms (corresponding to type-II MSG) is changed.
  When all magmoms are zero, the newer `get_symmetry` returns the same spatial symmetry with `time_reversal=True` and `time-reversal=False`.
  This doubles the size of symmetry operations compared to the previous version.

#### `get_spacegroup_type`

- `hall_number` member is added at version 2.0.

#### `get_spacegroup_type_from_symmetry`

- New at version 2.0
- Replacement of get_hall_number_from_symmetry

#### `get_magnetic_symmetry`

- Experimental: new at version 2.0

#### `get_magnetic_symmetry_dataset`

- Experimental: new at version 2.0

#### `get_magnetic_spacegroup_type`

- Experimental: new at version 2.0

#### `get_magnetic_symmetry_from_database`

- Experimental: new at version 2.0

#### `get_hall_number_from_symmetry`

- Deprecated at version 2.0
- Will be removed at version 3.0
- Will be replaced by `get_spacegroup_type_from_symmetry`

#### `get_ir_reciprocal_mesh`

- Plan to make it deprecated at version 3.0
