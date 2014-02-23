#!/bin/bash

TEST_DIR=`dirname "${BASH_SOURCE[0]}"`
TARGET_DIR=$TEST_DIR/../target

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
    echo Running test case $CASE
    # echo Should produce 
    # cat $EXPECTED_OUTPUT

    OUTPUT=`tmp_file`
    STRIPPED_OUTPUT=`tmp_file`

    cat "$PROGRAM" | $EXECUTABLE | tee $OUTPUT | sed "s/^/\[$CASE\]: /"
    RESULT=${PIPESTATUS[1]}

    cat $OUTPUT | grep -v "^\[trace\] .*$" > $STRIPPED_OUTPUT

    if [ $RESULT -ne 0 ]; then
        echo -e ${COL_RED}ERROR:$COL_END $CASE ended with $RESULT.
    else
        colordiff -u $STRIPPED_OUTPUT $EXPECTED_OUTPUT | tail -n +3
        if [ ${PIPESTATUS[0]} -ne 0 ]; then
            echo -e ^ "${COL_YELLOW}FAILURE:$COL_END $CASE doesn't match. Expected output marked with +"
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

    head -n -3 $TEMP_DIR/x0 > $CODE
    mv $TEMP_DIR/x1 $OUTPUT
    
    execute $CODE $OUTPUT $CASE
}

if [ $# -eq 0 ]; then
    ARGS=`ls *.case`
else
    ARGS=$*
fi

echo Tests to run: $ARGS
for CASE in $ARGS; do
    test_case $CASE
done
