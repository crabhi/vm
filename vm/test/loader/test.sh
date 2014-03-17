#!/bin/bash

. ../test.base

CASES_DIR=`dirname "${BASH_SOURCE[0]}"`
EXECUTABLE=$TEST_DIR/../target/loader

run_tests $*
