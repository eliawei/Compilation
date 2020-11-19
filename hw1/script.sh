#!/bin/bash
# Compile
flex scanner.lex
g++ -std=c++11 lex.yy.c hw1.cpp -o hw1.out
TEST_NUMBER=11
for i in $(seq 1 $TEST_NUMBER)
do
  echo "Running extra test "$i
  ./hw1.out < ./New\ folder/Ours"$i".in >& ./New\ folder/Ours"$i".myout
  diff ./New\ folder/Ours"$i".out ./New\ folder/Ours"$i".myout
done