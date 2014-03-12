# This is where one defines the systematics

from systematic import Systematic

class SystematicObject:

    def __init__(self, configMgr):

        ## MC Stat
        self.AR_mcstat_Zjets = Systematic("AR_mcstat_Zjets", "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat") #shapeStat
        self.AR_mcstat_Fakes = Systematic("AR_mcstat_Fakes", "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")
        self.AR_mcstat_Higgs = Systematic("AR_mcstat_Higgs", "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")
        self.AR_mcstat_ZV = Systematic("AR_mcstat_ZV"   , "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")
        self.AR_mcstat_TOP = Systematic("AR_mcstat_TOP"  , "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")
        self.AR_mcstat_WW = Systematic("AR_mcstat_WW"   , "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")
        self.AR_mcstat_SIG = Systematic("AR_mcstat_SIG"  , "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")

        onejetsys = 0.174
        onejethigh = 1 + onejetsys
        onejetlow = 1 - onejetsys

        twojetsys = 0.344  # .344 .43
        twojethigh = 1 + twojetsys
        twojetlow = 1 - twojetsys

        ## Z+V Uncertainty
        self.AR_SR1jetAdhoc_ZV = Systematic("AR_SR1jetAdhoc_ZV"  ,configMgr.weights,onejethigh,onejetlow,"user","userOverallSys") # 17.6% on ZV in SR1jet
        self.AR_SR23jetsAdhoc_ZV = Systematic("AR_SR23jetsAdhoc_ZV",configMgr.weights,twojethigh,twojetlow,"user","userOverallSys") # 43% in ZV in SR23jets

        ## genuine fakes
        self.AR_ELFR = Systematic("AR_ELFR", "_CENTRAL", "_ELFRUP", "_ELFRDOWN", "tree", "overallSys")
        self.AR_MUFR = Systematic("AR_MUFR", "_CENTRAL", "_MUFRUP", "_MUFRDOWN", "tree", "overallSys")
        self.AR_ELRE = Systematic("AR_ELRE", "_CENTRAL", "_ELREUP", "_ELREDOWN", "tree", "overallSys")
        self.AR_MURE = Systematic("AR_MURE", "_CENTRAL", "_MUREUP", "_MUREDOWN", "tree", "overallSys")

        ## genuine uncert
        self.AR_EESZ = Systematic("AR_EESZ", "_CENTRAL", "_EESZUP", "_EESZDOWN", "tree", "overallSys")
        self.AR_EER = Systematic("AR_EER", "_CENTRAL", "_EERUP", "_EERDOWN", "tree", "overallSys")
        self.AR_EESLOW = Systematic("AR_EESLOW", "_CENTRAL", "_EESLOWUP", "_EESLOWDOWN", "tree", "overallSys")
        self.AR_EESMAT = Systematic("AR_EESMAT", "_CENTRAL", "_EESMATUP", "_EESMATDOWN", "tree", "overallSys")
        self.AR_EESPS = Systematic("AR_EESPS", "_CENTRAL", "_EESPSUP", "_EESPSDOWN", "tree", "overallSys")
        self.AR_ID = Systematic("AR_ID", "_CENTRAL", "_IDUP", "_IDDOWN", "tree", "overallSys")
        self.AR_JES = Systematic("AR_JES", "_CENTRAL", "_JESUP", "_JESDOWN", "tree", "overallSys")
        self.AR_MS = Systematic("AR_MS", "_CENTRAL", "_MSUP", "_MSDOWN", "tree", "overallSys")
        self.AR_SCALEST = Systematic("AR_SCALEST", "_CENTRAL", "_SCALESTUP", "_SCALESTDOWN", "tree", "overallSys")

        self.AR_BJET = Systematic("AR_BJET",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_BJETUP"), 
                                 ("eventweight","1.","syst_BJETDOWN"),
                                 "weight",
                                 "overallSys")

        # THIS IS NOT USED
        flat_lumi_sys = 0.028
        flat_lumi_sys_high = 1 + flat_lumi_sys
        flat_lumi_sys_low = 1 - flat_lumi_sys
        self.AR_LUMI = Systematic("AR_LUMI",
                                  configMgr.weights,
                                  flat_lumi_sys_high,
                                  flat_lumi_sys_low,
                                  "user",
                                  "userOverallSys")
        
        self.AR_XS = Systematic("AR_XS",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_XSUP"), 
                                 ("eventweight","1.","syst_XSDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_BKGMETHOD = Systematic("AR_BKGMETHOD",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_BKGMETHODUP"), 
                                 ("eventweight","1.","syst_BKGMETHODDOWN"),
                                 "weight",
                                 "overallSys")

        ## add "RESOST"  
'''
"CENTRAL" "ELFRDOWN" "ELFRUP" "MUFRDOWN" "MUFRUP" "ELREDOWN" "ELREUP" "MUREDOWN" "MUREUP"
"EESZDOWN"  "EESZUP"  "EERDOWN" "EERUP" "EESLOWDOWN"   "EESLOWUP"  "EESMATDOWN"  "EESMATUP" 
"EESPSDOWN" "EESPSUP"  "IDDOWN"  "IDUP"  "JESDOWN" "JESUP" "MSDOWN" "MSUP" 
"RESOST"   "SCALESTDOWN" "SCALESTUP"
'''