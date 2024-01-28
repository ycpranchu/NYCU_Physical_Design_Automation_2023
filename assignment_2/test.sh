#!/bin/sh
make clean && make
python3 case_generator.py

time -p ./Floorplan sample.in sample.out
./SolutionChecker sample.in sample.out

python3 check.py
