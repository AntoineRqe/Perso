#!/bin/bash

DIR_PATH=$1

mkdir --verbose -p ${DIR_PATH}
rm -rf ${DIR_PATH}/*

count=0

while true
do
    if [ -f ${DIR_PATH}/test_${count}.txt ]
    then
        rm ${DIR_PATH}/test_${count}.txt
    fi
    echo -e '-2\n2\n0\n1\n-1\n-2\n5\n8\n3\n-2\n0\n6\n-5\n10' >> ${DIR_PATH}/test_${count}.txt
    echo "generated file test_${count}.txt"
    ((count++))
    sleep 0.01
done
