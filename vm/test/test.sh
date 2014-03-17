#!/bin/bash

TEST_DIR=`dirname "${BASH_SOURCE[0]}"`
TARGET_DIR=$TEST_DIR/../target

COMPILER=$TEST_DIR/../../compiler/main.py
EXECUTABLE=$TARGET_DIR/vm

COL_END="\e[0m"
COL_RED="\e[0;31m"
COL_YELLOW="\e[0;33m"
COL_GREEN="\e[0;32m"

tup upd
if [ $? -ne 0 ]; then
    echo -e ${COL_RED}Compilation failure, no tests ran.$COL_END
    exit $?
fi

TEMP_DIR=`mktemp -d tmp.XXX`
trap cleanup EXIT

function tmp_file() {
    mktemp --tmpdir="$TEMP_DIR"
}

function execute() {
    PROGRAM=$1
    EXPECTED_OUTPUT=$2
    CASE=$3
    EXPECTED_EXIT=$4

    echo Running test case $CASE
    # echo Should produce 
    # cat $EXPECTED_OUTPUT

    OUTPUT=`tmp_file`
    STRIPPED_OUTPUT=`tmp_file`

    cat "$PROGRAM" | $EXECUTABLE 2>&1 | tee $OUTPUT | sed "s/^/\[$CASE\]: /"
    RESULT=${PIPESTATUS[1]}

    cat $OUTPUT | grep -v "^\[trace\] .*$" > $STRIPPED_OUTPUT

    if [ $RESULT -ne $EXPECTED_EXIT ]; then
        echo -e ${COL_RED}ERROR:$COL_END $CASE ended with $RESULT. Expected $EXPECTED_EXIT.
        ERRORS=$(($ERRORS + 1))
    else
        colordiff -u $STRIPPED_OUTPUT $EXPECTED_OUTPUT | tail -n +3
        if [ ${PIPESTATUS[0]} -ne 0 ]; then
            echo -e ^ "${COL_YELLOW}FAILURE:$COL_END $CASE doesn't match. Expected output marked with +"
            FAILURES=$(($FAILURES + 1))
        else
            echo -e $CASE: ${COL_GREEN}SUCCESS$COL_END
        fi
    fi
}

function cleanup() {
    rm -r $TEMP_DIR
}

function test_case() {
    CASE=$1

    csplit "$CASE" "/SHOULD PRODUCE/+2" -n 1 -f $TEMP_DIR/x > /dev/null

    CODE=`tmp_file`
    OUTPUT=`tmp_file`
    COMPILED=`tmp_file`

    DELIMITER_LINE=`tail -n -2 $TEMP_DIR/x0`

    EXPECTED_EXIT=`echo $DELIMITER_LINE | grep -oE 'WITH CODE [0-9]+' | sed "s/.*\([0-9]\+\).*/\1/"`
    if [ -z $EXPECTED_EXIT ]; then
        EXPECTED_EXIT=0
    fi

    head -n -3 $TEMP_DIR/x0 > $CODE
    mv $TEMP_DIR/x1 $OUTPUT

    cat $CODE | $COMPILER > $COMPILED
    
    execute $COMPILED $OUTPUT $CASE $EXPECTED_EXIT
}

if [ $# -eq 0 ]; then
    ARGS=`ls *.case`
    N_CASES=`ls *.case | wc -l`
else
    ARGS=$*
    N_CASES=$#
fi

ERRORS=0
FAILURES=0

echo Will run $N_CASES tests.

for CASE in $ARGS; do
    test_case $CASE
done

if [ $ERRORS -eq 0 -a $FAILURES -eq 0 ]; then
    echo -e ${COL_GREEN}SUCCESS \($N_CASES tests\)$COL_END
else
    echo -e ${COL_RED}$ERRORS errors, $FAILURES failures out of $N_CASES tests$COL_END
    exit 1
fi

