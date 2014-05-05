# This is where one defines the systematics

from systematic import Systematic

class SystematicObject:

    def __init__(self, configMgr):

        ## MC Stat
        self.AR_mcstat_fake = Systematic("AR_mcstat_fake", "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")
        self.AR_mcstat_Zjets = Systematic("AR_mcstat_zjets", "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat") #shapeStat
        self.AR_mcstat_Higgs = Systematic("AR_mcstat_Higgs", "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")
        self.AR_mcstat_WW = Systematic("AR_mcstat_WW"   , "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")
        self.AR_mcstat_WZ = Systematic("AR_mcstat_WZ"   , "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")
        self.AR_mcstat_ZZ = Systematic("AR_mcstat_ZZ"   , "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")
        self.AR_mcstat_triboson = Systematic("AR_mcstat_triboson"   , "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")
        self.AR_mcstat_ttbarV = Systematic("AR_mcstat_ttbarV"  , "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")
        self.AR_mcstat_SIG = Systematic("AR_mcstat_SIG"  , "_CENTRAL", "_CENTRAL", "_CENTRAL", "tree", "shapeStat")
###
###        ## genuine fakes
        self.AR_fake_eFr = Systematic("AR_fake_eFr", "_CENTRAL", "_ELFRUP", "_ELFRDOWN", "tree", "overallSys")
        self.AR_fake_mFr = Systematic("AR_fake_mFr", "_CENTRAL", "_MUFRUP", "_MUFRDOWN", "tree", "overallSys")
        self.AR_fake_eRe = Systematic("AR_fake_eRe", "_CENTRAL", "_ELREUP", "_ELREDOWN", "tree", "overallSys")
        self.AR_fake_mRe = Systematic("AR_fake_mRe", "_CENTRAL", "_MUREUP", "_MUREDOWN", "tree", "overallSys")
###
###        ## genuine uncert
        self.AR_all_EESZ = Systematic("AR_all_EESZ", "_CENTRAL", "_EESZUP", "_EESZDOWN", "tree", "overallSys")
        self.AR_all_EER = Systematic("AR_all_EER", "_CENTRAL", "_EERUP", "_EERDOWN", "tree", "overallSys")
        self.AR_all_EESLOW = Systematic("AR_all_EESLOW", "_CENTRAL", "_EESLOWUP", "_EESLOWDOWN", "tree", "overallSys")
        self.AR_all_EESMAT = Systematic("AR_all_EESMAT", "_CENTRAL", "_EESMATUP", "_EESMATDOWN", "tree", "overallSys")
        self.AR_all_EESPS = Systematic("AR_all_EESPS", "_CENTRAL", "_EESPSUP", "_EESPSDOWN", "tree", "overallSys")
        self.AR_all_MID = Systematic("AR_all_MID", "_CENTRAL", "_IDUP", "_IDDOWN", "tree", "overallSys")
        self.AR_all_JES = Systematic("AR_all_JES", "_CENTRAL", "_JESUP", "_JESDOWN", "tree", "overallSys")
        self.AR_all_MMS = Systematic("AR_all_MMS", "_CENTRAL", "_MSUP", "_MSDOWN", "tree", "overallSys")
        self.AR_all_SCALEST = Systematic("AR_all_SCALEST", "_CENTRAL", "_SCALESTUP", "_SCALESTDOWN", "tree", "overallSys")

        self.AR_all_JER = Systematic('AR_all_JER','_CENTRAL','_JER','_CENTRAL','tree','histoSysOneSide')
        self.AR_all_RESOST   = Systematic('AR_all_RESOST','_CENTRAL','_RESOST','_CENTRAL','tree','histoSysOneSide')
        # we choose not to symmetrize it

        self.AR_all_BJET = Systematic("AR_all_BJET",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_BJETUP"), 
                                 ("eventweight","1.","syst_BJETDOWN"),
                                 "weight",
                                 "overallSys")
                
        self.AR_all_CJET = Systematic("AR_all_CJET",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_CJETUP"), 
                                 ("eventweight","1.","syst_CJETDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_all_BKGMETHOD = Systematic("AR_all_BKGMETHOD",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_BKGMETHODUP"), 
                                 ("eventweight","1.","syst_BKGMETHODDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_all_BMISTAG = Systematic("AR_all_BMISTAG",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_BMISTAGUP"), 
                                 ("eventweight","1.","syst_BMISTAGDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_all_ETRIGREW = Systematic("AR_all_ETRIGREW",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_ETRIGREWUP"), 
                                 ("eventweight","1.","syst_ETRIGREWDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_all_MTRIGREW = Systematic("AR_all_MTRIGREW",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_MTRIGREWUP"), 
                                 ("eventweight","1.","syst_MTRIGREWDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_all_ESF = Systematic("AR_all_ESF",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_ESFUP"), 
                                 ("eventweight","1.","syst_ESFDOWN"),
                                 "weight",
                                 "overallSys")

        # GEN
        # GEN
        # GEN

        self.AR_ww_GEN = Systematic("AR_ww_GEN",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_GENUP"), 
                                 ("eventweight","1.","syst_GENDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_wz_GEN = Systematic("AR_wz_GEN",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_GENUP"), 
                                 ("eventweight","1.","syst_GENDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_zz_GEN = Systematic("AR_zz_GEN",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_GENUP"), 
                                 ("eventweight","1.","syst_GENDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_triboson_GEN = Systematic("AR_triboson_GEN",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_GENUP"), 
                                 ("eventweight","1.","syst_GENDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_ttbarV_GEN = Systematic("AR_ttbarV_GEN",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_GENUP"), 
                                 ("eventweight","1.","syst_GENDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_zjets_GEN = Systematic("AR_zjets_GEN",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_GENUP"), 
                                 ("eventweight","1.","syst_GENDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_Higgs_GEN = Systematic("AR_Higgs_GEN",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_GENUP"), 
                                 ("eventweight","1.","syst_GENDOWN"),
                                 "weight",
                                 "overallSys")

        # XS
        # XS
        # XS

        self.AR_ww_XS = Systematic("AR_ww_XS",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_XSUP"), 
                                 ("eventweight","1.","syst_XSDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_wz_XS = Systematic("AR_wz_XS",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_XSUP"), 
                                 ("eventweight","1.","syst_XSDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_zz_XS = Systematic("AR_zz_XS",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_XSUP"), 
                                 ("eventweight","1.","syst_XSDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_triboson_XS = Systematic("AR_triboson_XS",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_XSUP"), 
                                 ("eventweight","1.","syst_XSDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_ttbarV_XS = Systematic("AR_ttbarV_XS",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_XSUP"), 
                                 ("eventweight","1.","syst_XSDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_zjets_XS = Systematic("AR_zjets_XS",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_XSUP"), 
                                 ("eventweight","1.","syst_XSDOWN"),
                                 "weight",
                                 "overallSys")

        self.AR_Higgs_XS = Systematic("AR_Higgs_XS",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_XSUP"), 
                                 ("eventweight","1.","syst_XSDOWN"),
                                 "weight",
                                 "overallSys")


# _MEFF up is equal to down (negative)
        self.AR_all_MEFF = Systematic("AR_all_MEFF",
                                 configMgr.weights,
                                 ("eventweight","1.","syst_MEFFUP"), 
                                 ("eventweight","1.","syst_MEFFDOWN"),
                                 "weight",
                                 "overallSys")

'''
"CENTRAL" "ELFRDOWN" "ELFRUP" "MUFRDOWN" "MUFRUP" "ELREDOWN" "ELREUP" "MUREDOWN" "MUREUP"
"EESZDOWN"  "EESZUP"  "EERDOWN" "EERUP" "EESLOWDOWN"   "EESLOWUP"  "EESMATDOWN"  "EESMATUP" 
"EESPSDOWN" "EESPSUP"  "IDDOWN"  "IDUP"  "JESDOWN" "JESUP" "MSDOWN" "MSUP" 
"RESOST"   "SCALESTDOWN" "SCALESTUP"
'''