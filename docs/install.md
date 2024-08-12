# How to install spglib C-API

## Download

The source code is downloaded at <https://github.com/spglib/spglib/releases> or you can
git-clone the spglib repository by

```
% git clone https://github.com/spglib/spglib.git
```

## Install

### Unix-like

After expanding source code, go into the source code directory:

```
% tar xvfz spglib-2.0.0.tar.gz
% cd spglib-2.0.0
```

Build and install in `_build` directory by

```bash
% mkdir _build
% cd _build
% cmake ..
% cmake --build .
% cmake --install (probably installed under /usr/local)
```

Or to install under the parent directory,

```bash
% mkdir _build
% cd _build
% cmake -DCMAKE_INSTALL_PREFIX=.. ..
% cmake --build .
% cmake --install . --prefix ..
```

### Windows & Cygwin

For windows, cmake behaves slightly differently and the following
way is recommended

```bash
% cmake -DCMAKE_INSTALL_PREFIX="$(shell cygpath -w "${BUILD_DIR}")" ..
% cmake --build . --config Release
% cmake --install . --config Release
```

The detail of the windows installation process is discussed in {issue}`118`.

## Usage

1. Include `spglib.h`
2. Link `libsymspg.a` or `libsymspg.so`
3. A compilation example is shown in {path}`example/README.md`.

## Example

A few examples are found in {path}`example` directory.
