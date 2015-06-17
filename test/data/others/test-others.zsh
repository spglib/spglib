#!/bin/zsh

export LD_LIBRARY_PATH=`pwd`/../../../src/.libs
export RUBYLIB=`pwd`/../..

findsym=$1
# this is special check using findsym
if [ x$findsym = "xfindsym" ]; then
	export ISODATA=~/tools/isotropy/
fi

for i in `/bin/ls */CONTCAR*`;do
	spg=`ruby ../../symPoscar.rb -s 0.1 -n --shift="0.1 0.2 0.3" $i`
	numspg=`echo $spg|awk -F"(" '{print $2}'|sed s/\)//`
	numposcar=`echo $i|awk -F"/" '{print $2}'|cut -c 8-10|awk '{print $1*1}'`

	if [ x$findsym = "xfindsym" ]; then
		numfindsym=`../poscar2findsym.rb $i | $ISODATA/findsym|grep Space|awk '{print $3}'`
	fi

	if [ x$findsym = "xfindsym" ]; then
		if [ $numspg = $numfindsym ]; then
			echo " ..." $spg '(ok)' 
		else
			echo " ..." $spg ' fail (' $numfindsym ')'
		fi
	else
		echo $i $spg 
	fi
done

cat spg/README

