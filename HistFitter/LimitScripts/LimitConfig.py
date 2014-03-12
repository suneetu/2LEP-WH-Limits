################################################################
## LimitConfig.py for the Wh SS analysis                      ##
## author : A. S. Mete <amete@cern.ch>                        ##
## date   : 2014/2/24                                         ##
## In principle all you have to setup is defined in this file ##
################################################################

import sys

# This bit is necessary for the HistFitter stuff
from configManager import configMgr
from configWriter  import TopLevelXML,Measurement,ChannelXML,Sample
#from systematic    import Systematic

# This bit is necessary if we want to plot stuff
import ROOT
ROOT.gROOT.LoadMacro("./macros/AtlasStyle.C")
ROOT.SetAtlasStyle()

# User defined modules
sys.path.append("./LimitScripts")
from ConfigDefs      import *
from RuntimeOptions  import *
from SelectionConfig import *
from SystematicObject import *

# Read user input
SignalRegionChoices = ["allSRs","SR1jet","SR23jets"]
ChannelChoices      = ["allCHs","ee","mm","em"]
GridChoices         = ["SSWH"]
SysChoices          = ["NoSys","up","down" ] # For +/-1sigma theory bands

inputs        = sys.argv
_signalRegion = ""
_channel      = ""
_grid         = ""
_sigUncert    = ""

for inp in inputs:
    if inp in SignalRegionChoices: _signalRegion = inp
    elif inp in ChannelChoices:    _channel      = inp
    elif inp in GridChoices:       _grid         = inp
    elif inp in SysChoices:        _sigUncert    = inp


if _signalRegion == "":
    print "Signal region not found in inputs"
    print inputs
    sys.exit()

if _channel == "":
    print "Channel not found in inputs"
    print inputs
    sys.exit()

if _grid == "":
    print "Grid not found in inputs"
    print inputs
    sys.exit()

if _sigUncert == "":
    print "Signal uncertainty not found in inputs"
    print inputs
    sys.exit()

print "---------------------------------------------"
print "Signal region : ",_signalRegion
print "Channel       : ",_channel
print "Grid          : ",_grid
print "Uncert        : ",_sigUncert
print "---------------------------------------------\n"

# Input files w/ full path
indir      = "/gdata/atlas/suneetu/Documents/WHLimits2014/Inputs/20140310/Processed/"
bkgFile    = indir + "SSBG8TeV.root"
signalFile = indir + "SSWH8TeV.root"

# Set the analysis name
analysisName = _signalRegion+"_"+_channel+"_"+_grid

# Creat user defs instance
userDefs = ConfigDefs()

# Get the channel enum
# Let default be all
_ch = userDefs.allCHs 
if _channel in "ee": _ch = userDefs.ee
if _channel in "mm": _ch = userDefs.mm
if _channel in "em": _ch = userDefs.em

# Initialize Runtime Options
userOpts = RuntimeOptions(False,                    # doToys
                          "",                       # specify grid for toys
                          1000,                     # specify number of toys
                          True,                     # doExclusion
						  False,                    # doDiscovery
                          False,                    # doValidation
                          True,                     # Blind SR
                          True,                     # Blind CR
                          True,                     # Blind VR
                          _sigUncert,               # Specify uncertainty
                          _grid,                    # Specify grid
                          _signalRegion,            # Specify the signal region
                          _ch,                      # Lepton Channel
                          bkgFile,                  # input bkg file
                          signalFile,               # input signal file
                          analysisName,             # Analysis Name for saving
                          20.3,                     # Input Lumi units
                          20.3,                     # Ouput Lumi units
                          "fb-1"                    # Input Lumi units
                          )

# Define Data and BG samples
dataSample  = Sample("Data_CENTRAL", ROOT.kBlack   )
zjetsSample = Sample("Zjets"       , ROOT.kGreen+2 )
fakeSample  = Sample("Fakes"       , ROOT.kOrange-4)
higgsSample = Sample("Higgs"       , ROOT.kYellow  )
zvSample    = Sample("ZV"          , ROOT.kGreen   )
wwSample    = Sample("WW"          , ROOT.kAzure-4 )
topSample   = Sample("Top"         , ROOT.kViolet  )

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#
#                                 USER SHOULD NOT HAVE TO EDIT BELOW HERE                                    #
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#

#--------------------------------------------------------#
# Useful methods used below
#--------------------------------------------------------#
def userPrint(message):
    print "\tUSER MESSAGE: " + message
    return

#--------------------------------------------------------#
# Setup the various configurations
#--------------------------------------------------------#

################################################################################################
## Blinding regions 
################################################################################################
userPrint("Specifying the options for blinding SR/CR/VR")

configMgr.blindSR = userOpts.blindSR
configMgr.blindCR = userOpts.blindCR
configMgr.blindVR = userOpts.blindVR

################################################################################################
## Toys Vs. Asymtoptic
################################################################################################
userPrint("Setting up toys vs. Asymptotic options")

# calculatorType       => Calculator type      0: Freq., 1: Hybrid,   2: Asymptotic
if not userOpts.doToys:
    configMgr.calculatorType = 2  
else:
    configMgr.calculatorType = 0 
    configMgr.nTOYS = userOpts.nToys
    toyindex        = 2 
# testStatType         => Test statistic type, 0: LEP,  1: Tevatron, 2: Profile likelihood, 3: One sided PL
configMgr.testStatType = 3
# nPoints              => Number of scan points for the POI if the interval is fixed
configMgr.nPoints      = 20
# Write out XML        => Useful for cross analysis combination
configMgr.writeXML     = True 

################################################################################################
## Specify the Fit Mode
################################################################################################
userPrint("Specifying the fit mode")

if userOpts.doDiscovery: configMgr.doExclusion = False
if userOpts.doExclusion: configMgr.doExclusion = True

# One cannot do validation and exclusion at the same time, choose one 
if userOpts.doValidation and userOpts.doExclusion:
    userPrint("Can't do validation and Exclusion, setting validation to false")
    userOpts.doValidation = False

################################################################################################
## Specify output files 
################################################################################################
userPrint("Specifying the output files for HistFitter")

# Specify ananame
configMgr.analysisName = userOpts.anaName + "_" + userOpts.sigUncert

# Specify where hist will be sent
configMgr.histCacheFile = "data/" + configMgr.analysisName + ".root"

# Specify the output file based on whether we do toys or not
if not userOpts.doToys:
    configMgr.outputFileName = "results/"+configMgr.analysisName+"_Output.root"
else:
    configMgr.outputFileName = "results/"+configMgr.analysisName+"_"+userOpts.gridForToys+"_withToys"+userOpts.sigUncert+"_"+userOpts.signalRegion+"_"+str(toyindex)+"_Output.root"

################################################################################################
## Set the luminosity
################################################################################################
userPrint("Specifying Luminosity")

configMgr.inputLumi  = userOpts.inputLumi
configMgr.outputLumi = userOpts.outputLumi
configMgr.setLumiUnits( userOpts.lumiUnits )

################################################################################################
## Specify the background input files
################################################################################################
userPrint("Specifying background input")

bkgFiles = []
bkgFiles.append( userOpts.bkgFile )
configMgr.setFileList( bkgFiles )

################################################################################################
## Configure the selection
################################################################################################
userPrint("Configuring selection")

configMgr = selectionConfig(configMgr)

################################################################################################
## Configure weight
################################################################################################
userPrint("Sepcifying Weight")

configMgr.weights = ["eventweight","1.0"]

################################################################################################
## Setup the systematics
################################################################################################
userPrint("Configuring Systematic")

configMgr.nomName = "_CENTRAL"
sysObj = SystematicObject(configMgr) 

################################################################################################
## Setting up Samples and normalization factors
################################################################################################
userPrint("Setting up samples, norm factors and systematics")

# Specify the top level XML and the paramater of interest
tlx  = configMgr.addFitConfig("TopLvlXML")
meas = tlx.addMeasurement(name="NormalMeasurement",lumi=1.,lumiErr=0.028) # fractional luminosity error
meas.addPOI("mu_SIG")  ## EXCL:mu_SIG, upper limit table

# Determine if we should use stat
useStat = False # This is added as systematics below

# Add systematics here

# Stat
zjetsSample.setStatConfig(useStat)
fakeSample.setStatConfig(useStat)
higgsSample.setStatConfig(useStat) 
zvSample.setStatConfig(useStat)
wwSample.setStatConfig(useStat)   
topSample.setStatConfig(useStat)

if not useStat:
    zjetsSample.addSystematic(sysObj.AR_mcstat_Zjets)
    fakeSample.addSystematic(sysObj.AR_mcstat_Fakes)
    higgsSample.addSystematic(sysObj.AR_mcstat_Higgs) 
    zvSample.addSystematic(sysObj.AR_mcstat_ZV)   
    wwSample.addSystematic(sysObj.AR_mcstat_WW)   
    topSample.addSystematic(sysObj.AR_mcstat_TOP)
else:
    tlx.statErrThreshold = 0.001

# Rest should be implemented here
fakeSample.addSystematic(sysObj.AR_ELFR)
fakeSample.addSystematic(sysObj.AR_MUFR)
fakeSample.addSystematic(sysObj.AR_ELRE)
fakeSample.addSystematic(sysObj.AR_MURE)

mc_systematics = [sysObj.AR_EESZ, sysObj.AR_EER, sysObj.AR_EESLOW, sysObj.AR_EESMAT, sysObj.AR_EESPS,
                  sysObj.AR_JES, sysObj.AR_SCALEST, sysObj.AR_XS, sysObj.AR_BJET, sysObj.AR_BKGMETHOD, sysObj.AR_LUMI]

# Longer list for copy/paste
#mc_systematics = [sysObj.AR_EESZ, sysObj.AR_EER, sysObj.AR_EESLOW, sysObj.AR_EESMAT, sysObj.AR_EESPS,
#                  sysObj.AR_ID, sysObj.AR_JES, sysObj.AR_MS, sysObj.AR_SCALEST, sysObj.AR_LUMI, sysObj.AR_BJET,
#                  sysObj.AR_CJET, sysObj.AR_XS, sysObj.AR_BKGMETHOD]

mcsamples = [higgsSample, wwSample, topSample]
# Longer list for copy/paste
#mcsamples = [zjetsSample, higgsSample, zvSample, wwSample, topSample]
##### We've excluded zvSample and zjetsSample because of errors in the HFT's that cause HistFitter to fail
##### when those are included. This is OK to first order because zjetsSample mostly does not contribute
##### and zvSample has a large uncertainty applied "directly"


# execute systematic selection
for mc_sample_ in mcsamples:
    for systematic_ in mc_systematics:
        mc_sample_.addSystematic(systematic_)

# Set those that are MC-only so that luminosity uncertainty is passed on
zjetsSample.setNormByTheory()
higgsSample.setNormByTheory() 
zvSample.setNormByTheory()   
wwSample.setNormByTheory()   
topSample.setNormByTheory()

# Set data
dataSample.setData()

# Set samples list
tlx.addSamples([wwSample,topSample,zvSample,zjetsSample,higgsSample,dataSample,fakeSample])

################################################################################################
## Setup the signal regions
################################################################################################
userPrint("Setting up signal regions")

if userOpts.doExclusion or userOpts.doValidation:

    srList    = []
    srCounter = 0
    lepChan   = userOpts.leptonChannel
    SR        = userOpts.signalRegion

    channels = []
    if lepChan == userDefs.allCHs : channels = ['ee','mm','em']
    if lepChan == userDefs.ee     : channels = ['ee']
    if lepChan == userDefs.mm     : channels = ['mm']
    if lepChan == userDefs.em     : channels = ['em']

    srs = []
    if SR == 'allSRs'   : srs = ['SR1jet','SR23jets']
    if SR == 'SR1jet'   : srs = ['SR1jet']
    if SR == 'SR23jets' : srs = ['SR23jets']

    # Define simple function
    def addSR(type, channels, counter, srs, nbins, low, high):
        for sr in srs:
            for chan in channels:
                currentChannel = tlx.addChannel(type,[chan+sr],nbins,low,high)
                # Ad-hoc Uncertinties
                if 'SR1jet' in sr:
                    currentChannel.getSample("ZV").addSystematic(sysObj.AR_SR1jetAdhoc_ZV)
                elif 'SR23jets' in sr: 
                    currentChannel.getSample("ZV").addSystematic(sysObj.AR_SR23jetsAdhoc_ZV)
                srList.append( currentChannel )
                srList[counter].userOverflowBin =True
                srList[counter].userUnderflowBin=True
                counter+=1
        return counter

    # Add the channels
    srCounter = addSR("cuts",channels,srCounter,srs,1,0.5,1.5) 

    # One can define SR as VR but this might be redundant since we don't fit anything at the moment
    if userOpts.doValidation:
        tlx.setValidationChannels(srList)
    else:
        tlx.setSignalChannels(srList)

################################################################################################
## Do discovery fit
################################################################################################
userPrint("Setting up discovery fit section")

if userOpts.doDiscovery:

    if userOpts.leptonChannel == 'allCHs' or userOpts.signalRegion == 'allSRs':
        print 'One can run Discovery fit for only 1 SR 1 channel at once with the current configuration! Quitting...'
        sys.exit(0)
 
    srChanName = ""
    if userOpts.leptonChannel == userDefs.ee: srChanName += "ee"
    if userOpts.leptonChannel == userDefs.mm: srChanName += "mm"
    if userOpts.leptonChannel == userDefs.em: srChanName += "em"        
    srChanName += userOpts.signalRegion
    discoChannel = tlx.addChannel("cuts",[srChanName],1,0.5,1.5)
    discoChannel.addDiscoverySamples(["SIG"],[1.],[0.],[100.],[ROOT.kRed])
    # Ad-hoc Uncertinties
    if 'SR1jet' in srChanName:
        discoChannel.getSample("ZV").addSystematic(sysObj.AR_SR1jetAdhoc_ZV)
    elif 'SR23jets' in srChanName: 
        discoChannel.getSample("ZV").addSystematic(sysObj.AR_SR23jetsAdhoc_ZV)
    # Add stat - shouldn't be done - double check
    #discoChannel.addSystematic(sysObj.AR_mcstat_SIG)
    tlx.setSignalChannels(discoChannel)

       
################################################################################################
## Do discovery fit
################################################################################################
userPrint("Setting up discovery fit section")

if userOpts.doDiscovery:

    if userOpts.leptonChannel == 'allCHs' or userOpts.signalRegion == 'allSRs':
        print 'One can run Discovery fit for only 1 SR 1 channel at once with the current configuration! Quitting...'
        sys.exit(0)
 
    srChanName = ""
    if userOpts.leptonChannel == userDefs.ee: srChanName += "ee"
    if userOpts.leptonChannel == userDefs.mm: srChanName += "mm"

################################################################################################
## Do exclusion fit
################################################################################################
userPrint("Set the grid for exclusions")

sigSamples = []
if userOpts.doExclusion:

    if "SSWH" in userOpts.sigGrid: sigSamples = userDefs.getGrid(userOpts.sigGrid)
    else:
        userPrint("Could not determine a grid to run on.")
        userPrint("Exitting")
        sys.exit()

    # Now loop over signal samples
    userPrint("Looping over Signal Samples")
    for s in sigSamples:
        signame = s 

        # This bit needs cleaning - TO-DO
        doToys   = userOpts.doToys
        hasHippo = doToys and 'hippo' in userOpts.gridForToys
        hasSig   = doToys and signame in userOpts.gridForToys
        if not doToys or hasHippo or hasSig:
            exclusion = configMgr.addFitConfigClone(tlx,'TopLvlXML_Exclusion_%s'%s)
            print '!!!--- in LOOP ', s
            sigSample = Sample(s, ROOT.kRed)
            sigSample.setStatConfig(useStat)
            if not useStat:
                sigSample.addSystematic(sysObj.AR_mcstat_SIG)

            sigUncert = userOpts.sigUncert

            sigSample.setFileList([userOpts.sigFile])
            if sigUncert == "up":
                sigSample.setWeights(("eventweight","syst_XSUP","1.0"))
            elif sigUncert == "down":
                sigSample.setWeights(("eventweight","syst_XSDOWN","1.0"))

            #!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            # Add systematics here
            #!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
            # Set normalization factor
            sigSample.setNormFactor('mu_SIG',1.,0.,10.)

            # Add the sample to the exclusion
            exclusion.addSamples(sigSample)
            exclusion.setSignalSample(sigSample)

            print s, 'Leaving LOOP ---!!!'
