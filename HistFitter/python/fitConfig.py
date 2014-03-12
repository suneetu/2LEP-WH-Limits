import ROOT
from ROOT import TFile, TMath, RooRandom, TH1, TH1F
from ROOT import kBlack, kWhite, kGray, kRed, kPink, kMagenta, kViolet, kBlue, kAzure, kCyan, kTeal, kGreen, kSpring, kYellow, kOrange, kDashed, kSolid, kDotted
from measurement import Measurement
from channel import Channel
from sample import Sample
from logger import Logger
from systematic import SystematicBase

log = Logger('fitConfig')

import os, errno

TH1.SetDefaultSumw2(True)

from copy import deepcopy
from configManager import configMgr

def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise

class fitConfig(object):
    """
    Defines the content of a top-level HistFactory workspace
    """

    def __init__(self, name):
        """
        Set the combination mode, store the configuration and open output file
        """
        self.ConstructorInit(name)

        # attributes to below are OK to deepcopy
        self.mode = "comb"
        self.statErrThreshold = None #None means to turn OFF mcStat error
        self.statErrorType = "Gaussian"
        self.measurements = []
        self.channels = []
        self.sampleList = []
        self.signalSample = None
        self.signalChannels = []
        self.validationChannels = []
        self.bkgConstrainChannels = []
        self.systDict = {}
        self.files = []
        self.weights = []
        self.treeName = ''
        self.hasDiscovery = False

        # Plot cosmetics
        self.dataColor = kBlack
        self.totalPdfColor = kBlue
        self.errorLineColor = kBlue-5
        self.errorLineStyle = kSolid
        self.errorFillColor = kBlue-5
        self.errorFillStyle = 3004
        self.setLogy = False
        self.tLegend = None
        self.hypoTestName = ""

    def Clone(self, newName=""):
        if newName == "":
            newName = self.name
        #copies all properties prior to initialize
        newTLX = deepcopy(self)
        newTLX.ConstructorInit(newName)
        for chan in newTLX.channels:
            chan.ConstructorInit(newTLX.prefix)
        return newTLX

    def ConstructorInit(self, name):
        #shared method between __init__ and Clone
        self.name = name
        mkdir_p('./results/%s' % configMgr.analysisName)
        mkdir_p('./config/%s' % configMgr.analysisName)
        mkdir_p('./data/%s' % configMgr.analysisName)
        self.prefix = configMgr.analysisName + "/" + self.name
        self.xmlFileName = "N/A"
        self.wsFileName = "N/A"
        return

    def initialize(self):
        self.xmlFileName = "config/"+self.prefix+".xml"
        
        #Note: wsFileName is an educated guess of the workspace
        # file name externally decided by HistFactory.
        self.wsFileName = "results/%s_combined_%s_model.root" % (self.prefix, self.measurements[0].name)
       
        for sam in self.sampleList:
            if sam.isData: # FIXME (works but ugly)
                self.sampleList.remove(sam)       #Just making sure that Data is the last element of the list
                self.sampleList.append(sam)
                break
        
        #Consistency checks
        if not self.signalSample is None:
            found = False

            for s in self.sampleList:
                if s.name == self.signalSample:
                    found = True

            if not found:
                for chan in self.channels:
                    for s in chan.sampleList:
                        if s.name == self.signalSample:
                            found = True

                if not found:
                    log.warning("signal sample %s is not contained in sampleList of TopLvlXML %s or its daughter channels" % (self.signalSample, self.name))

        for chan in self.channels:
            chanName = chan.channelName
            isSR = False
            isCR = False
            isVR = False
            nFound = 0

            if self.signalChannels.__contains__(chanName):
                isSR = True
                nFound += 1

            if self.bkgConstrainChannels.__contains__(chanName):
                isCR = True
                nFound += 1

            if self.validationChannels.__contains__(chanName):
                isVR = True
                nFound += 1

            if nFound == 0:
                log.warning("TopLvlXML: %s, Channel: %s --> SR/CR/VR undefined" % (self.name, chanName))

            if nFound > 1:
                log.warning("TopLvlXML: %s, Channel: %s --> SR=%s CR=%s VR=%s is ambiguous" % (self.name, chanName, isSR, isCR, isVR))

            #for sample in self.sampleList:
            #    try:
            #        chan.getSample(sample.name)
            #    except:
            #        chan.addSample(sample)
            #    for (systName, syst) in sample.systDict.items():
            #        try:
            #            chan.getSample(sample.name).getSystematic(systName)
            #        except:
            #            for s in sample.getSystematic(systName):
            #                chan.getSample(sample.name).addSystematic(s)
        return

    def writeWorkspaces(self):
        channelObjects = []
        for chan in self.channels:
            c = chan.createHistFactoryObject()
            #add channel to some array to use below, all channels by default belong to all measurements
            channelObjects.append(c)
        
        for meas in self.measurements:
            m = meas.createHistFactoryObject(self.prefix)

            for chan in channelObjects:
                m.AddChannel(chan)

            m.CollectHistograms()
            
            # can be used to compare to our own XML if necessary
            #m.PrintXML("xmlFromPy/"+self.prefix, m.GetOutputFilePrefix())
            
            #NB this function's name is deceiving - does not run fits unless m.exportOnly=False
            ROOT.RooStats.HistFactory.MakeModelAndMeasurementFast(m)
        
        return

    def close(self):
        """
        Write workspace to file and close
        """
        self.writeWorkspaces()
        return

    def addMeasurement(self, name, lumi, lumiErr):
        #verify that this name is not already used
        for meas in self.measurements:
            if meas.name == name:
                raise RuntimeError("Measurement %s already exists in TopLevelXML %s. Please use a different name." % (name, self.name))
            pass
        #add measurement to the list
        self.measurements.append(Measurement(name, lumi, lumiErr))
        return self.measurements[len(self.measurements) - 1]

    def addMeasurementObj(self, obj):
        if not isinstance(obj, Measurement):
            raise RuntimeError("addMeasurement does not support input of type '%s'." % (type(obj)))
        #verify that this name is not already used
        for meas in self.measurements:
            if meas.name == obj.name:
                raise RuntimeError("Measurement %s already exists in TopLevelXML %s. Please use a different name." % (obj.name, self.name))
            pass
        #add measurement clone to the list
        self.measurements.append(obj.Clone())
        return self.measurements[len(self.measurements) - 1]

    def getMeasurement(self, name):
        """
        Find the measurement object with given name
        """
        for m in self.measurements:
            if m.name == name:
                return m

        raise RuntimeError("Measurement %s does not exist in %s" % (name, self.name))

    def statStatErrorType(self, t):
        """
        Set stat error type for config, and propagate down to channels
        """
        self.statErrorType = t
        for chan in self.channels:
            chan.statErrorType = t

    def addChannel(self, variableName, regions, nBins, binLow, binHigh):
        """
        Build a channel object from this TopLevel
        """
        if variableName == "cuts":
            nBins = len(regions)
            #binLow = 0.5
            binHigh = nBins + binLow
            pass
        chanObj = Channel(variableName, regions, self.prefix, nBins,
                             binLow, binHigh, self.statErrThreshold)

        # Verify that this name is not already used
        for chan in self.channels:
            if chan.name == chanObj.name:
                log.info("Not gonna add the region, because it exists in fitConfig --> channel-List follows:" )
                for chan in self.channels:
                    print "     chan.name = ", chan.name
                raise RuntimeError("Channel %s already exists in TopLevelXML %s. Please use a different name." % (chanObj.name, self.name))

        #set channel parent
        chanObj.parentTopLvl = self

        #set stat error type
        chanObj.statErrorType = self.statErrorType

        # Channel doesn't have weights so add them
        chanObj.setWeights(self.weights)

        # Propagate systematics into channel
        for (systName, syst) in self.systDict.items():
            chanObj.addSystematic(syst)

        # Put samples owned by this TopLevel into the channel
        for s in self.sampleList:
            chanObj.addSample(s.Clone())

        # Add channel to the list
        self.channels.append(chanObj)

        return self.channels[len(self.channels) - 1]

    def addChannelObj(self, obj):
        """
        Add channel as a pre-built object
        """
        if not isinstance(obj, Channel):
            raise RuntimeError("addChannel does not support input of type '%s'." % (type(obj)))

        # Verify that this name is not already used
        for chan in self.channels:
            if chan.name == obj.name:
                raise RuntimeError("Channel %s already exists in TopLevelXML %s. Please use a different name." % (obj.name, self.name))

        # Create a copy
        newObj = deepcopy(obj)

        #reset channel parent
        newObj.parentTopLvl = self

        newObj.statErrorType = self.statErrorType

        # If the channel doesn't have any weights then add them
        if len(newObj.weights) == 0:
            newObj.weights.setWeights(self.weights)

        # Propagate systematics into channel
        for (systName, syst) in self.systDict.items():
            if not systName in newObj.systDict.keys():
                newObj.addSystematic(syst)

        # Put samples owned by this TopLevel into the channel
        for s in self.sampleList:
            if not s.name in [sam.name for sam in newObj.sampleList]:
                newObj.addSample(s)

        # Add channel clone to the list
        self.channels.append(newObj)
        return self.channels[len(self.channels) - 1]

    def addValidationChannel(self, variableName, regions, nBins,
                             binLow, binHigh):
        """
        Create a channel and give it a validation flag
        """
        ch = self.addChannel(variableName, regions, nBins, binLow, binHigh)
        self.setValidationChannels(ch)
        return ch


    def getChannelByName(self, name):
        """
        Find the channel with the given name
        """
        for chan in self.channels:
            if chan.name == name:
                return chan

        raise RuntimeError("No channel with name %s found" % name)


    def getChannel(self, variableName, regions):
        """
        Find the channel with the given variable and regions
        """
        for chan in self.channels:
            if chan.variableName == variableName and chan.regions == regions:
                return chan

        raise RuntimeError("No channel with variable name %s and regions %s found" % (variableName, regions))

    def addSamples(self, input):
        """
        Add list (or single object) of pre-built samples to this TopLevel
        """
        if isinstance(input, list):
            sampleList = input
        else:
            sampleList = [input]
            pass

        for s in sampleList:
            # If the sample doesn't exist in TopLevel already then add it,
            # else something has gone wrong
            if not s.name in [sam.name for sam in self.sampleList]:

                # Append copy of the sample
                self.sampleList.append(s.Clone())

                # Only apply weights and systematics to MC-derived samples
                if not s.isData and not s.isDiscovery and not s.isQCD:
                    # If the sample doesn't have weights then add them
                    if len(self.sampleList[-1].weights) == 0:
                        self.sampleList[-1].setWeights(self.weights)

                    # Propagate systematics into sample
                    for (systName, syst) in self.systDict.items():
                        if not systName in self.sampleList[-1].systDict.keys():
                            self.sampleList[-1].addSystematic(syst)

            else:
                raise RuntimeError("Sample %s already defined in TopLevel %s" % (s.name, self.name))

            # Propagate to channels that are already owned as well
            for c in self.channels:
                if not s.name in [sam.name for sam in c.sampleList]:
                    c.addSample(self.getSample(s.name))
        return

    def getSample(self, name):
        """
        Find the sample with the given name
        """
        for s in self.sampleList:
            if s.name == name:
                return s
        raise Exception("Sample with name %s not found in TopLevel %s" % (name, self.name))

    def setWeights(self, weights):
        """
        Set the weights
        This overrides all previously defined weights
        """
        self.weights = deepcopy(weights)

        # Propagate to owned channels that do not already have weights
        for c in self.channels:
            if len(c.weights) == 0:
                c.setWeights(weights)

        # Propagate to owned samples that do not already have weights
        for s in self.sampleList:
            if not s.isData and not s.isDiscovery and not s.isQCD:
                if len(s.weights) == 0:
                    s.setWeights(weights)

        return

    def addWeight(self, weight):
        """
        Add a single weight
        """
        if not weight in self.weights:
            self.weights.append(weight)
        else:
            raise RuntimeError("Weight %s already defined in fitConfig %s" % (weight, self.name))

        # Propagate to owned channels that do not already have this weight
        for c in self.channels:
            if not weight in c.weights:
                c.addWeight(weight)

        # Propagate to owned samples that do not already have this weight
        for s in self.sampleList:
            if not s.isData and not s.isDiscovery and not s.isQCD:
                if not weight in s.weights:
                    s.addWeight(weight)

        # Propagate to owned weight-type systematics that do not
        # already have this weight
        for syst in self.systDict.values():
            if syst.type == "weight":
                if not weight in syst.high:
                    syst.high.append(weight)
                if not weight in syst.low:
                    syst.low.append(weight)
        return

    def removeWeight(self, weight):
        """
        Remove a single weight
        """
        if weight in self.weights:
            self.weights.remove(weight)
        else:
            raise RuntimeError("Weight %s does not exist in TopLevel %s" % (weight, self.name))

        # Propagate to owned channels
        for c in self.channels:
            if weight in c.weights:
                c.removeWeight(weight)

        # Propagate to owned samples
        for s in self.sampleList:
            if not s.isData and not s.isDiscovery and not s.isQCD:
                if weight in s.weights:
                    s.removeWeight(weight)

        # Propagate to owned weight-type systematics
        for syst in self.systDict.values():
            if syst.type == "weight":
                if weight in syst.high:
                    syst.high.remove(weight)
                if weight in syst.low:
                    syst.low.remove(weight)
        return

    def setSignalSample(self, sig):
        """
        Flag the signal sample
        """
        if isinstance(sig, Sample):
            self.signalSample = sig.name
        elif isinstance(sig, str):
            self.signalSample = sig
        else:
            raise RuntimeError("setSignalSample does not support type %s" % (type(sig)))
        return

    def appendStrChanOrListToList(self, input, targetList):
        #little utility function
        if isinstance(input, list):
            inList = input
        else:
            inList = [input]
            pass
        for i in inList:
            if isinstance(i, Channel):
                chanName = i.channelName
            else:
                chanName = i
                pass
            if not targetList.__contains__(chanName):
                targetList.append(chanName)
                pass
            pass
        return

    def setSignalChannels(self, channels):
        """
        Set the channels to be treated as signal (SRs)
        """
        self.appendStrChanOrListToList(channels, self.signalChannels)
        return

    def setBkgConstrainChannels(self, channels):
        """
        Set the channels to be treated as constraining regions (CRs)
        """
        self.appendStrChanOrListToList(channels, self.bkgConstrainChannels)
        return

    def setValidationChannels(self, channels):
        #TODO should be renamed appendValidationChannels !
        """
        Set the channels to be treated as validation regions (VRs)
        """
        self.appendStrChanOrListToList(channels, self.validationChannels)
        return

    def setFileList(self, filelist):
        """
        Set file list for this top level xml.
        This will be used as default for channels that don't specify
        their own file list.
        """
        self.files = filelist
        return

    def setFile(self, file):
        """
        Set file for this top level xml.
        This will be used as default for channels that don't specify
        their own file list.
        """
        self.files = [file]
        return

    def propagateFileList(self, fileList):
        """
        Propagate the file list downwards.
        """
        # if we don't have our own file list, use the one given to us
        if not self.files:
            self.files = fileList

        # propagate our file list downwards
        for ch in self.channels:
            ch.propagateFileList(self.files)

        return

    def addSystematic(self, syst):
        """
        Add a systematic to this object.
        This will be propagated to all owned samples
        """
        if syst.name in self.systDict.keys():
            raise RuntimeError("Attempt to overwrite systematic %s in TopLevel %s" % (syst.name, self.name))
        else:
            self.systDict[syst.name] = syst.Clone()

            for chan in self.channels:
                chan.addSystematic(syst)

            for sam in self.sampleList:
                if not sam.isData and not sam.isDiscovery and not sam.isQCD:
                    sam.addSystematic(syst)

        return

    def getSystematic(self, systName):
        """
        Find the systematic with given name
        """
        # protection against strange people who pass a systematic to this function
        name = systName
        if isinstance(systName, SystematicBase):
            name = systName.name
        
        try:
            return self.systDict[name]
        except KeyError:
            raise KeyError("Could not find systematic %s in topLevel %s" % (name, self.name))

    def clearSystematics(self):
        """
        Remove all systematics from this TopLevel
        """
        self.systDict.clear()
        return

    def removeSystematic(self, systName):
        """
        Remove a single systematic from this TopLevel
        """

        # allow removal using the object too
        name = systName
        if isinstance(systName, SystematicBase):
            name = systName.name
        
        del self.systDict[name]
        return

    def setTreeName(self, treeName):
        """
        Set the tree name
        """
        self.treeName = treeName
        return

    def propagateTreeName(self, treeName):
        """
        Propagate the tree name
        """
        if self.treeName == '':
            self.treeName = treeName
        ## propagate down to channels
        for chan in self.channels:
            chan.propagateTreeName(self.treeName)
            pass
        return

    def __str__(self):
        """
        Convert instance to XML string
        """
        self.writeString = "<!DOCTYPE Combination  SYSTEM '../HistFactorySchema.dtd'>\n\n"
        self.writeString += "<Combination OutputFilePrefix=\"./results/" + self.prefix + "\"  >\n\n"
        
        for chan in self.channels:
            self.writeString += "  <Input>" + chan.xmlFileName + "</Input>\n"
        
        self.writeString += "\n"
        
        for meas in self.measurements:
            self.writeString += str(meas)
        
        self.writeString += "</Combination>\n"
        
        return self.writeString

    def writeXML(self):
        """
        Write instance to XML file and close
        """
        log.info("Writing file: '%s'" % self.xmlFileName)
        log.verbose("Following will be written:")
        log.verbose(str(self))
        
        self.xmlFile = open(self.xmlFileName, "w")
        self.xmlFile.write(str(self))
        self.xmlFile.close()
        for chan in self.channels:
            log.verbose("Following channel XML will be written:")    
            log.verbose(str(chan))
            chan.writeXML()
        return

    def executehist2workspace(self, option=""):
        """
        Run hist2workspace binary for the XML file for this fitConfig
        """
        cmd = "hist2workspace "
        if len(option):
            cmd += "-" + option

        cmd += self.xmlFileName

        log.info("Executing: '%s'" % cmd)
        p = os.popen(cmd)
        line = p.readline()
        while 1:
            line = p.readline()
            if not line: break
            log.debug(line.strip())

        p.close()

        log.info("Created workspace 'combined' in file '%s'\n" % self.wsFileName)
        return

