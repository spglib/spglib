#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# shellcheck disable=all
source /usr/share/beakerlib/beakerlib.sh || exit 1

rlJournalStart
  rlPhaseStartSetup
    rlRun "tmp=\$(mktemp -d)" 0 "Create tmp directory"
	rlRun "rsync -rL ./ $tmp" 0 "Copy test-suite project"
    rlRun "pushd $tmp"
    rlRun "set -o pipefail"
  rlPhaseEnd

  rlPhaseStartTest
   	rlRun "cmake -B ./build -DSPGLIB_WITH_Fortran=ON" 0 "CMake configure"
   	rlRun "cmake --build ./build" 0 "CMake build"
   	rlRun "ctest --test-dir ./build --output-on-failure" 0 "Run CTest"
  rlPhaseEnd

  rlPhaseStartCleanup
    rlRun "popd"
    rlRun "rm -r $tmp" 0 "Remove tmp directory"
  rlPhaseEnd
rlJournalEnd
