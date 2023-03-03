#!/bin/bash

COMMAND="./tape-sorting"

echo "Run command: ${COMMAND}"

FAIL=0

# find line of max_numbers_in_memory and get number
LENGTH_OF_NUMBERS=$(grep -E "length ?= ?[0-9]+;" config.cfg | grep -E -o "[0-9]+")
LENGTH_OF_NUMBERS=$((LENGTH_OF_NUMBERS+1)) # for plus or minus

function run_test {
    # get count of lines in input file
    count_of_lines=$(wc -l < tests/"$1".in)
    touch tests/"$1".out
    for ((i=0; i < count_of_lines; i++)); do
        printf "%0${LENGTH_OF_NUMBERS}d\n" >> tests/"$1".out
    done
    # run 
    ${COMMAND} tests/"$1".in tests/"$1".out
    # check answer
    diff tests/"$1".out tests/"$1".sol
    if [ $? -ne 0 ]; then
        FAIL=1
        echo -e "\033[31mFAIL\033[0m"
    else
        echo -e "\033[32mPASS\033[0m"
    fi
    rm -f tests/"$1".out
}

for test_name in $(cat tests.txt); do
    test_name=$(echo $test_name)  # To remove \r from tests.txt on Windows
    echo ========= $test_name =========
    run_test "${test_name}"
done

if [[ "$FAIL" == "0" ]]; then
    echo -e "=========  \033[32;1mALL PASS\033[0m ========= "
else
    echo -e "=========  \033[31;1mSOME FAIL\033[0m ========= "
fi

exit $FAIL
