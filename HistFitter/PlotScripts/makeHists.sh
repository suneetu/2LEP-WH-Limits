#!/bin/bash

#---------------------------#
# Options
#---------------------------#

SignalRegions=(allSRs)         # Options: SR1jet SR23jets allSRs
Channels=(allCHs)              # Options: ee mm em allCHs
Systematics=(NoSys up down)    # Options: NoSys up down
Grid="SSWH"                    # Options: SSWH 
#nFiles=N                      # If jobs are sub-divided 


#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#
#           LOOP TO MAKE THE HISTOGRAMS                  #
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#

for sr in ${SignalRegions[@]}; do
    for ch in ${Channels[@]}; do
	for sys in ${Systematics[@]}; do
	
	    options='"'${sr}'","'${ch}'","'${Grid}'","'${sys} #,"'${LHRH}'",'${nFiles}
	    command="macros/makeLimitHists.C("${options}")"
	    root -l -b -q ${command}

	done
    done
done

