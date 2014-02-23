#!/bin/bash

TEST_DIR=`dirname "${BASH_SOURCE[0]}"`
TARGET_DIR=$TEST_DIR/../target

EXECUTABLE=$TARGET_DIR/vm

tup upd

TEMP_DIR=`mktemp -d tmp.XXX`
trap cleanup EXIT

function tmp_file() {
    mktemp --tmpdir="$TEMP_DIR"
}

COL_END="\e[0m"
COL_RED="\e[0;31m"
COL_YELLOW="\e[0;33m"

function execute() {
    PROGRAM=$1
    EXPECTED_OUTPUT=$2
    CASE=$3
    echo Running test case $CASE

    OUTPUT=`tmp_file`

    cat "$PROGRAM" | $EXECUTABLE | tee $OUTPUT | sed "s/^/\[$CASE\]: /"
    RESULT=${PIPESTATUS[1]}


    if [ $RESULT -ne 0 ]; then
        echo -e ${COL_RED}ERROR:$COL_END $CASE ended with $RESULT.
    else
        colordiff -u $OUTPUT $EXPECTED_OUTPUT | tail -n +3
        if [ ${PIPESTATUS[0]} -ne 0 ]; then
            echo -e ^ "${COL_YELLOW}FAILURE:$COL_END $CASE doesn't match. Expected output marked with +"
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
