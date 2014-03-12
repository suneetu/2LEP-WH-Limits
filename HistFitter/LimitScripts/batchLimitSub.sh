#!/bin/bash

#PBS -q atlas
#PBS -l nodes=1:ppn=14

# cd to working directory
cd $PBS_O_WORKDIR

# Setup ROOT
source setup_UCIT3.sh

# run setup script for limits so knows
# where to find HistFitter.py
source setup.sh

#run job
HistFitter.py -twfp LimitScripts/LimitConfig.py ${SR} ${Chan} ${Grid} ${Sys} > batchLog/${SR}_${Chan}_${Grid}_${Sys}.log
