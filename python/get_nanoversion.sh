#!/bin/bash

br=`git branch |tail -n1`
read o u f <<< `git remote -v |grep origin |grep fetch`
echo "Repo: $o $u $br"

# This doesn't work well for rc because it is supposed tag is not pushed yet.
git describe --tags --dirty

TD=`mktemp -d`
WD=`pwd`

# full clone is necessary to count number of commits after tag.
echo "Run 'git clone $u $TD'"
git clone --depth 1 $u $TD
cd $TD
echo $u $br
git checkout $br
git describe --tags --dirty | sed -e 's/\([.0-9]*\)-\(.*\)-g.*/\2/' -e 's/^[vr]//g' >$WD/__nanoversion__.txt
cd $WD
rm -rf "$TD"
