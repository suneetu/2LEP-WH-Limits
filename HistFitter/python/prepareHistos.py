from ROOT import gROOT,TFile,TH1F,Double,gDirectory,TChain,TObject
from math import sqrt
from logger import Logger

log = Logger('PrepareHistos')

class PrepareHistosABC(object):
    """
    Abstract base class to define histogram preparation methods
    """

    def __init__(self):
        """
        All implementations initialized the same
        """
        from configManager import configMgr
        self.configMgr = configMgr
        self.var = ""
        self.cuts = ""
        self.weights = ""
        self.channel = None

        self.cutDict = {}
        self.cutList = []
        self.histList = []
        self.nameList = []

    def checkTree(self,treeName,fileList):
        """
        Check tree name in file list
        """
        raise NotImplementedError("The checkTree method must be implemented "+str(self.__class__.__name__))

    def read(self,rootName,fileList):
        """
        Read in the root object that will make histograms
        """
        raise NotImplementedError("The read method must be implemented "+str(self.__class__.__name__))

    def addHisto(self,name,reg,isCut,nBins,binLow,binHigh,nBinsY=0,binLowY=0.,binHighY=0.):
        """
        Make histogram and add it to the dictionary of prepared histograms
        """
        raise NotImplementedError("The addHisto method must be implemented "+str(self.__class__.__name__))

    def addQCDHistos(self):
        """
        Make the nominal QCD histogram and its errors 
        """
        raise NotImplementedError("The qcdHistos method must be implemented "+str(self.__class__.__name__))

    def updateHistBin(self, h, binIn, binOver):
        newVal = h.GetBinContent(binIn) + h.GetBinContent(binOver)
        h.SetBinContent(binIn,newVal)
        h.SetBinContent(binOver,0.0)
        e1=h.GetBinError(binIn)
        e2=h.GetBinError(binOver)
        newErr=sqrt(e1*e1 + e2*e2)
        h.SetBinError(binIn,newErr)
        h.SetBinError(binOver,0.0)
        return

    def updateOverflowBins(self, h, useOverflow, useUnderflow):
        if useOverflow:
            binIn = h.GetNbinsX()
            binOver = binIn+1
            self.updateHistBin(h, binIn, binOver)

        if useUnderflow:
            binIn = 1
            binOver = 0
            self.updateHistBin(h, binIn, binOver)

        return

class TreePrepare(PrepareHistosABC):
    """
    Implementation of the PrepareHistosABC

    Generate the histograms from a set of TTrees

    Supports merging of multiple input files
    """
    def __init__(self):
        PrepareHistosABC.__init__(self)
        self.currentChainName=''

    def checkTree(self, treeName, fileList):
        """
        Check existence of tree. True=ok, False=none. 
        """
        if len(fileList)==0 or len(treeName)==0: 
            return False
        
        for f in fileList:
            file = TFile.Open(f)
            if file is None:
                continue

            tree = file.Get(treeName)
            if not tree or tree is None:
                file.Close()
                continue

            if tree is not None and tree.ClassName() != 'TTree':
                file.Close()
                continue

            file.Close()
            return True

        return False

    def read(self,treeName,fileList):
        """
        Set the chain name and add input files
        """
        if treeName=='':
            raise Exception("Empty tree name provided for reading input root files.")

        if not self.currentChainName=='' and not (self.currentChainName.find(treeName)>-1):
            del self.configMgr.chains[self.currentChainName]

        chainID = treeName
        for fileName in fileList: chainID += '_' +fileName
        self.currentChainName = chainID
        
        ## MB : no need to recreate chain if it already exists
        if not self.configMgr.chains.has_key(chainID):
            self.configMgr.chains[chainID] = TChain(treeName)
            for fileName in fileList:
                self.configMgr.chains[self.currentChainName].Add(fileName)

            #### To add non-existent branches, follow examples like these
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("mtmu2"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("mtmu2","( sqrt(2*lep2Pt*met*(1-cos(lep2Phi-metPhi))) )")

            ##if not self.configMgr.chains[self.currentChainName].GetBranch("phQuality"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("phQuality","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("phIso"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("phIso","(1>0)")

            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt0GeVWeight"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt0GeVWeight","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt50GeVWeight"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt50GeVWeight","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt100GeVWeight"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt100GeVWeight","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt150GeVWeight"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt150GeVWeight","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt200GeVWeight"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt200GeVWeight","(1>0)")

            ### up 
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt0GeVWeightUp"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt0GeVWeightUp","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt50GeVWeightUp"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt50GeVWeightUp","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt100GeVWeightUp"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt100GeVWeightUp","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt150GeVWeightUp"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt150GeVWeightUp","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt200GeVWeightUp"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt200GeVWeightUp","(1>0)")

            ### down 
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt0GeVWeightDown"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt0GeVWeightDown","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt50GeVWeightDown"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt50GeVWeightDown","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt100GeVWeightDown"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt100GeVWeightDown","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt150GeVWeightDown"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt150GeVWeightDown","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("truthZpt200GeVWeightDown"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("truthZpt200GeVWeightDown","(1>0)")

            ##if not self.configMgr.chains[self.currentChainName].GetBranch("hfWeightUp"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("hfWeightUp","(1>0)")
            ##if not self.configMgr.chains[self.currentChainName].GetBranch("hfWeightDown"):
            ##    self.configMgr.chains[self.currentChainName].SetAlias("hfWeightDown","(1>0)")

        return

    def addHisto(self, name, nBins=0, binLow=0., binHigh=0., nBinsY=0, binLowY=0., binHighY=0., useOverflow=False, useUnderflow=False):
        """
        Use the TTree::Draw method to create the histograms for var from cuts and weights defined in instance

        Recover from ROOT memory and add to dictionary of histograms
        """
        if self.var == "cuts":
            if self.configMgr.hists[name] is None:
                self.configMgr.hists[name] = TH1F(name, name, len(self.channel.regions), self.channel.binLow, float(len(self.channel.regions))+self.channel.binLow)
                for (iReg,reg) in enumerate(self.channel.regions):
                    self.cuts = self.configMgr.cutsDict[reg]
                    
                    tempName = "%stemp%s" % (name, str(iReg))
                    tempHist = TH1F(tempName, tempName, 1, 0.5, 1.5)
                    self.configMgr.chains[self.currentChainName].Project(tempName, self.cuts, self.weights)
                    
                    error = Double()
                    integral = tempHist.IntegralAndError(1, tempHist.GetNbinsX(), error)
                    self.configMgr.hists[name].SetBinContent(iReg+1, integral)
                    self.configMgr.hists[name].SetBinError(iReg+1, error)
                    self.configMgr.hists[name].GetXaxis().SetBinLabel(iReg+1, reg)
                    tempHist.Delete()

                    for iBin in xrange(1, self.configMgr.hists[name].GetNbinsX()+1):
                        binVal = self.configMgr.hists[name].GetBinContent(iBin)
                        binErr = self.configMgr.hists[name].GetBinError(iBin)
                        if binVal < 0.0:
                            self.configMgr.hists[name].SetBinContent(iBin, 0.0)

        else:
            if self.configMgr.hists[name] is None:
                if self.var.find(":") == -1:
                    self.configMgr.hists[name] = TH1F(name, name, self.channel.nBins, self.channel.binLow, self.channel.binHigh)
                else:
                    self.configMgr.hists[name] = TH2F(name, name, self.channel.nBins, self.channel.binLow, self.channel.binHigh, self.channelnBinsY, self.channel.binLowY, self.channel.binHighY)
                for (iReg,reg) in enumerate(self.channel.regions):
                    tempName = "%stemp%s" % (name, str(iReg))
                    #self.cuts = self.configMgr.cutsDict[reg]
                    if self.var.find(":") == -1:                    
                        tempHist = TH1F(tempName, tempName, self.channel.nBins, self.channel.binLow, self.channel.binHigh)
                    else:
                        tempHist = TH2F(tempName, tempName, self.channel.nBins, self.channel.binLow, self.channel.binHigh, self.channelnBinsY, self.channel.binLowY, self.channel.binHighY)
                    #print "!!!!!! PROJECTING",name+"temp"+str(iReg)
                    #print "!!!!!! VAR",self.var
                    #print "!!!!!! WEIGHTS",self.weights
                    #print "!!!!!! CUTS",self.cuts
                    nCuts = self.configMgr.chains[self.currentChainName].Project(tempName, self.var, self.weights+" * ("+self.cuts+")")
                    self.configMgr.hists[name].Add(tempHist.Clone())
                    tempHist.Delete()

                    for iBin in xrange(1, self.configMgr.hists[name].GetNbinsX()+1):
                        binVal = self.configMgr.hists[name].GetBinContent(iBin)
                        binErr = self.configMgr.hists[name].GetBinError(iBin)
                        if binVal < 0.:
                            self.configMgr.hists[name].SetBinContent(iBin, 0.)
                        #if binErr==0:
                        #    self.configMgr.hists[name].SetBinError(iBin,1E-8)

        self.name = name

        #Over/Underflow bins
        if useOverflow or useUnderflow:
            self.updateOverflowBins(self.configMgr.hists[name], useOverflow, useUnderflow)
            
        return self.configMgr.hists[name]

    def addQCDHistos(self, sample, useOverflow=False, useUnderflow=False):
        """
        Make the nominal QCD histogram and its up and down fluctuations
        """
        regString = "".join(self.channel.regions)

        prefixNom = "h%sNom_%s_obs_%s" % (sample.name, regString, self.channel.variableName.replace("/","") )
        prefixHigh = "h%sHigh_%s_obs_%s" % (sample.name, regString, self.channel.variableName.replace("/","") )
        prefixLow = "h%sLow_%s_obs_%s" % (sample.name, regString, self.channel.variableName.replace("/","") )
        
        if self.channel.hasBQCD:
            self.weights = self.configMgr.weightsQCDWithB
            weightsQCD = self.configMgr.weightsQCDWithB
        else:
            self.weights = self.configMgr.weightsQCD
            weightsQCD = self.configMgr.weightsQCD
        self.addHisto(prefixNom)
        self.addHisto(prefixHigh)
        self.addHisto(prefixLow)
        
        systName = "%sSyst" % self.name
        statName = "%sStat" % self.name
        qcdHistoSyst = TH1F(systName, systName, self.channel.nBins, self.channel.binLow, self.channel.binHigh)
        qcdHistoStat = TH1F(statName, statName, self.channel.nBins, self.channel.binLow, self.channel.binHigh)
        
        if self.var == "cuts":
            for (iReg,reg) in enumerate(self.channel.regions):
                if self.configMgr.hists[prefixNom+"_"+str(iReg+1)] is None:
                    tempNameSyst = "%sSyst%s" % (self.name, str(iReg+1))
                    qcdHistoSystTemp = TH1F(tempNameSyst, tempNameSyst, self.channel.nBins, self.channel.binLow, self.channel.binHigh)
                    self.configMgr.chains[self.currentChainName].Project(tempNameSyst, self.configMgr.cutsDict[reg], self.weights+"Syst")
                    qcdHistoSyst.SetBinContent(iReg+1,qcdHistoSystTemp.GetBinContent(1))
                    
                    tempNameStat = "%sStat%s" % (self.name, str(iReg+1))
                    qcdHistoStatTemp = TH1F(tempNameStat, tempNameStat, self.channel.nBins, self.channel.binLow, self.channel.binHigh)
                    self.configMgr.chains[self.currentChainName].Project(tempNameStat, self.configMgr.cutsDict[reg], self.weights+"Stat")
                    qcdHistoStat.SetBinContent(iReg+1, qcdHistoStatTemp.GetBinContent(1))
        else:
            if self.weights == "1.0":
                sysWeightStat = "0.01" #rough average of Dan's results
                sysWeightSyst = "0.25" #rough average of Dan's results
            else:
                sysWeightStat = self.weights+"Stat"
                sysWeightSyst = self.weights+"Syst"
                
            if self.configMgr.hists[prefixNom+"_"+str(1)] is None:
                self.configMgr.chains[self.currentChainName].Project(systName, self.var, sysWeightSyst+" * ("+self.cuts+")")
                self.configMgr.chains[self.currentChainName].Project(statName, self.var, sysWeightStat+" * ("+self.cuts+")")

        ## correct nominal bins (not overflow)
        for iBin in xrange(1,self.configMgr.hists[prefixNom].GetNbinsX()+1):
            #
            if self.configMgr.hists[prefixNom+"_"+str(iBin)] is None:
                if self.channel.variableName == "cuts":
                    self.configMgr.hists[prefixNom+"_"+str(iBin)] = TH1F(prefixNom+"_"+str(iBin),prefixNom+"_"+str(iBin),len(self.channel.regions),self.channel.binLow,float(len(self.channel.regions))+self.channel.binLow)
                else:
                    self.configMgr.hists[prefixNom+"_"+str(iBin)] = TH1F(prefixNom+"_"+str(iBin),prefixNom+"_"+str(iBin),self.channel.nBins,self.channel.binLow,self.channel.binHigh)

                binVal = self.configMgr.hists[prefixNom].GetBinContent(iBin)
                #binError = sqrt(qcdHistoSyst.GetBinContent(iBin)**2+qcdHistoStat.GetBinContent(iBin)**2)
                #binStatError = qcdHistoStat.GetBinContent(iBin)
                if qcdHistoStat.GetBinContent(iBin)<-1*qcdHistoSyst.GetBinContent(iBin)**2: # Exception for folks using negative weights 
                    binError = sqrt(-qcdHistoSyst.GetBinContent(iBin)**2-qcdHistoStat.GetBinContent(iBin))
                else: 
                    binError = sqrt(qcdHistoSyst.GetBinContent(iBin)**2+qcdHistoStat.GetBinContent(iBin))

                if qcdHistoStat.GetBinContent(iBin)<0: # Check for negative weights (possible in QCD!)
                    binStatError = sqrt(-qcdHistoStat.GetBinContent(iBin))
                else:
                    binStatError = sqrt(qcdHistoStat.GetBinContent(iBin))
                binSystError = qcdHistoSyst.GetBinContent(iBin)

                ##self.configMgr.hists[prefixNom+"_"+str(iBin)].SetBinContent(iBin,self.configMgr.hists[prefixNom].GetBinContent(iBin))
                #
                #print "GREPME %s bin %g content %.2g stat error %.2g syst error %.2g total error %.2g" % (prefixNom,iBin,self.configMgr.hists[prefixNom].GetBinContent(iBin),binStatError,binSystError,binError)
                if binVal > 0.:
                    #self.configMgr.hists[prefixNom].SetBinContent(iBin,binVal) 
                    self.configMgr.hists[prefixNom+"_"+str(iBin)].SetBinContent(iBin,self.configMgr.hists[prefixNom].GetBinContent(iBin))
                else:
                    self.configMgr.hists[prefixNom+"_"+str(iBin)].SetBinContent(iBin,0.)
                    self.configMgr.hists[prefixNom+"_"+str(iBin)].SetBinError(iBin,binError)
                    self.configMgr.hists[prefixNom].SetBinContent(iBin,0.)
                    self.configMgr.hists[prefixNom].SetBinError(iBin,binError)
            #
            if self.configMgr.hists[prefixHigh+"_"+str(iBin)] is None:
                if self.channel.variableName == "cuts":
                    self.configMgr.hists[prefixHigh+"_"+str(iBin)] = TH1F(prefixHigh+"_"+str(iBin),prefixHigh+"_"+str(iBin),len(self.channel.regions),self.channel.binLow,float(len(self.channel.regions))+self.channel.binLow)
                else:
                    self.configMgr.hists[prefixHigh+"_"+str(iBin)] = TH1F(prefixHigh+"_"+str(iBin),prefixHigh+"_"+str(iBin),self.channel.nBins,self.channel.binLow,self.channel.binHigh)
                #
                ##self.configMgr.hists[prefixHigh+"_"+str(iBin)].SetBinContent(iBin,binVal+binError) #self.configMgr.hists[prefixNom].GetBinContent(iBin)+binError)
                ##self.configMgr.hists[prefixHigh].SetBinContent(iBin,binVal+binError) #self.configMgr.hists[prefixNom].GetBinContent(iBin)+binError)
                #
                if binVal+binError > 0.: # self.configMgr.hists[prefixNom].GetBinContent(iBin) > 0.:
                    self.configMgr.hists[prefixHigh+"_"+str(iBin)].SetBinContent(iBin,binVal+binError) #self.configMgr.hists[prefixNom].GetBinContent(iBin)+binError)
                    self.configMgr.hists[prefixHigh].SetBinContent(iBin,binVal+binError) #self.configMgr.hists[prefixNom].GetBinContent(iBin)+binError)
                else:
                    self.configMgr.hists[prefixHigh+"_"+str(iBin)].SetBinContent(iBin,0.)
                    self.configMgr.hists[prefixHigh+"_"+str(iBin)].SetBinError(iBin,binError)
                    self.configMgr.hists[prefixHigh].SetBinContent(iBin,0.)
                    self.configMgr.hists[prefixHigh].SetBinError(iBin,binError)
            #
            if self.configMgr.hists[prefixLow+"_"+str(iBin)] is None:
                if self.channel.variableName == "cuts":
                    self.configMgr.hists[prefixLow+"_"+str(iBin)] = TH1F(prefixLow+"_"+str(iBin),prefixLow+"_"+str(iBin),len(self.channel.regions),self.channel.binLow,float(len(self.channel.regions))+self.channel.binLow)
                else:
                    self.configMgr.hists[prefixLow+"_"+str(iBin)] = TH1F(prefixLow+"_"+str(iBin),prefixLow+"_"+str(iBin),self.channel.nBins,self.channel.binLow,self.channel.binHigh)
                #
                ##self.configMgr.hists[prefixLow+"_"+str(iBin)].SetBinContent(iBin,binVal-binError) # self.configMgr.hists[prefixNom].GetBinContent(iBin)-binError)
                ##self.configMgr.hists[prefixLow].SetBinContent(iBin,binVal-binError) # self.configMgr.hists[prefixNom].GetBinContent(iBin)-binError)
                #
                if (binVal-binError)>0. : # ( self.configMgr.hists[prefixNom].GetBinContent(iBin) - binError ) > 0.:
                    self.configMgr.hists[prefixLow+"_"+str(iBin)].SetBinContent(iBin,binVal-binError) # self.configMgr.hists[prefixNom].GetBinContent(iBin)-binError)
                    self.configMgr.hists[prefixLow].SetBinContent(iBin,binVal-binError) # self.configMgr.hists[prefixNom].GetBinContent(iBin)-binError)
                else:
                    self.configMgr.hists[prefixLow+"_"+str(iBin)].SetBinContent(iBin, 0.)
                    self.configMgr.hists[prefixLow+"_"+str(iBin)].SetBinError(iBin, binError)
                    self.configMgr.hists[prefixLow].SetBinContent(iBin, 0.)
                    self.configMgr.hists[prefixLow].SetBinError(iBin, binError)

        ## MB : also correct the overflow bin!
        for iBin in xrange(self.configMgr.hists[prefixNom].GetNbinsX()+1, self.configMgr.hists[prefixNom].GetNbinsX()+2):
            #
            binVal = self.configMgr.hists[prefixNom].GetBinContent(iBin)
            binError = sqrt(qcdHistoSyst.GetBinContent(iBin)**2+qcdHistoStat.GetBinContent(iBin))
            binStatError = sqrt(qcdHistoStat.GetBinContent(iBin))
            ##binError = sqrt(qcdHistoSyst.GetBinContent(iBin)**2+qcdHistoStat.GetBinContent(iBin)**2)
            ##binStatError = qcdHistoStat.GetBinContent(iBin)
            binSystError = qcdHistoSyst.GetBinContent(iBin)
            #print "GREPME %s bin %g content %.2g stat error %.2g syst error %.2g total error %.2g" % (prefixNom,iBin,self.configMgr.hists[prefixNom].GetBinContent(iBin),binStatError,binSystError,binError)
            if binVal > 0.: # self.configMgr.hists[prefixNom].GetBinContent(iBin) > 0.:
                pass
            else:
                self.configMgr.hists[prefixNom].SetBinContent(iBin, 0.)
            #
            if binVal+binError > 0.: # self.configMgr.hists[prefixNom].GetBinContent(iBin) > 0.:
                self.configMgr.hists[prefixHigh].SetBinContent(iBin, binVal+binError) #self.configMgr.hists[prefixNom].GetBinContent(iBin)+binError)
            else:
                self.configMgr.hists[prefixHigh].SetBinContent(iBin, 0.)
                self.configMgr.hists[prefixHigh].SetBinError(iBin, binStatError)
            #
            if (binVal-binError)>0. : # ( self.configMgr.hists[prefixNom].GetBinContent(iBin) - binError ) > 0.:
                self.configMgr.hists[prefixLow].SetBinContent(iBin, binVal-binError) # self.configMgr.hists[prefixNom].GetBinContent(iBin)-binError)
            else:
                self.configMgr.hists[prefixLow].SetBinContent(iBin, 0.)
                self.configMgr.hists[prefixLow].SetBinError(iBin, binStatError)

        #Over/Underflow bins
        if useOverflow or useUnderflow:
            self.updateOverflowBins(self.configMgr.hists[prefixNom], useOverflow, useUnderflow)
            self.updateOverflowBins(self.configMgr.hists[prefixLow], useOverflow, useUnderflow)
            self.updateOverflowBins(self.configMgr.hists[prefixHigh], useOverflow, useUnderflow)

        return
        
class HistoPrepare(PrepareHistosABC):
    """
    Implementation of PerpareHistosABC

    Reads histograms from a prepared set

    Only read from one file
    """

    def __init__(self, filepath, file2path=''):
        PrepareHistosABC.__init__(self)
        self.cacheFileName = filepath
        import os
        #
        self.cache2FileName = file2path
        if os.path.isfile(file2path):
            self.cache2File = TFile(file2path,"READ")
        else:
            self.cache2File = None
        #
        if os.path.isfile(filepath):
            if not os.path.isfile(file2path): # default, no archive file
                self.cacheFile = TFile(filepath,"READ")
                self.recreate=False
            else:
                self.cacheFile = TFile(filepath,"UPDATE")
                self.recreate=True
        else:
            self.cacheFile = TFile(filepath,"RECREATE")
            self.recreate=True

        return

    def checkTree(self,treeName,fileList):
        """
        Check existence of tree. 
        """
        return False

    def read(self, name, fileList):
        """
        Get the histogram from the file
        """
        log.info("HistoPrepare " + str(fileList))
        return
    
    def addHisto(self, name, nBins=None, binLow=None, binHigh=None, useOverflow=False, useUnderflow=False):
        """
        Add this histogram to the dictionary of histograms
        """
        #Note: useOverflow and useUnderflow has no effect. It's there just for symmetry with TreePrepare above.
        if self.configMgr.hists[name] is None:
            try:
                self.configMgr.hists[name] = self.cache2File.Get(name)
                testsum = self.configMgr.hists[name].GetSum()
            except: # IOError:
                try:
                    self.configMgr.hists[name] = self.cacheFile.Get(name)
                    testsum = self.configMgr.hists[name].GetSum()
                except: # IOError:
                    log.error("Could not find histogram <"+name+"> in "+self.cacheFileName+" ! ")
                    self.configMgr.hists[name] = None
                    raise #Exception("Could not find histogram <"+name+"> in "+self.cacheFileName)

        self.name = name
        return self.configMgr.hists[name]

    def addQCDHistos(self, sample, useOverflow=False, useUnderflow=False):
        #Note: useOverflow and useUnderflow has no effect. It's there just for symmetry with TreePrepare above.
        """
        Read the nominal, high and low QCD histograms
        """
        regString = "".join(self.channel.regions)

        prefixNom = "h%sNom_%s_obs_%s" % (sample.name, regString, self.channel.variableName.replace("/","") )
        prefixHigh = "h%sHigh_%s_obs_%s" % (sample.name, regString, self.channel.variableName.replace("/","") )
        prefixLow = "h%sLow_%s_obs_%s" % (sample.name, regString, self.channel.variableName.replace("/","") )

        self.addHisto(prefixNom)
        self.addHisto(prefixHigh)
        self.addHisto(prefixLow)

        if self.channel.variableName == "cuts":
            nHists = len(self.channel.regions)
        else:
            nHists = self.channel.nBins

        for iBin in xrange(1,nHists+1):
            self.addHisto(prefixNom+"_"+str(iBin))
            self.addHisto(prefixHigh+"_"+str(iBin))
            self.addHisto(prefixLow+"_"+str(iBin))

        return self.configMgr.hists[prefixNom], self.configMgr.hists[prefixLow], self.configMgr.hists[prefixHigh]

    def __del__(self):
        self.cacheFile.Close()
        if self.cache2File!=None: self.cache2File.Close()
