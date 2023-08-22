#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# shellcheck disable=all
source /usr/share/beakerlib/beakerlib.sh || exit 1

rlJournalStart
    rlPhaseStartSetup
        rlRun "tmp=\$(mktemp -d)" 0 "Create tmp directory"
		rlRun "rsync -r .$TMT_TEST_NAME/ $tmp" 0 "Copy example project"
        rlRun "pushd $tmp"
        rlRun "tree" 0 "Show directory tree"
        rlRun "set -o pipefail"
    rlPhaseEnd

    rlPhaseStartTest
    	rlRun "cmake -B ./build"
    	rlRun "cmake --build ./build"
    	rlRun "./build/example"
    	rlRun "./build/example_full"
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "popd"
        rlRun "rm -r $tmp" 0 "Remove tmp directory"
    rlPhaseEnd
rlJournalEnd
