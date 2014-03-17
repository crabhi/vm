#!/bin/bash

CASES_DIR=`dirname "${BASH_SOURCE[0]}"`

. ../test.base

EXECUTABLE=$TEST_DIR/../target/loader

run_tests
