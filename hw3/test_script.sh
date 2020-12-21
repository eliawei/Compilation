rm ./tests/*/*.res
#	number of tests:
numtests=50
#	command to execute test:
passed=0
failed=0
failed_arr_1="1: "
command="./hw3 < ./tests/1/t\$i.in >& ./tests/1/t\$i.res"
i="1"

while [ $i -le $numtests ]
	do
		eval $command
		diff ./tests/1/t$i.res ./tests/1/t$i.out &> /dev/null
		if [[ $? != 0 ]] 
			then
				echo "Failed test #"$i"! (folder #1)"
				((failed++))
				failed_arr_1+="$i "
		else
				echo "Passed test #"$i"! (folder #1)"
				((passed++))

		fi
		i=$[$i+1]
done

#	number of tests:
numtests=49
failed_arr_2="2: "
#	command to execute test:
command="./hw3 < ./tests/2/t\$i.in >& ./tests/2/t\$i.res"
i="1"
while [ $i -le $numtests ]
	do
		eval $command
		diff ./tests/2/t$i.res ./tests/2/t$i.out &> /dev/null
		if [[ $? != 0 ]] 
			then
				echo "Failed test #"$i"! (folder #2)"
				((failed++))
				failed_arr_2+="$i "

		else
				echo "Passed test #"$i"! (folder #2)"
				((passed++))

		fi
		i=$[$i+1]
done

echo "passed: $passed / 99"
echo "failed: $failed / 99"

echo $failed_arr_1
echo $failed_arr_2
