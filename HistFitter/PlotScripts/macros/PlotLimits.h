
//////////////////////////////////////////////////////////
// Header file to hold some of the constants and useful //
// information needed for plotting.                     //
/////////////////////////////////////////////////////////

#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TLegend.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TString.h"
#include <vector>
#include <string>
#include <iostream>


//----------------------------------------//
// For plotting
//----------------------------------------//
//const float m_max_x = 550;
//const float m_max_y = 550;
float m_max_x = 600;
float m_max_y = 600;
float m_min_x = 90;
float m_min_y = 0;
float m_ObsLbl_x0 = 0;
float m_ObsLbl_x1 = 0;
float m_ObsLbl_y0 = 0;
float m_ObsLbl_y1 = 0;


//----------------------------------------//
// The input file to consider
//----------------------------------------//
TFile* m_inFile = NULL;

//----------------------------------------//
// Decide if we plot observed
//----------------------------------------//
bool m_plotObs  = false;
TString m_process = "";
TString m_forbidText =  ""; 

//----------------------------------------//
// Masses and stuff
//----------------------------------------//
const TString chi1p  = "#tilde{#chi}_{1}^{+}";
const TString chi1n  = "#tilde{#chi}_{1}^{-}";
const TString chi1pm = "#tilde{#chi}_{1}^{#pm}";
const TString slep   = "#tilde{l}";
const TString sleppm = "#tilde{l}^{#pm}";
const TString snu    = "#tilde{#nu}";
const TString selec  = "#tilde{e}";
const TString smuon  = "#tilde{#mu}";
const TString neut1  = "#tilde{#chi}_{1}^{0}";
const TString neut2  = "#tilde{#chi}_{2}^{0}";
const TString nu     = "#nu";
const TString lep    = "l";

//----------------------------------------//
// Specify some of the decorations
//----------------------------------------//
//const TString m_atlasLabel = "Work in Progress";
//const TString m_atlasLabel = "Preliminary";
const TString m_atlasLabel = "Internal";
//const TString m_atlasLabel = "";
//const TString m_process    = chi1p+chi1n+" #rightarrow 2 #times "+slep+nu+"("+snu+lep+") #rightarrow 2 #times "+lep+nu+neut1;
const TString m_lumiLabel  = "#bf{#scale[0.6]{#int} Ldt = 20.3 fb^{-1}, #sqrt{s} = 8 TeV}";
const TString m_massInfo   = "#bf{m_{#tilde{#nu},#tilde{l}} = (m_{#tilde{#chi}_{1}^{#pm}}+m_{#tilde{#chi}_{1}^{0}})/2 }";

//----------------------------------------//
// Set an enum for command line options
//----------------------------------------//
enum RunOption
{
  RO_SingleSR = 0,
  RO_CombineSR,
  RO_BestSR,
  RO_CharSlep,
  RO_N,
};

const TString RONames[] = {"SingleSR","CombineSR","BestSR","HackCharSlep"};

//----------------------------------------//
// Enum for grid to consider
//----------------------------------------//

enum SignalGrid
{
  SG_ModeCwslep = 0,
  SG_Slepton,
  SG_RHSlepton,
  SG_LHSlepton,
  SG_Selectron,
  SG_RHSelectron,
  SG_LHSelectron,
  SG_Smuon,
  SG_RHSmuon,
  SG_LHSmuon,
  SG_DGemt100,
  SG_DGemt140,
  SG_DGemt250,
  SG_SSWH,
  SG_N
};

const TString SGNames[] = {"SMCwslep","SparseDLiSlep","SparseDLiSlepROnly","SparseDLiSlepLOnly",
			   "SparseDLiSlep","SparseDLiSlepROnly","SparseDLiSlepLOnly",
			   "SparseDLiSlep","SparseDLiSlepROnly","SparseDLiSlepLOnly",
				"DGemt100", "DGemt140", "DGemt250","SSWH"};

//----------------------------------------//
// Enum for signal regions
//----------------------------------------//
enum SignalRegions
{
  SR_1 =0,
  SR_2a,
  SR_2b,
  SR_12,
  SR_4a,
  SR_4b,
  SR_4c,
  SR_4,
  SR_1jet,
  SR_23jets,
  SR_SSCombined,
  SR_All,
  SR_N
};

const TString SRNames[]       = {"SR1",   "SR2a",    "SR2b",  "SR12",  
                                 "SR4a",  "SR4b",    "SR4c",  "SR4", 
                                 "SR1jet","SR23jets","allSRs", 
                                 "SRAll"};
const TString SRProperNames[] = {"SR-WWa","SR-WWb","SR-WWc","SR-WW", 
                                 "SR-m_{T2,90}","SR-m_{T2,120}","SR-m_{T2,150}","SR-mT2", 
                                 "SR-1jet","SR-23jets", "SR-SSWH Combined", 
                                 "SR-All"};
const int SRColors[]          = {kBlue, kViolet, kRed, kBlack, kBlue, kViolet, kRed, kBlack, kBlack};
const int SRMarkers[]         = {24, 25, 26, 23, 20,21,22,23, 23};

//----------------------------------------//
// There will be an option of what to do
// with the plot that is made. Temporary
// plot it to screen, write to .eps, or 
// write to file
//----------------------------------------//

enum SaveOption
{
  SO_Nothing = 0,
  SO_ToEPS,
  SO_ToPDF,
  SO_ToPNG,
  SO_ToRoot,
  SO_N
};

const TString SONames[] = {"Do Nothing","To .eps", "To .pdf", "To .png", "To .root"};


//----------------------------------------//
// Auxiliary Options
//----------------------------------------//

enum AuxOptions
{
  Aux_None = 0,
  Aux_Conf,      // Plot previous conf too
  Aux_Grid,      // Plot grid
  Aux_CLs,       // Plot CLs
  Aux_XS,        // Plot XS limit
  Aux_Sys,       // Plot Systematic per signal point
  Aux_N
};

const TString AuxNames[] = {"None", "Plot Conf", "Plot Grid",
			    "Plot CLs", "Plot XS", "Plot Sys"};

//----------------------------------------//
// Previous limits
//----------------------------------------//
const float m_LEP2_MODECwslep = 103.5;
const TString m_LEP2_MODECwslep_Label = "LEP2 " + chi1pm + " (103.5 GeV)";
const TString m_LEP2_Slepton_Label = "LEP2 #tilde{#mu}_{R} excluded";
