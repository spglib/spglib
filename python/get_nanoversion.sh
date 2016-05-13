#!/bin/bash

br=`git branch |grep '^\*'|sed 's/^\(\*\s*\)//'`
read o u f <<< `git remote -v |grep origin |grep fetch`
echo "Repo: $o $u $br"
TD=`mktemp -d`
WD=`pwd`
git clone $u $TD
cd $TD
echo $u $br
git checkout $br
git describe --tags --dirty | sed -e 's/\([.0-9]*\)-\(.*\)-g.*/\2/' -e 's/^[vr]//g' >$WD/__nanoversion__.txt
cd $WD
rm -rf "$TD"
