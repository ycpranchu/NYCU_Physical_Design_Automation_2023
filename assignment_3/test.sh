#!/bin/sh
make clean && make
time -p ./Lab3 ASYNC_DFFHx1_ASAP7_75t_R.sp sample.out
./SolutionChecker ASYNC_DFFHx1_ASAP7_75t_R.sp sample.out