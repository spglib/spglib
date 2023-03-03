#!/usr/bin/env python3

from argparse import ArgumentParser
from jinja2 import Environment, FileSystemLoader
from packaging.version import Version
import pathlib
import yaml

# Path of the repository source
repo_path = pathlib.Path(__file__).parent.parent.resolve()
# Files to generate. Must have an associated f"{file}.j2" jinja template file
files_to_generate = ["ChangeLog.md"]


def is_release_candidate(val: str, version: str = "") -> bool:
    """
    Jinja test to check if `val` is a release candidate of `version`. If no `version` is passed, checks if `val` is
    release candidate only.

    :param val: value to test
    :param version: release version
    :return: whether `val` is a rc or `version`
    """
    val = Version(val)
    if not version:
        return val.is_prerelease
    version = Version(version)
    if str(version) != val.base_version:
        # `val` is not a related to `version`
        return False
    # Otherwise just check if "rc" is in the text
    return val.is_prerelease


def run(src, out, changelog, type, release):
    with open(changelog) as fil:
        data = yaml.load(fil, Loader=yaml.Loader)
    # Make sure entries are sorted
    data['releases'] = sorted(data['releases'], key=lambda r: Version(r['version']))
    if 'roadmap' in data:
        data['roadmap'] = sorted(data['roadmap'], key=lambda r: Version(r['version'].replace('x', '0')))
    if not release and data['releases']:
        release = data['releases'][-1]['version']
    data['type'] = type
    data['release'] = release
    jinja = Environment(loader=FileSystemLoader(src),
                        # Defaults of Ansible
                        trim_blocks=True, lstrip_blocks=False)
    jinja.tests['release_candidate'] = is_release_candidate
    for file in files_to_generate:
        # Relative path to `src` handled by `FileSystemLoader(src)`
        template = jinja.get_template(f"{file}.j2")
        with open(f"{out}/{file}", "w+") as fil:
            fil.write(template.render(data))


if __name__ == "__main__":
    parser = ArgumentParser(description="Generate changelogs (doc/releases.md, ChangeLog.md) from ChangeLog.yaml")
    parser.add_argument("--type", help="Documentation type to build releases.md for",
                        choices=["local", "tag", "external"], default="local")
    parser.add_argument("--release", help="The most recent release to include",
                        type=str)
    parser.add_argument("--src", help="Location of the source",
                        type=str, default=repo_path)
    parser.add_argument("--output", help="Path to output generated items",
                        type=str, default=repo_path)
    parser.add_argument("--changelog", help="Location of the changelog.yaml",
                        type=str, default=f"{repo_path}/ChangeLog.yaml")
    args = parser.parse_args()
    run(src=args.src, out=args.output,
        changelog=args.changelog,
        type=args.type, release=args.release)
