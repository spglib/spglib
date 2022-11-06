# Releasing a new Spglib version

1. Update <doc/releases.md>
2. Increment version at <src/version.h>
3. Push the commit to `rc` branch (Github actions publishes the package to TestPyPI automatically)
4. Push the commit to `master` branch (Github actions publishes the package to PyPI automatically)
