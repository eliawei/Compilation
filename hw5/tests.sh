#!/bin/bash

echo "Cleaning old build files"
rm -f lex.yy.c
rm -f parser.tab.*pp
rm -f hw5
echo "Building hw5"
flex scanner.lex
bison -d parser.ypp
g++ -std=c++11 -o hw5 *.c *.cpp

folders=(
	'segelTests' 'simpleSetTestDaniel' 'RoeesSetTests' 'prevSemester' 
	)
numtests=(
	'3' '1' '10' '24'
	)

for i in "${!folders[@]}"; do
	echo "Running tests from folder ${folders[i]}"
	echo "Number of tests = ${numtests[i]}"
	j="1"
	while [ $j -le ${numtests[i]} ]
	 	do
	# 		# dos2unix RoeesSetTests/t$i.out
	 		./hw5 < ${folders[i]}/t$j.in >& ${folders[i]}/t$j.ll
	 		lli ${folders[i]}/t$j.ll > ${folders[i]}/t$j.res
	 		diff ${folders[i]}/t$j.res ${folders[i]}/t$j.out &> /dev/null
	 		if [[ $? != 0 ]] 
	 			then
					tput setaf 1
	 				echo "Failed test #"$j" in folder ${folders[i]}!"
					tput sgr 0
	 				exit
	 		else
				tput setaf 2
				echo "Passed test number "$j""
				tput sgr 0
			fi
	 		j=$[$j+1]
	done
	rm ${folders[i]}/*.ll
	rm ${folders[i]}/*.res
done
tput setaf 1
tput setab 7
echo ""; echo ""
echo "emit(\"yaaaaaaaaaay\")"
tput sgr0
echo ""
