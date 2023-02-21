The development of spglib is managed on the `develop` branch of github spglib
repository.

- Github issues is the place to discuss about spglib issues.
- Github pull request is the place to request merging source code.

## Local checks

We provide integration with [`pre-commit`](https://pre-commit.com/) to ensure
some good practices before submitting a PR. We encourage contributors to set
it up on their local environment, e.g.:
```console
$ pip install pre-commit
$ pre-commit install
```

This installs pre-commit hooks, so make sure you set it up before committing.
Alternatively, you can manually run it for debug purposes:
```console
$ pre-commit run --all-files
```

Make sure you have necessary tools installed (`clang-format`,`clang-tidy`), e.g.
```console
# dnf install clang-tools-extra
```

Consult [`.pre-commit-config.yaml`](.pre-commit-config.yaml) for advanced
standard changes.

## Releasing a new Spglib version

1. Update <doc/releases.md>
2. Increment version at <CMakeLists.txt>
3. Push the commit to `rc` branch (Github actions publishes the package to TestPyPI automatically)
4. Push the commit to `master` branch (Github actions publishes the package to PyPI automatically)

Note that scikit-build-core does not yet support dynamic variables for the
updating `pyproject.toml` (follow progress at the upstream issues
[#172](https://github.com/scikit-build/scikit-build-core/issues/172) and
[#116](https://github.com/scikit-build/scikit-build-core/issues/116)).
Currently, the Pypi release versions and releases candidates have to be
manually updated in the version field of the [`pyproject.toml`](pyproject.toml)
file.
