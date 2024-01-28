#!/bin/sh
make clean && make
time -p ./Lab4 testcase/case1.in testcase/case.out
./SolutionChecker testcase/case1.in testcase/case.out