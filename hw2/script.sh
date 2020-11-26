#!/bin/bash
# Compile
make
i=1
test_path="./hw2-tests"
for test in "$test_path"/*.in;do
  echo "Running test "$i
  ./hw2 < "$test" >& $test_path/t$i.myout
  diff_res="$(diff $test_path/t$i.out $test_path/t$i.myout)"
  if [[ -n "$diff_res" ]]; then
    echo "FAIL";
  else
    echo "PASS";
  fi
  echo "$diff_res">&diff_$(date +%H_%M)
  i=$((i+1))
done