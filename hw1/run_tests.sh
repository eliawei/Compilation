# Compile
flex scanner.lex
g++ -std=c++11 lex.yy.c hw1.cpp -o hw1.out

# Run staff tests
TEST_NUMBER=2
for i in $(seq 1 $TEST_NUMBER)
do
  echo "Running staff test "$i
  ./hw1.out < tests/t"$i".in >& tests/t"$i".myout
  diff tests/t"$i".out tests/t"$i".myout
done

# Run "my" tests
TEST_NUMBER=4
for i in $(seq 1 $TEST_NUMBER)
do
  echo "Running \"my\" test "$i
  ./hw1.out < tests/my_test"$i".in >& tests/my_test"$i".myout
  diff tests/my_test"$i".out tests/my_test"$i".myout
done

# Run "good" tests
TEST_NUMBER=18
for i in $(seq 1 $TEST_NUMBER)
do
  echo "Running \"good\" test "$i
  ./hw1.out < tests/good_test"$i".in >& tests/good_test"$i".myout
  diff tests/good_test"$i".out tests/good_test"$i".myout
done

