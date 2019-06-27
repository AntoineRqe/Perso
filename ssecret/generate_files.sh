#!/bin/bash

DIR_PATH=$1

mkdir --verbose -p ${DIR_PATH}
rm -rf ${DIR_PATH}/*

count=0

while true
do
    echo "-2 2 0 1 -1 -2 5 8 3 -2 0 6 -5 10" >> ${DIR_PATH}/test_${count}.txt
    count+=1
    sleep 1
done
