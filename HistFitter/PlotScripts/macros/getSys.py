
## Want to run Yields table on all of the signal points
## and then parse the output so that we can get the
## systematic.

import sys
import os
from subprocess import *
import ROOT
from ROOT import *

######################################################
# Specify some command line options and read in here #
######################################################

#------------------#
# Expected
#------------------#

SignalRegions = ["SR4a","SR4b","SR4c"]
Grids         = ["SMCwslep","SparseDLiSlep"]
Handedness    = ["ROnly", "LOnly"]
Channels      = ["tot","ee","mm"]

inputs = sys.argv

SR   = ""
Grid = ""
Hand = ""
Ch   = ""

for inp in inputs:
    if inp in SignalRegions: SR   = inp
    elif inp in Grids:       Grid = inp
    elif inp in Handedness:  Hand = inp
    elif inp in Channels:    Ch   = inp
    
if SR == "":
    print "Signal region not supported"
    print inputs
    print SignalRegions
    sys.exit()

if Grid == "":
    print "Grid not found"
    print inputs
    print Grids
    sys.exit()

if Ch == "":
    print "Channel not found"
    print inputs
    print Channels
    sys.exit()


#------------------#
# Output file name
#------------------#
append = ""
if Hand != "": append += "_" + Hand
outname = "data/" + Grid + append + "_" + SR + "_" + Ch + ".txt"
outfile = open(outname, "w")

######################################################
# Load the points to look at given the Grid name     #
######################################################

def loadPoints(grid):
    fname = ""
    if grid == "SMCwslep": fname = "data/ModeC_mC1_mN1.txt"
    else:                  fname = "data/Slepton_mSl_mN_Sparse.txt"

    points = []
    inFile = open(fname,"r")
    for line in inFile:
        list = line.split(" ")
        mass = list[0] + "_"
        mass = mass + (list[1].split("\n"))[0]
        points.append( mass)

    return points

masses = loadPoints(Grid)

######################################################
# Now create a list of the potential directires that #
# the files could be in.  Loop over that directory   #
# and see if the file is there. If yes, get it and   #
# save the point in the output file.                 #
######################################################

def getFilesInDir(Path):

    ls = Popen(['ls ' + Path + '/* | grep Exclusion | grep afterFit.root'],shell=True,stdout=PIPE)
    ls.wait()
    list = ls.stdout.read().split("\n")
    del list[-1]
    return list

def makeList(sr, grid, ch):
    masterList  = []
    inDir       = "../results/"
    nFiles      = 0

    # Decide Grid
    if grid == "SMCwslep": nFiles = 5
    else:                  nFiles = 14

    # Loop and add to the list
    for i in range(nFiles):

        # Get the path to the directory
        path = inDir+sr+"_"+ch+"_"+grid+str(i)+Hand+"_NoSys/"
        print "Trying ", path

        # Find the files in that dir
        tempList = getFilesInDir(path)

        # Append to master list
        for entry in tempList:
            masterList.append( entry )

    return masterList

fileList = makeList(SR, Grid, Ch)

######################################################
# Need a way to subtract out the statistical error.  #
# There doesn't appear to be a convenient way, so we #
# will read in the stat error from histogram stored  #
# under HistFitter/data/                             #
######################################################

def makeStatFileList(sr, grid, ch):
    masterList = []
    inDir      = "../data/"
    nFiles     = 0

    # Decide Grid
    if grid == "SMCwslep": nFiles = 5
    else:                  nFiles = 14

    # Loop and add to the list
    for i in range(nFiles):

        # Get the path to the directory
        path = inDir+sr+"_"+ch+"_"+grid+str(i)+Hand+"_NoSys.root"
        print "Trying ", path
        
        # Append to master list
        masterList.append( path )

    return masterList

statFileList = makeStatFileList(SR, Grid, Ch)

######################################################
# Have the list of files now, need to extract the    #
# systematic information and write it to the final   #
# output text file so that we can read it in to the  #
# other plotting macro                               #
######################################################

def makeTable(grid, sr, M, file):

    gridName = ""
    if grid == "SMCwslep": gridName = "SMCwslep8TeV"
    else                 : gridName = "DLiSlep"

    command = "YieldsTable.py -y -b "
    command += " -c " + sr
    command += " -s " + gridName+"_"+M
    command += " -w " + file
    command += " -o temp.tex"

    makingTable = Popen([command],shell=True,stdout=PIPE)
    makingTable.wait()
    return

def deleteTable():
    Popen(["rm temp.tex"],shell=True)
    return

def splitAsymmetricError(line):
    val_err = []
    line = line.split("_")
    val_err.append(float(line[0]))
    line = line[1].split("{")
    lower = float(line[1].split("}")[0])
    upper = float(line[2].split("}")[0])
    error = abs(lower)
    if error < abs(upper):
        error = abs(upper)
    val_err.append(error)
    return val_err

def getSys(grid, M):
    sysInput = open("temp.tex","r")

    gridName = ""
    if grid == "SMCwslep": gridName = "SMCwslep8TeV"
    else                 : gridName = "DLiSlep"

    lineWithSys = ""
    find = gridName+"_"+M
    for line in sysInput:
        if find in line and "MC exp." in line:
            lineWithSys = line
    lineWithSys = lineWithSys.split("$")[1]
    lineWithSys = lineWithSys.split("\pm")

    nEvents = 0
    nErr    = 0
    if len(lineWithSys) != 2:
        evt_err = splitAsymmetricError(lineWithSys[0])
        nEvents = evt_err[0]
        nErr    = evt_err[1]
    else:
        nEvents = float(lineWithSys[0])
        nErr    = float(lineWithSys[1])

    #print "\t\tFrom Sys: " + str(nEvents) + " +/- " + str(nErr)
    
    if nEvents == 0: return 0
    return (nErr/nEvents) * 100

def getStat(filelists, sr, grid, ch, M):

    # Specify grid name
    gridName = ""
    if grid == "SMCwslep": gridName = "SMCwslep8TeV"
    else                 : gridName = "DLiSlep"

    # Construct histogram name
    histName = "h"+gridName+"_"+M+"Nom_"+ch+sr+"_obs_cuts"

    # Counters
    bc  = 0
    err = 0

    # Loop over input files
    for fname in filelists:
        inputFile = TFile(fname)
        
        if inputFile.FindObjectAny(histName):
            hist = inputFile.Get(histName) 
            bc   = hist.GetBinContent(1)
            err  = hist.GetBinError(1)

    #print "\t\tFrom Stat: " + str(bc) + " +/- " + str(err)
    if bc == 0: return 0
    else:        return (err / bc) * 100

    

# Loop over points
for mass in masses:

    # Get Total error
    syst = 0
    for iF in range(len(fileList)):
        currentFile = fileList[iF]

        if mass not in currentFile: continue

        makeTable(Grid, SR, mass, currentFile)
        syst = getSys(Grid, mass)
        deleteTable()


    # Get Stat Erro    
    stat = getStat(statFileList, SR, Grid, Ch, mass)
    #print "Masses: " + mass + " sys: " + str(syst) + " stat: " + str(stat)

    # Add a catch to deal with the floating point precision
    # extracted from YieldsTable, which will round to second
    # decimal.
    if syst < stat: syst = 0
    if syst !=0:    syst = sqrt( syst*syst - stat*stat )
    #print "\tUpdated: " + str(syst)
    m1 = mass.split("_")[0]
    m2 = mass.split("_")[1]
    outfile.write(m1 + " " + m2 + " " + str(syst) + "\n")
    
