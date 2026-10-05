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
2. Before tagging, test the full release wheel matrix from GitHub's web interface:
   - Open the repository's **Actions** tab and select the [**CI** workflow](https://github.com/spglib/spglib/actions/workflows/ci.yaml)
     in the left sidebar.
   - Click **Run workflow**, select the release branch from the **Branch** dropdown, enter `*` in
     **Overwrite build targets**, and click **Run workflow** to start the run.
   - This uses the same test and wheel-building workflows as a release, generates package artifacts, and checks
     their metadata without uploading to PyPI or creating a GitHub release.
   - Ordinary pull-request CI only builds `cp311-*` wheels, so it does not cover the full release matrix.
   - Link the completed GitHub Actions run in the release PR and check that all build and package checks pass.
3. Push a corresponding tag, or notify the contributors of the request
   - For official release, push a tag with the appropriate version, e.g. `v2.1.0`.
     - This will update the release package on PyPI.
   - For pre-releases, include a `-rcX` suffix to the tag, e.g. `v2.1.0-rc1`.
     - This will publish a pre-release to PyPI, so it is not a dry run.
4. Notify the packaging contributors

Note that scikit-build-core does not yet support dynamic variables for the
updating `pyproject.toml` (follow progress at the upstream issues
[#172](https://github.com/scikit-build/scikit-build-core/issues/172) and
[#116](https://github.com/scikit-build/scikit-build-core/issues/116)).
Currently, the PyPI release versions and releases candidates is dynamically calculated from commit tag.
