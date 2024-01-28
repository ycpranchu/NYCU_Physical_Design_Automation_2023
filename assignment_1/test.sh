#!/bin/sh
make clean && make
time -p ./Lab1 case4.in case4.out
./SolutionChecker case4.in case4.out
