#!/bin/bash

SRs=(SR4a SR4b SR4c)
#Samples=(SMCwslep SparaseDLiSlep)
Samples=(SparseDLiSlep)

for SR in ${SRs[@]}; do
    for Sample in ${Samples[@]}; do

	python macros/dumpTable.py ${SR} ${Sample}

    done
done