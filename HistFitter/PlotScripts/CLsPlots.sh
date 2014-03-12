#!/bin/bash


#Mode C
root -l -b -q 'macros/PlotLimits.C("all",SR_4,1,1,0,1,3)'

#Slepton
root -l -b -q 'macros/PlotLimits.C("all",SR_4,1,1,1,1,3)'

#Slepton RH
root -l -b -q 'macros/PlotLimits.C("all",SR_4,1,1,2,1,3)'

#Slepton LH
root -l -b -q 'macros/PlotLimits.C("all",SR_4,1,1,3,1,3)'

#Selectron
root -l -b -q 'macros/PlotLimits.C("ee",SR_4,1,1,4,1,3)'

#Selectron RH
root -l -b -q 'macros/PlotLimits.C("ee",SR_4,1,1,5,1,3)'

#Selectron LH
root -l -b -q 'macros/PlotLimits.C("ee",SR_4,1,1,6,1,3)'

#Smuon
root -l -b -q 'macros/PlotLimits.C("mm",SR_4,1,1,7,1,3)'

#Smuon RH
root -l -b -q 'macros/PlotLimits.C("mm",SR_4,1,1,8,1,3)'

#Smuon LH
root -l -b -q 'macros/PlotLimits.C("mm",SR_4,1,1,9,1,3)'

