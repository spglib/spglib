# Working on spglib

Spglib is a C library for crystal and magnetic symmetry, with Python and Fortran
interfaces. Development and pull requests target `develop`. Read
`Contributing.md` for contribution and release procedures and `test/README.md`
for the test layout.

## Repository layout

- `src/` and `include/spglib.h`: C implementation and public API.
- `python/`: pybind11 bindings, Python package, and type stubs.
- `fortran/`: Fortran interface.
- `database/`: crystallographic source data and table generators.
- `test/`: native unit, functional, example, and packaging tests; Python tests
  live in `test/functional/python/`.
- `docs/`: Sphinx documentation. `ChangeLog.md` supplies the release notes.

## Build and validation

Python requires 3.11 or newer. Native builds require CMake >= 3.25, C and C++
compilers, and optionally a Fortran compiler. Run commands from the repository
root. Reuse a suitable environment when one already exists.

```sh
uv venv --python 3.11 .venv
uv pip install --python .venv/bin/python '.[test,docs]' setuptools
.venv/bin/python -m pytest

cmake -S . -B build -G Ninja -DSPGLIB_WITH_TESTS=ON -DSPGLIB_WITH_Fortran=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure

.venv/bin/sphinx-build -b html -W docs build/docs
prek run --all-files
```

- Reinstall the Python package after changing C or binding code so pytest uses
  the current sources. Use the environment's Python executable explicitly.
- Enable Fortran tests when a compiler is available; otherwise report that they
  were not run. `pre-commit run --all-files` is equivalent to the `prek` command.
- Run the full Python and native suites before opening or updating a PR. The
  normal pytest configuration excludes benchmarks; run those separately for
  performance work.
- Validate documentation changes with a strict Sphinx build. Regenerate API
  pages with `docs/generate-apidoc.py` when the public Python API changes.

## Scientific changes

- Preserve documented basis, origin, tolerance, and time-reversal conventions.
  Add regression tests with independently known expected results for fixes.
- When comparing symmetry operations, include translations modulo lattice
  vectors and magnetic time-reversal flags; do not rely on operation ordering.
- Keep database inputs and compiled tables consistent. Magnetic Hall changes in
  `database/msg/magnetic_hall_symbols.yaml` require regenerating the affected
  tables in `src/msg_database.c` with `database/msg/make_mhall_db.py`. That script
  emits tables, not a complete replacement C file.
- Update the corresponding bindings, type stubs, and documentation when changing
  a public API. Record user-visible changes under `Unreleased` in `ChangeLog.md`.

## Releases

- Versions come from Git tags through CMake and setuptools-scm. Do not hard-code
  a version or commit generated `python/spglib/_version.py`.
- Audit changes since the previous release before finalizing the changelog.
- Ordinary PR CI builds only `cp311-*` wheels. Before a release, run the full
  matrix on the pushed branch and link the completed run in the PR:
  `gh workflow run ci.yaml --ref <branch> -f 'cibw_build=*'`.
- The manual CI run tests and checks package artifacts without publishing.
  Release and release-candidate tags trigger PyPI publication; creating a
  release-preparation PR does not authorize pushing a release tag.
