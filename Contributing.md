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

1. Update [`ChangeLog.md`](ChangeLog.md)
2. Push a corresponding tag, or notify the contributors of the request
   - For official release, push a tag with the appropriate version, e.g. `v2.1.0`.
     - This will update the release package on PyPI.
   - For pre-releases, include a `-rcX` suffix to the tag, e.g. `v2.1.0-rc1`.
     - This will update the package on TestPyPI automatically
   - For testing, you can push a tag with suffix `-test` to your personal fork, or create a PR to `test-PyPi-action`
     branch
     - This will perform the wheel building tests that we use for release. Note that these builds are very
       time-consuming, so we encourage you to use the tag approach to trigger these tests.
     - Link in the PR the most recent Github Action run
     - If you are using the PR approach to `test-PyPi-action` approach, make sure to change the target branch at the
       end
     - This will not attempt to upload to PyPI, but the artifacts will still be generated.
3. Notify the packaging contributors

Note that scikit-build-core does not yet support dynamic variables for the
updating `pyproject.toml` (follow progress at the upstream issues
[#172](https://github.com/scikit-build/scikit-build-core/issues/172) and
[#116](https://github.com/scikit-build/scikit-build-core/issues/116)).
Currently, the PyPI release versions and releases candidates is dynamically calculated from commit tag.
