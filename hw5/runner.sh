#!/bin/bash
cd ~/Compilation/hw5
make
for test in ./hw5tests/*.in; do
  test_num=$(echo $test | cut -d '/' -f 3 | cut -d '.' -f 1)  
  ./hw5<$test > llvm_file.ll
  lli llvm_file.ll > ./hw5tests/my_$test_num.out      	
  result="PASS"
  diff ./hw5tests/test_num.out ./hw5tests/my_$test_num.out &> /dev/null; rtrn=$?
  if [[ $rtrn != 0 ]]; then
    result="FAIL";
  fi
  echo "Test $test_num: $result";
done
