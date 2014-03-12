# Here one defines the TCuts for each region

def selectionConfig(mgr):
  
    LeptonPtAsymm  = "lept1Pt>30000. && lept2Pt>20000."
    LeptonPtSymm   = "lept1Pt>30000. && lept2Pt>30000."
    OneJet         = "L2nCentralLightJets==1"
    TwoORThreeJets = "(L2nCentralLightJets==2 || L2nCentralLightJets==3)" 
    Zveto          = "!(L2Mll>(91200.0-10000.) && L2Mll<(91200.0+10000.))"
    deltaEta       = "(deltaEtaLl<1.5 && deltaEtaLl>-1.5)"

    # EE                          !isOS &&    (!isOS || L2qFlipWeight!=1.0) && 
    mgr.cutsDict["eeSR1jet"]   = "((!isOS || L2qFlipWeight!=1.0) && isEE   && "+ OneJet         +" && "+ LeptonPtAsymm +" && "+ Zveto +" && L2METrel>55000. && mlj<90000. && Ht>200000.)" 
    mgr.cutsDict["eeSR23jets"] = "((!isOS || L2qFlipWeight!=1.0) && isEE   && "+ TwoORThreeJets +" && "+ LeptonPtAsymm +" && "+ Zveto +" && L2METrel>30000. && mljj<120000. && mtmax>100000.)"

    # MM
    mgr.cutsDict["mmSR1jet"]   = "((!isOS || L2qFlipWeight!=1.0) && isMUMU && "+ OneJet         +" && "+ LeptonPtAsymm +" && "+ deltaEta +" && mlj<90000. && Ht>200000. && mtmax>100000.)" 
    mgr.cutsDict["mmSR23jets"] = "((!isOS || L2qFlipWeight!=1.0) && isMUMU && "+ TwoORThreeJets +" && "+ LeptonPtSymm  +" && "+ deltaEta +" && mljj<120000. && Ht>220000.)"

    # EM WITH MTWW
    mgr.cutsDict["emSR1jet"]   = "((!isOS || L2qFlipWeight!=1.0) && isEMU  && "+ OneJet         +" && "+ LeptonPtSymm +" && "+ deltaEta +" && mlj<90000. && mtmax>110000. && mtllmet>120000. )" 
    mgr.cutsDict["emSR23jets"] = "((!isOS || L2qFlipWeight!=1.0) && isEMU  && "+ TwoORThreeJets +" && "+ LeptonPtSymm +" && "+ deltaEta +" && mljj<120000. && mtllmet>110000. )"

    return mgr