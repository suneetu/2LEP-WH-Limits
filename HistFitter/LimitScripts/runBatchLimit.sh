#!/bin/bash

# Stolen form Matt

############################################
#               RUN OPTIONS                #
############################################

Grids=(SSWH)                # Currently only 1 grid is setup
Channels=(allCHs)           # ee mm em allCHs
SignalRegions=(allSRs)      # SR1jet SR23jets allSRs
Systematics=(NoSys up down) # NoSys up down (NoSys up down)

############################################
#         Below will submit jobs           #
############################################

for sr in ${SignalRegions[@]}; do
    for ch in ${Channels[@]}; do
	for grid in ${Grids[@]}; do
	    for sys in ${Systematics[@]}; do
		    name=${sr}_${ch}_${grid}_${sys}
		    qsub -j oe -V -v SR=${sr},Chan=${ch},Grid=${grid},Sys=${sys} -N ${name} -o batchlog LimitScripts/batchLimitSub.sh
		    sleep 0.5s
	    done
	done
    done
done

echo "Finished Submitting"
echo
