!/bin/bash

case "${READTHEDOCS_VERSION_TYPE}" in
"tag")
  python3 scripts/generate_changelog.py --type tag --version ${READTHEDOCS_VERSION}
  ;;
"external")
  python3 scripts/generate_changelog.py --type pull-request
  ;;
*)
  python3 scripts/generate_changelog.py --type release
  ;;
esac
