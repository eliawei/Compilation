rm ./*.res
#	number of tests:
numtests=50
#	command to execute test:
command="./hw3 < ./tests/1/t\$i.in >& t\$i.res"
i="1"
while [ $i -le $numtests ]
	do
		./hw3 < ./tests/1/t$i.in >& t$i.res
		diff t$i.res ./tests/1/t$i.out &> /dev/null
		if [[ $? != 0 ]] 
			then
				echo "Failed test #"$i"! (folder #1)"
				exit
		fi
		i=$[$i+1]
done

#	number of tests:
numtests=49
#	command to execute test:
command="./hw3 < ./tests/2/t\$i.in >& t\$i.res"
i="1"
while [ $i -le $numtests ]
	do
		eval $command
		diff t$i.res ./tests/2/t$i.out &> /dev/null
		if [[ $? != 0 ]] 
			then
				echo "Failed test #"$i"! (folder #2)"
				exit
		fi
		i=$[$i+1]
done
