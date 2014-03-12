
/////////////////////////////////////////////////////////
// Custom plotting script to handle (at first) 2L C1C1 //
// limits.  The end goal will be to get the contour    //
// plots saved in useable format. In the analysis      //
// chain that I have been introduced to, there seems   //
// to be some redundancy for how we go from tree to    //
// histograms to workspaces to trees to text files to  //
// histograms again.  Maybe we can cut some of these   //
// steps out...                                        //
/////////////////////////////////////////////////////////

// Constants and includes found here
//#include "macros/PlotLimits.h"
#include "macros/PlotTools.C"

// Needed to line up with standard 
#include "borrowed/CombinationGlob.C"

typedef unsigned int uint;

//TODO: Clean up 'new' objects. Right now, not handling memory well.

//TString savedir = "Sep9_Limits/";
//TString savedir = "Sep17_Limits_test/";
//TString savedir = "Sep23_Limits/";
//TString savedir = "Sep23_public/";
//TString inDir   = "limitResults/";
//TString savedir = "Dec1_nonzeroed/";
//TString savedir = "Dec6_hacked_public/";
//TString savedir = "Dec8_public/";
//TString savedir = "Dec9_SerhanTest1/";
//TString savedir = "Dec8_prelim/";
//TString savedir = "Jan9_SerhanUpdated/";
//TString savedir = "Jan13_SerhanUpdated2/";
//TString savedir = "Feb5_SerhanUpdated3/";
//TString savedir = "Feb13_HF00-00-37_v1/";

TString savedir = "Prelim_Plots/";

TString inDir = "limitResults/";

//------------------------------------------------------------//
// Main
//------------------------------------------------------------//
void PlotLimits(TString base = "",
	int SR = SR_4a,
	int runOpt = RO_N,
	bool IncludeObs = false,
	int Grid = SG_N,
	int saveOpt = 0,
	int AuxOption = 0)
{


	if (!checkOptions(base, SR, runOpt, IncludeObs, Grid, saveOpt, AuxOption)) return;

	// Print out some messages
	cout << "-------------------------------------------" << endl;
	cout << "Input base:       " << base << endl;
	cout << "Run Option:       " << RONames[runOpt] << endl;
	cout << "Signal Region:    " << SRProperNames[SR] << endl;
	cout << "Draw Observed:    " << (IncludeObs ? "Yes" : "No") << endl;
	cout << "Grid:             " << SGNames[Grid] << endl;
	cout << "Save Option:      " << SONames[saveOpt] << endl;
	cout << "Aux plotting Opt: " << AuxNames[AuxOption] << endl;
	cout << "-------------------------------------------" << endl;


	// Specify the range based on the choice of grid.  Right
	// now handling things as being symmetric in x-y, think
	// about updating for the future.
	setConstants(Grid);

	// Initialize this tool -- TODO: Why is this needed??
	CombinationGlob::Initialize();

	// Some root things
	gStyle->SetOptStat(kFALSE);
	gROOT->ProcessLine(".L borrowed/AtlasLabels.C");

	// Set if we plot obs
	m_plotObs = IncludeObs;

	// Plot for single SR
	if (runOpt == RO_SingleSR && (SR != SR_All || SR != SR_12 || SR != SR_4))
		plotSingleSR(base, SR, Grid, saveOpt);

	// Plot for combined SR
	if (runOpt == RO_CombineSR)
		plotCombineSR(base, SR, Grid, saveOpt, AuxOption);

	// Plot for best SR
	if (runOpt == RO_BestSR)
		plotBestSR(base, SR, Grid, saveOpt);

	// Plot Chargino+Slepton
	if (runOpt == RO_CharSlep)
		plotHackCharginoSlepton(base, SR, saveOpt, AuxOption);


	cleanUp();

}

//------------------------------------------------------------//
// Plot best region for given point
//------------------------------------------------------------//
void plotBestSR(TString base, int SR, int Grid, int saveOpt)
{

	// Right now there is only SR-mT2 as an option, but
	// try to keep as generic as possible to allow for 
	// others in the future.

	vector<TFile*> files = getFiles(base, SR, Grid, "NoSys");

	// Check to make sure SR is supported
	if (files.size() == 0){
		cout << "No files loaded. Check SR is supported" << endl;
		cout << "by looking in PlotLimits.h to see if enum exists" << endl;
		return;
	}

	// Get the grid points
	vector<TString> names;
	TPolyMarker* gridPoints = GetGridPoints(Grid);
	vector<TPolyMarker*> tempGrids = getBestCLsPerPoint(SR, Grid, files, gridPoints, names);
	vector<TPolyMarker*> bestGrids;

	// Map slepton points. Right now assuming that
	// no other grid is used.
	if (Grid != SG_ModeCwslep && !(Grid == SG_DGemt100 || Grid == SG_DGemt140 || Grid == SG_DGemt250)){
		for (uint ig = 0; ig < tempGrids.size(); ++ig)
			bestGrids.push_back(mapSleptonGrid(bestGrids.at(ig), Grid));
	}
	else bestGrids = tempGrids;

	// Make canvas
	TCanvas* c = makeCanvas("c");

	// Create and draw the fram
	TH2F* frame = makeFrame(Grid);
	frame->Draw();

	// Make legend
	// MATT
	//TLegend* leg = makeLegend(0.6, 0.9, 0.7, 0.85);

	// Draw our grid points.  Right now read
	// from hardcoded list, but should update.
	for (uint i = 0; i < bestGrids.size(); ++i){
		TPolyMarker* grid = bestGrids.at(i);
		grid->Draw("same");
		leg->AddEntry(grid, names.at(i), "p");
	}

	// Redraw Frame Axis
	frame->Draw("sameaxis");

	// Draw Legend
	leg->Draw("same");

	// Add decorations
	addDecorations(c, m_atlasLabel, m_process, m_lumiLabel, m_massInfo, "", Grid);

	// Save to eps
	if (saveOpt > 0){
		TString save = savedir;
		if (saveOpt == SO_ToEPS)   save += savedir + SGNames[Grid] + "_" + SRNames[SR] + "_" + base + "_bestRegion.eps";
		if (saveOpt == SO_ToPDF)   save += savedir + SGNames[Grid] + "_" + SRNames[SR] + "_" + base + "_bestRegion.pdf";
		if (saveOpt == SO_ToPNG)   save += savedir + SGNames[Grid] + "_" + SRNames[SR] + "_" + base + "_bestRegion.png";
		c->SaveAs(save.Data());
	}

	cout << "PLOT BEST SR" << endl;
	cout << "PLOT BEST SR" << endl;
	cout << "PLOT BEST SR" << endl;

}

//------------------------------------------------------------//
// Plot combined region limit
//------------------------------------------------------------//
void plotCombineSR(TString base, int SR, int Grid, int saveOpt,
	int auxOpt)
{

	// Right now there is only SR-mT2 as an option, but
	// try to keep as generic as possible to allow for 
	// others in the future.

	vector<TFile*> f_nom = getFiles(base, SR, Grid, "NoSys");

	// Dont open files unless necessary
	vector<TFile*> f_up;
	vector<TFile*> f_dn;
	if (m_plotObs){
		f_up = getFiles(base, SR, Grid, "up");
		f_dn = getFiles(base, SR, Grid, "down");
	}

	// Check to make sure SR is supported
	if (f_nom.size() == 0 || ((f_up.size() == 0 || f_dn.size() == 0) && m_plotObs)){
		cout << "No files loaded. Check SR is supported" << endl;
		cout << "by looking in PlotLimits.h to see if enum exists" << endl;
		return;
	}

	// Need to build the CLs, CLs1sUp, CLs1sDn
	//vector<TH2F*> h_CLsBest = getBestCLs(f_nom);
	//vector<TH2F*> h_ObsBest;
	//if( m_plotObs ) h_ObsBest = getBestObserved(f_nom,f_up,f_dn);
	vector<TH2F*> h_CLsBest;
	vector<TH2F*> h_ObsBest;
	TH2F* h_CLsObs;
	getBestNew(h_CLsBest, h_ObsBest, f_nom, f_up, f_dn, h_CLsObs, Grid);

	// Add hack to fix specific grids
	hackLimits(h_CLsBest, Grid, true);
	hackLimits(h_ObsBest, Grid, false);

	// Get the contours
	TH2F* h_contourCLSexp = FixAndSetBorders(*h_CLsBest[0], "contour_expcls", "contour_expcls", 0);
	TH2F* h_contourCLS1sUp = FixAndSetBorders(*h_CLsBest[1], "contour1", "contour", 0);
	TH2F* h_contourCLS1sDn = FixAndSetBorders(*h_CLsBest[2], "contour2", "contour", 0);
	TH2F* h_contourObs = NULL;
	TH2F* h_contourObsUp = NULL;
	TH2F* h_contourObsDn = NULL;
	if (m_plotObs){
		h_contourObs = FixAndSetBorders(*h_ObsBest[0], "contour_obs", "contour_obs", 0);
		h_contourObsUp = FixAndSetBorders(*h_ObsBest[1], "contour3", "contour", 0);
		h_contourObsDn = FixAndSetBorders(*h_ObsBest[2], "contour4", "contour", 0);
	}

	//// Test - SERHAN - 05/02/2014
	//h_contourCLSexp->Smooth();
	////h_contourCLS1sUp->Smooth();
	////h_contourCLS1sDn->Smooth();
	//if(h_contourObs)   h_contourObs->Smooth();
	//if(h_contourObsUp) h_contourObsUp->Smooth();
	//if(h_contourObsDn) h_contourObsDn->Smooth();

	////************************** Don't try this at home!!! **************************
	//// Hack SERHAN
	//TH2F* histoSerhan;
	//for(unsigned int iSS=0; iSS<6; ++iSS) {
	//  if(iSS==0)      histoSerhan = h_contourCLSexp;
	//  else if(iSS==1) {continue; histoSerhan = h_contourCLS1sUp;}
	//  else if(iSS==2) {continue; histoSerhan = h_contourCLS1sDn;}
	//  else if(iSS==3) histoSerhan = h_contourObs;
	//  else if(iSS==4) histoSerhan = h_contourObsUp;
	//  else if(iSS==5) histoSerhan = h_contourObsDn;
	//  //cout << "SERHAN :: Currently manipulating " << histoSerhan->GetName() << endl;
	//  for(unsigned int iSX=1;iSX<histoSerhan->GetXaxis()->GetNbins(); iSX++) {
	//    for(unsigned int iSY=1;iSY<histoSerhan->GetYaxis()->GetNbins(); iSY++) {
	//      if( histoSerhan->GetXaxis()->GetBinLowEdge(iSX) > 120 && histoSerhan->GetYaxis()->GetBinUpEdge(iSY) < 10 ) 
	//        histoSerhan->SetBinContent(iSX,iSY,histoSerhan->GetBinContent(iSX,histoSerhan->GetYaxis()->FindBin(10.5)));
	//    }
	//  }
	//}

	// Smooth or not
	if (Grid == SG_ModeCwslep){
		h_contourCLSexp->Smooth();
		h_contourCLS1sUp->Smooth();
		h_contourCLS1sDn->Smooth();
		if (h_contourObs)   h_contourObs->Smooth();
		if (h_contourObsUp) h_contourObsUp->Smooth();
		if (h_contourObsDn) h_contourObsDn->Smooth();

		////************************** Don't try this at home!!! **************************
		//// Hack SERHAN
		//TH2F* histoSerhan;
		//for(unsigned int iSS=0; iSS<6; ++iSS) {
		//  if(iSS==0)      histoSerhan = h_contourCLSexp;
		//  else if(iSS==1) histoSerhan = h_contourCLS1sUp;
		//  else if(iSS==2) histoSerhan = h_contourCLS1sDn;
		//  else if(iSS==3) histoSerhan = h_contourObs;
		//  else if(iSS==4) histoSerhan = h_contourObsUp;
		//  else if(iSS==5) histoSerhan = h_contourObsDn;
		//  //cout << "SERHAN :: Currently manipulating " << histoSerhan->GetName() << endl;
		//  for(unsigned int iSX=1;iSX<histoSerhan->GetXaxis()->GetNbins(); iSX++) {
		//    for(unsigned int iSY=1;iSY<histoSerhan->GetYaxis()->GetNbins(); iSY++) {
		//      if( histoSerhan->GetXaxis()->GetBinLowEdge(iSX) > 400 && histoSerhan->GetYaxis()->GetBinUpEdge(iSY) < 15 ) 
		//        histoSerhan->SetBinContent(iSX,iSY,histoSerhan->GetBinContent(iSX,histoSerhan->GetYaxis()->FindBin(15.5)));
		//    }
		//  }
		//}
		////*******************************************************************************
	}

	// Plotting bands instead of dashed lines
	TGraph* gr_contourCLS1sUp = ContourGraph(h_contourCLS1sUp)->Clone();
	TGraph* gr_contourCLS1sDn = ContourGraph(h_contourCLS1sDn)->Clone();

	// Make canvas
	TCanvas* c = makeCanvas("c");

	// Create and draw the fram
	TH2F* frame = makeFrame(Grid, auxOpt);
	frame->Draw();

	cout << "Frame: " << endl;
	cout << "\t" << frame->GetNbinsX() << " " << frame->GetXaxis()->GetXmin() << " " << frame->GetXaxis()->GetXmax() << endl;
	cout << "\t" << frame->GetNbinsY() << " " << frame->GetYaxis()->GetXmin() << " " << frame->GetYaxis()->GetXmax() << endl;

	// Get the forbidden line
	if (!(Grid == SG_DGemt100 || Grid == SG_DGemt140 || Grid == SG_DGemt250)){
		double m_max_y_offset = 105;
		if (Grid == SG_ModeCwslep) m_max_y_offset = 150;
		TLine* forbidden = getForbiddenLine(m_min_x, m_min_y, m_max_x, m_max_y - m_max_y_offset, Grid);
		forbidden->Draw("same FILL2");
	}

	// Draw Lep Limit
	TGraph* LEP2 = DrawLEPLimit(Grid);

	// Get the 7 TeV result
	TH2F* result7TeV = (Grid == SG_ModeCwslep) ? getATLAS7TeVResult() : NULL;

	// Make legend
	TLegend* leg = NULL;
	if (Grid == SG_ModeCwslep) leg = makeLegend(0.50, 0.80, 0.75, 0.90);
	else                      leg = makeLegend(0.50, 0.80, 0.75, 0.90);

	// Draw stuff
	Int_t c_myYellow = TColor::GetColor("#ffe938"); // TColor::GetColor( "#fee000" )
	Int_t c_myRed = TColor::GetColor("#aa000");
	Int_t c_myExp = TColor::GetColor("#28373c");

	// Draw Contours
	DrawExpectedBand(gr_contourCLS1sUp, gr_contourCLS1sDn, c_myYellow, 1001, 0);
	DrawContourLine95(leg, h_contourCLSexp, "", c_myExp, 2, 2);
	if (result7TeV) DrawContourLine95(leg, result7TeV, "", kBlue, 1, 2);
	if (m_plotObs){
		DrawContourLine95(leg, h_contourObs, "Observed limit (#pm1 #sigma^{SUSY}_{theory})", c_myRed, 1, 4);
		DrawContourLine95(leg, h_contourObsUp, "", c_myRed, 3, 2);
		DrawContourLine95(leg, h_contourObsDn, "", c_myRed, 3, 2);
	}
	DummyLegendExpected(leg, "Expected limit (#pm1 #sigma_{exp})", c_myYellow, 1001, c_myExp, 2, 2);
	if (result7TeV) DummyLegendExpected(leg, "ATLAS 4.7 fb^{-1}, #sqrt{s} = 7 TeV", 0, 0, kBlue, 1, 2);
	//leg->AddEntry(LEP2, m_LEP2_MODECwslep_Label, "F");
	leg->AddEntry(LEP2, LEP2->GetName(), "F");

	// Need to hack some stuff into the legend
	TLine* obsUp = new TLine(m_ObsLbl_x0, m_ObsLbl_y1, m_ObsLbl_x1, m_ObsLbl_y1);
	TLine* obsDn = new TLine(m_ObsLbl_x0, m_ObsLbl_y0, m_ObsLbl_x1, m_ObsLbl_y0);
	if (m_plotObs){
		obsUp->SetLineStyle(3);
		obsUp->SetLineWidth(2);
		obsUp->SetLineColor(c_myRed);
		obsUp->Draw("same");
		obsDn->SetLineStyle(3);
		obsDn->SetLineWidth(2);
		obsDn->SetLineColor(c_myRed);
		obsDn->Draw("same");
	}

	// Plot auxiliary material if chosen
	if (auxOpt == Aux_Conf) plotConf(c, Grid);
	if (auxOpt == Aux_Grid) plotBestGrid(c, SR, f_nom, Grid);
	if (auxOpt == Aux_CLs)  plotCLs(c, Grid, h_CLsObs);
	if (auxOpt == Aux_XS)   plotXS(c, f_nom, Grid, SR, base);
	if (auxOpt == Aux_Sys)  plotSys(c, f_nom, Grid, SR, base);

	// Redraw Frame Axis
	frame->Draw("sameaxis");

	// Draw Legend
	leg->Draw("same");

	// Add decorations
	addDecorations(c, m_atlasLabel, m_process, m_lumiLabel, m_massInfo, m_forbidText, Grid);

	drawZaxis(c, auxOpt, Grid);

	// Save to eps
	if (saveOpt > 0){
		TString save = savedir + SGNames[Grid] + "_" + SRNames[SR] + "_" + base;

		if (auxOpt == Aux_Conf) save += "_confIncluded";
		if (auxOpt == Aux_Grid) save += "_gridIncluded";
		if (auxOpt == Aux_CLs)  save += "_CLsIncluded";
		if (auxOpt == Aux_XS)   save += "_XSIncluded";
		if (auxOpt == Aux_Sys)  save += "_SysIncluded";

		if (saveOpt == SO_ToEPS)   save += ".eps";
		if (saveOpt == SO_ToPDF)   save += ".pdf";
		if (saveOpt == SO_ToPNG)   save += ".png";

		c->SaveAs(save.Data());
	}

	if (saveOpt == SO_ToRoot){
		TString save = savedir + SGNames[Grid] + "_" + SRNames[SR] + "_" + base + ".root";
		TFile* outputFile = new TFile(save.Data(), "recreate");
		h_contourCLSexp->Write("exp_signif");
		h_contourCLS1sUp->Write("exp_signif_plus1sig");
		h_contourCLS1sDn->Write("exp_signif_minus1sig");
		h_contourObs->Write("obs_signif");
		h_contourObsUp->Write("obs_signif_plus1sig");
		h_contourObsDn->Write("obs_signif_minus1sig");
		outputFile->Write();
		outputFile->Close();
	}

	cout << "PLOT COMBINED SR" << endl;
	cout << "PLOT COMBINED SR" << endl;
	cout << "PLOT COMBINED SR" << endl;
}

//------------------------------------------------------------//
// Plot combined region limit
//------------------------------------------------------------//
void plotHackCharginoSlepton(TString base, int SR, int saveOpt,
	int auxOpt)
{

	// Need to load Slepton and Chargino Grid
	vector<TFile*> f_char = getFiles(base, SR, SG_ModeCwslep, "NoSys");
	vector<TFile*> f_slep = getFiles(base, SR, SG_LHSlepton, "NoSys");

	// Dont open files unless necessary
	vector<TFile*> f_up_char;
	vector<TFile*> f_dn_char;
	vector<TFile*> f_up_slep;
	vector<TFile*> f_dn_slep;
	if (m_plotObs){
		f_up_char = getFiles(base, SR, SG_ModeCwslep, "up");
		f_dn_char = getFiles(base, SR, SG_ModeCwslep, "down");
		f_up_slep = getFiles(base, SR, SG_LHSlepton, "up");
		f_dn_slep = getFiles(base, SR, SG_LHSlepton, "down");
	}

	// Check to make sure SR is supported
	if (f_char.size() == 0 || ((f_up_char.size() == 0 || f_dn_char.size() == 0) && m_plotObs) ||
		f_slep.size() == 0 || ((f_up_slep.size() == 0 || f_dn_slep.size() == 0) && m_plotObs)){
		cout << "No files loaded. Check SR is supported" << endl;
		cout << "by looking in PlotLimits.h to see if enum exists" << endl;
		return;
	}

	// Need to build the CLs, CLs1sUp, CLs1sDn
	// Chargino
	vector<TH2F*> h_CLsBest;
	vector<TH2F*> h_ObsBest;
	TH2F* h_CLsObs;
	getBestNew(h_CLsBest, h_ObsBest, f_char, f_up_char, f_dn_char, h_CLsObs, SG_ModeCwslep);

	vector<TH2F*> h_CLsBest_slep;
	vector<TH2F*> h_ObsBest_slep;
	TH2F* h_CLsObs_slep;
	getBestNew(h_CLsBest_slep, h_ObsBest_slep, f_slep, f_up_slep, f_dn_slep, h_CLsObs_slep, SG_LHSlepton);

	// Add hack to fix specific grids
	hackLimits(h_CLsBest, SG_ModeCwslep, true);
	hackLimits(h_ObsBest, SG_ModeCwslep, false);
	hackLimits(h_ObsBest_slep, SG_LHSlepton, false);

	// Get the contours
	// Chargino
	TH2F* h_contourCLSexp = FixAndSetBorders(*h_CLsBest[0], "contour_expcls", "contour_expcls", 0);
	TH2F* h_contourCLS1sUp = FixAndSetBorders(*h_CLsBest[1], "contour1", "contour", 0);
	TH2F* h_contourCLS1sDn = FixAndSetBorders(*h_CLsBest[2], "contour2", "contour", 0);
	TH2F* h_contourObs = NULL;
	TH2F* h_contourObsUp = NULL;
	TH2F* h_contourObsDn = NULL;
	if (m_plotObs){
		h_contourObs = FixAndSetBorders(*h_ObsBest[0], "contour_obs", "contour_obs", 0);
		h_contourObsUp = FixAndSetBorders(*h_ObsBest[1], "contour3", "contour", 0);
		h_contourObsDn = FixAndSetBorders(*h_ObsBest[2], "contour4", "contour", 0);
	}

	// Slepton
	TH2F* h_contourObs_slep = NULL;
	TH2F* h_contourObsUp_slep = NULL;
	TH2F* h_contourObsDn_slep = NULL;
	if (m_plotObs){
		h_contourObs_slep = FixAndSetBorders(*h_ObsBest_slep[0], "contour_obs_slep", "contour_obs_slep", 0);
		h_contourObsUp_slep = FixAndSetBorders(*h_ObsBest_slep[1], "contour3_slep", "contour_slep", 0);
		h_contourObsDn_slep = FixAndSetBorders(*h_ObsBest_slep[2], "contour4_slep", "contour_slep", 0);
	}

	// Smooth or not
	//if( Grid == SG_ModeCwslep ){
	// always smooth ModeC
	h_contourCLSexp->Smooth();
	h_contourCLS1sUp->Smooth();
	h_contourCLS1sDn->Smooth();
	if (h_contourObs)   h_contourObs->Smooth();
	if (h_contourObsUp) h_contourObsUp->Smooth();
	if (h_contourObsDn) h_contourObsDn->Smooth();
	//************************** Don't try this at home!!! **************************
	// Hack SERHAN
	TH2F* histoSerhan;
	for (unsigned int iSS = 0; iSS < 6; ++iSS) {
		if (iSS == 0)      histoSerhan = h_contourCLSexp;
		else if (iSS == 1) histoSerhan = h_contourCLS1sUp;
		else if (iSS == 2) histoSerhan = h_contourCLS1sDn;
		else if (iSS == 3) histoSerhan = h_contourObs;
		else if (iSS == 4) histoSerhan = h_contourObsUp;
		else if (iSS == 5) histoSerhan = h_contourObsDn;
		//cout << "SERHAN :: Currently manipulating " << histoSerhan->GetName() << endl;
		for (unsigned int iSX = 1; iSX < histoSerhan->GetXaxis()->GetNbins(); iSX++) {
			for (unsigned int iSY = 1; iSY<histoSerhan->GetYaxis()->GetNbins(); iSY++) {
				if (histoSerhan->GetXaxis()->GetBinLowEdge(iSX) > 400 && histoSerhan->GetYaxis()->GetBinUpEdge(iSY) < 15)
					histoSerhan->SetBinContent(iSX, iSY, histoSerhan->GetBinContent(iSX, histoSerhan->GetYaxis()->FindBin(15.5)));
			}
		}
	}
	//*******************************************************************************
	//}

	// Plotting bands instead of dashed lines
	TGraph* gr_contourCLS1sUp = ContourGraph(h_contourCLS1sUp)->Clone();
	TGraph* gr_contourCLS1sDn = ContourGraph(h_contourCLS1sDn)->Clone();

	TGraph* gr_ObsSlep = ContourGraph(h_contourObs_slep)->Clone();
	TGraph* gr_ObsUpSlep = ContourGraph(h_contourObsUp_slep)->Clone();
	TGraph* gr_ObsDnSlep = ContourGraph(h_contourObsDn_slep)->Clone();
	gr_ObsSlep = mapSlepToCharginoGraph(gr_ObsSlep);
	gr_ObsUpSlep = mapSlepToCharginoGraph(gr_ObsUpSlep);
	gr_ObsDnSlep = mapSlepToCharginoGraph(gr_ObsDnSlep);

	// Make canvas
	TCanvas* c = makeCanvas("c");

	// Create and draw the fram
	TH2F* frame = makeFrame(SG_ModeCwslep, auxOpt);
	frame->Draw();

	// Get the forbidden line
	TLine* forbidden = getForbiddenLine(m_min_x, m_min_y, m_max_x, m_max_y - 175, SG_ModeCwslep);
	forbidden->Draw("same FILL2");

	// Draw Lep Limit
	TGraph* LEP2 = DrawLEPLimit(SG_ModeCwslep);

	// Get the 7 TeV result
	TH2F* result7TeV = getATLAS7TeVResult();

	// Make legend
	TLegend* leg = NULL;
	leg = makeLegend(0.50, 0.80, 0.70, 0.90);

	// Draw stuff
	Int_t c_myYellow = TColor::GetColor("#ffe938"); // TColor::GetColor( "#fee000" )
	Int_t c_myRed = TColor::GetColor("#aa000");
	Int_t c_myExp = TColor::GetColor("#28373c");

	// Draw Contours
	DrawExpectedBand(gr_contourCLS1sUp, gr_contourCLS1sDn, c_myYellow, 1001, 0);
	DrawContourLine95(leg, h_contourCLSexp, "", c_myExp, 2, 2);
	if (result7TeV) DrawContourLine95(leg, result7TeV, "", kBlue, 1, 2);
	if (m_plotObs){
		DrawContourLine95(leg, h_contourObs, "Observed limit (#pm1 #sigma^{SUSY}_{theory})", c_myRed, 1, 4);
		DrawContourLine95(leg, h_contourObsUp, "", c_myRed, 3, 2);
		DrawContourLine95(leg, h_contourObsDn, "", c_myRed, 3, 2);
		DummyLegendExpected(leg, "Expected limit (#pm1 #sigma_{exp})", c_myYellow, 1001, c_myExp, 2, 2);
		//DrawContourLine95(leg, h_contourObs_slep,   "Observed limit (#pm1 #sigma^{SUSY}_{theory})", kMagenta, 1 , 4);
		//DrawContourLine95(leg, h_contourObsUp_slep, "", kMagenta, 3, 2);
		//DrawContourLine95(leg, h_contourObsDn_slep, "", kMagenta, 3, 2);
		gr_ObsSlep->SetLineColor(kGreen + 3);
		gr_ObsUpSlep->SetLineColor(kGreen + 3);
		gr_ObsDnSlep->SetLineColor(kGreen + 3);
		gr_ObsUpSlep->SetLineStyle(2);
		gr_ObsDnSlep->SetLineStyle(2);
		gr_ObsSlep->Draw("samecont");
		gr_ObsUpSlep->Draw("samecont");
		gr_ObsDnSlep->Draw("samecont");
		leg->AddEntry(gr_ObsSlep, "#tilde{l}_{L}#tilde{l}_{L} Observed limit (#pm1 #sigma^{SUSY}_{theory})", "l");
	}

	// STOPPED HERE

	if (result7TeV) DummyLegendExpected(leg, "ATLAS 4.7 fb^{-1}, #sqrt{s} = 7 TeV", 0, 0, kBlue, 1, 2);
	//leg->AddEntry(LEP2, m_LEP2_MODECwslep_Label, "F");
	leg->AddEntry(LEP2, LEP2->GetName(), "F");

	// Need to hack some stuff into the legend
	//double greenOffset = 49; 
	double greenOffset = 40;
	TLine* obsUp = new TLine(m_ObsLbl_x0, m_ObsLbl_y1, m_ObsLbl_x1, m_ObsLbl_y1);
	TLine* obsUp_Slep = new TLine(m_ObsLbl_x0, m_ObsLbl_y1 - greenOffset, m_ObsLbl_x1, m_ObsLbl_y1 - greenOffset);
	TLine* obsDn = new TLine(m_ObsLbl_x0, m_ObsLbl_y0, m_ObsLbl_x1, m_ObsLbl_y0);
	TLine* obsDn_Slep = new TLine(m_ObsLbl_x0, m_ObsLbl_y0 - greenOffset, m_ObsLbl_x1, m_ObsLbl_y0 - greenOffset);
	if (m_plotObs){
		obsUp->SetLineStyle(3);
		obsUp->SetLineWidth(2);
		obsUp->SetLineColor(c_myRed);
		obsUp->Draw("same");
		obsDn->SetLineStyle(3);
		obsDn->SetLineWidth(2);
		obsDn->SetLineColor(c_myRed);
		obsDn->Draw("same");
		obsUp_Slep->SetLineStyle(3);
		obsUp_Slep->SetLineWidth(2);
		obsUp_Slep->SetLineColor(kGreen + 3);
		obsUp_Slep->Draw("same");
		obsDn_Slep->SetLineStyle(3);
		obsDn_Slep->SetLineWidth(2);
		obsDn_Slep->SetLineColor(kGreen + 3);
		obsDn_Slep->Draw("same");
	}


	// Plot auxiliary material if chosen
	if (auxOpt == Aux_Conf) plotConf(c, Grid);
	if (auxOpt == Aux_Grid) plotBestGrid(c, SR, f_nom, Grid);
	if (auxOpt == Aux_CLs)  plotCLs(c, Grid, h_CLsObs);
	if (auxOpt == Aux_XS)   plotXS(c, f_nom, Grid, SR, base);
	if (auxOpt == Aux_Sys)  plotSys(c, f_nom, Grid, SR, base);

	// Redraw Frame Axis
	frame->Draw("sameaxis");

	// Draw Legend
	leg->Draw("same");

	// Add decorations
	addDecorations(c, m_atlasLabel, m_process, m_lumiLabel, m_massInfo, m_forbidText, SG_ModeCwslep);

	drawZaxis(c, auxOpt, SG_ModeCwslep);

	// Save to eps
	if (saveOpt > 0){
		//TString save = savedir + SGNames[Grid] + "_"+ SRNames[SR] + "_" + base;
		TString save = savedir + "ModeCSlepHack_" + SRNames[SR] + "_" + base;

		if (auxOpt == Aux_Conf) save += "_confIncluded";
		if (auxOpt == Aux_Grid) save += "_gridIncluded";
		if (auxOpt == Aux_CLs)  save += "_CLsIncluded";
		if (auxOpt == Aux_XS)   save += "_XSIncluded";
		if (auxOpt == Aux_Sys)  save += "_SysIncluded";

		if (saveOpt == SO_ToEPS)   save += ".eps";
		if (saveOpt == SO_ToPDF)   save += ".pdf";
		if (saveOpt == SO_ToPNG)   save += ".png";

		c->SaveAs(save.Data());
	}

	if (saveOpt == SO_ToRoot){
		TString save = savedir + "ModeCSlepHack_" + SRNames[SR] + "_" + base + ".root";
		TFile* outputFile = new TFile(save.Data(), "recreate");
		h_contourCLSexp->Write("exp_signif");
		h_contourCLS1sUp->Write("exp_signif_plus1sig");
		h_contourCLS1sDn->Write("exp_signif_minus1sig");
		h_contourObs->Write("obs_signif");
		h_contourObsUp->Write("obs_signif_plus1sig");
		h_contourObsDn->Write("obs_signif_minus1sig");
		outputFile->Write();
		outputFile->Close();
	}

	cout << "PLOT CS HACK HACK SR" << endl;
	cout << "PLOT CS HACK HACK SR" << endl;
	cout << "PLOT CS HACK HACK SR" << endl;

}


//------------------------------------------------------------//
// Plot single signal region limit
//------------------------------------------------------------//
void plotSingleSR(TString base, int SR, int Grid, int saveOpt)
{

	// Specify the file to read in from
	TString grid = SGNames[Grid];
	TString nomAppend = "_" + grid + "_NoSys";//_Output_hypotest";
	TString upAppend = "_" + grid + "_up";//_Output_hypotest";
	TString downAppend = "_" + grid + "_down";//_Output_hypotest";
	TString nominal = inDir + SRNames[SR] + "_" + base + nomAppend + "/" + SRNames[SR] + "_" + base + nomAppend + "_histograms.root";
	TString up = inDir + SRNames[SR] + "_" + base + upAppend + "/" + SRNames[SR] + "_" + base + upAppend + "_histograms.root";
	TString down = inDir + SRNames[SR] + "_" + base + downAppend + "/" + SRNames[SR] + "_" + base + downAppend + "_histograms.root";

	// Open files
	m_inFile = new TFile(nominal);
	TFile* f_up = NULL;
	TFile* f_dn = NULL;

	// Don't load files if not plotting observed
	if (m_plotObs){
		f_up = new TFile(up);
		f_dn = new TFile(down);
	}

	if (m_inFile == NULL || ((f_up == NULL || f_dn == NULL) && m_plotObs)){
		cout << " Input file cannot be opened " << endl;
		return;
	}

	// Get the necessary histograms
	TH2F* h_CLsval = (TH2F*)m_inFile->Get("hCLs");
	TH2F* h_CLsexp = (TH2F*)m_inFile->Get("sigp1expclsf");
	TH2F* h_CLs1sUp = (TH2F*)m_inFile->Get("sigclsu1s");
	TH2F* h_CLs1sDn = (TH2F*)m_inFile->Get("sigclsd1s");
	TH2F* h_Obs = NULL;
	TH2F* h_ObsUp = NULL;
	TH2F* h_ObsDn = NULL;
	if (m_plotObs){
		h_Obs = (TH2F*)m_inFile->Get("sigp1clsf");
		h_ObsUp = (TH2F*)f_up->Get("sigp1clsf")->Clone("up");   // Later take from diff file
		h_ObsDn = (TH2F*)f_dn->Get("sigp1clsf")->Clone("down"); // Later take from diff file
	}

	// Get the contours
	TH2F* h_contourCLSexp = FixAndSetBorders(*h_CLsexp, "contour_expcls", "contour_expcls", 0);
	TH2F* h_contourCLS1sUp = FixAndSetBorders(*h_CLs1sUp, "contour_1su", "contour_1su", 0);
	TH2F* h_contourCLS1sDn = FixAndSetBorders(*h_CLs1sDn, "contour_1sd", "contour_1sd", 0);
	TH2F* h_contourObs = NULL;
	TH2F* h_contourObsUp = NULL;
	TH2F* h_contourObsDn = NULL;
	if (m_plotObs){
		h_contourObs = FixAndSetBorders(*h_Obs, "contour_obs", "contour_obs", 0);
		h_contourObsUp = FixAndSetBorders(*h_ObsUp, "contour_obs1su", "contour_obs1su", 0);
		h_contourObsDn = FixAndSetBorders(*h_ObsDn, "contour_obs1sd", "contour_obs1sd", 0);
	}

	//// Smooth
	//h_contourCLSexp->Smooth();
	//h_contourCLS1sUp->Smooth();
	//h_contourCLS1sDn->Smooth();
	//if(h_contourObs)   h_contourObs->Smooth();
	//if(h_contourObsUp) h_contourObsUp->Smooth();
	//if(h_contourObsDn) h_contourObsDn->Smooth();

	// Plotting bands instead of dashed lines
	TGraph* gr_contourCLS1sUp = ContourGraph(h_contourCLS1sUp)->Clone();
	TGraph* gr_contourCLS1sDn = ContourGraph(h_contourCLS1sDn)->Clone();

	// Make canvas
	TCanvas* c = makeCanvas("c");

	// Create and draw the fram
	TH2F* frame = makeFrame(Grid);
	frame->Draw();

	// Make legend
	TLegend* leg = makeLegend(0.50, 0.80, 0.75, 0.915);
	//TLegend* leg = makeLegend(0.6, 0.9, 0.7, 0.9);
	//TLegend* leg = makeLegend(0.15, 0.45, 0.59, 0.69);

	// Draw stuff
	Int_t c_myYellow = TColor::GetColor("#ffe938"); // TColor::GetColor( "#fee000" )
	Int_t c_myRed = TColor::GetColor("#aa000");
	Int_t c_myExp = TColor::GetColor("#28373c");

	// Draw Contours
	DrawExpectedBand(gr_contourCLS1sUp, gr_contourCLS1sDn, c_myYellow, 1001, 0);
	DrawContourLine95(leg, h_contourCLSexp, "", c_myExp, 3, 2);
	if (m_plotObs){
		DrawContourLine95(leg, h_contourObs, "Observed limit (#pm1 #sigma^{SUSY}_{theory})", c_myRed, 1, 4);
		DrawContourLine95(leg, h_contourObsUp, "", c_myRed, 3, 2);
		DrawContourLine95(leg, h_contourObsDn, "", c_myRed, 3, 2);
	}
	DummyLegendExpected(leg, "Expected limit (#pm1 #sigma_{exp})", c_myYellow, 1001, c_myExp, 2, 2);

	// Need to hack some stuff into the legend
	TLine* obsUp = new TLine(m_ObsLbl_x0, m_ObsLbl_y1, m_ObsLbl_x1, m_ObsLbl_y1);
	TLine* obsDn = new TLine(m_ObsLbl_x0, m_ObsLbl_y0, m_ObsLbl_x1, m_ObsLbl_y0);
	if (m_plotObs){
		obsUp->SetLineStyle(3);
		obsUp->SetLineWidth(2);
		obsUp->SetLineColor(c_myRed);
		obsUp->Draw("same");
		obsDn->SetLineStyle(3);
		obsDn->SetLineWidth(2);
		obsDn->SetLineColor(c_myRed);
		obsDn->Draw("same");
	}

	// Redraw Frame Axis
	frame->Draw("sameaxis");

	// Draw Legend
	leg->Draw("same");

	// Draw our grid points.  Right now read
	// from hardcoded list, but should update.
	TPolyMarker* gridPoints = GetGridPoints(Grid);
	gridPoints->SetMarkerStyle(29);
	gridPoints->SetMarkerSize(1.2);
	gridPoints->SetMarkerColor(kGreen + 3);
	gridPoints->Draw("same");

	// Get the forbidden line
	if (Grid == SG_SSWH){
		TLine* forbidden = new TLine(125., 0., 225. - 30., 100. - 30.);
		forbidden->SetLineStyle(9);
		forbidden->SetLineWidth(2);
		forbidden->SetLineColor(kBlack);
		forbidden->Draw("same FILL2");
	}

	// START paste cls SUNEET

	//TCanvas* &c, int Grid, TH2F* contour

	// Get the grid points
	TPolyMarker* gridPoints = GetGridPoints(Grid);
	int nP = gridPoints->GetN();
	double* xP = gridPoints->GetX();
	double* yP = gridPoints->GetY();

	TH2F* h_CLs = new TH2F("temp", "", 200, m_min_x, m_max_x, 200, m_min_y, m_max_y);

	//cout<<"------------------------"<<endl;

	// Now loop over and get the CLs values
	for (int i = 0; i < nP; ++i){
		float x = xP[i];
		float y = yP[i];
		float nx = 0;
		float ny = 0;

		int bin = h_CLsval->FindBin(x, y);
		float CLs = h_CLsval->GetBinContent(bin);
		int rotate = 0x7451DCF3;
		float step = 0.01;
		float range = step;
		while (CLs == 0) {
			if (range > 3.0) break;
			for (int j = 0; j < 8; j++) {
				nx = x + range * (rotate >> 4 * j & 3 ? (rotate >> 4 * j & 2 ? 1 : -1) : 0);
				ny = y + range * (rotate >> 4 * j + 2 & 3 ? (rotate >> 4 * j + 2 & 2 ? 1 : -1) : 0);
				bin = h_CLsval->FindBin(nx, ny);
				CLs = h_CLsval->GetBinContent(bin);
				if (CLs != 0) break;
			}
			range += step;
		}
		if (nx != 0) cout << " dx: " << nx - x << endl;
		if (ny != 0) cout << " dy: " << ny - y << endl;
		h_CLs->Fill(x + 5.0, y, CLs);
		cout << "x: " << x << " y: " << y << " CLs: " << CLs << endl;
	}

	//c->cd();
	gStyle->SetPaintTextFormat("4.2f");
	h_CLs->SetMarkerSize(1.0);
	h_CLs->Draw("same text");

	// END paste cls

	// Add decorations
	addDecorations(c, m_atlasLabel, m_process, m_lumiLabel, m_massInfo, m_forbidText, Grid);

	// Save to eps
	if (saveOpt > 0){
		TString save;
		if (saveOpt == SO_ToEPS)   TString save = savedir + SGNames[Grid] + "_" + SRNames[SR] + "_single.eps";
		if (saveOpt == SO_ToPDF)   TString save = savedir + SGNames[Grid] + "_" + SRNames[SR] + "_single.pdf";
		if (saveOpt == SO_ToPNG)   TString save = savedir + SGNames[Grid] + "_" + SRNames[SR] + "_single.png";
		c->SaveAs(save.Data());
	}

	cout << "PLOT SINGLE SR" << endl;
	cout << "PLOT SINGLE SR" << endl;
	cout << "PLOT SINGLE SR" << endl;

}


//------------------------------------------------------------//
// Load files for given SR
//------------------------------------------------------------//
vector<TFile*> getFiles(TString base, int SR, int Grid, TString sys)
{

	// Structure for the files
	TString append = sys;

	// Vector for files
	vector<TFile*> inFiles;

	// Common name
	TString common = base + "_" + SGNames[Grid];

	// SR 4 -- SR-mT2
	if (SR == SR_All || SR == SR_4){
		TString f_SR4a = inDir + SRNames[SR_4a] + "_" + common + "_" + append + "/" + SRNames[SR_4a] + "_" + common + "_" + append + "_histograms.root";
		TString f_SR4b = inDir + SRNames[SR_4b] + "_" + common + "_" + append + "/" + SRNames[SR_4b] + "_" + common + "_" + append + "_histograms.root";
		TString f_SR4c = inDir + SRNames[SR_4c] + "_" + common + "_" + append + "/" + SRNames[SR_4c] + "_" + common + "_" + append + "_histograms.root";
		inFiles.push_back(new TFile(f_SR4a.Data()));
		inFiles.push_back(new TFile(f_SR4b.Data()));
		inFiles.push_back(new TFile(f_SR4c.Data()));
	}
	if (SR == SR_All || SR == SR_12){
		TString f_SR1 = inDir + SRNames[SR_1] + "_" + common + "_" + append + "/" + SRNames[SR_1] + "_" + common + "_" + append + "_histograms.root";
		TString f_SR2a = inDir + SRNames[SR_2a] + "_" + common + "_" + append + "/" + SRNames[SR_2a] + "_" + common + "_" + append + "_histograms.root";
		TString f_SR2b = inDir + SRNames[SR_2b] + "_" + common + "_" + append + "/" + SRNames[SR_2b] + "_" + common + "_" + append + "_histograms.root";
		inFiles.push_back(new TFile(f_SR1.Data()));
		inFiles.push_back(new TFile(f_SR2a.Data()));
		inFiles.push_back(new TFile(f_SR2b.Data()));
	}


	return inFiles;

}

//------------------------------------------------------------//
// Get best histogram
//------------------------------------------------------------//
vector<TH2F*> getBestCLs(vector<TFile*> inputs)
{

	// The return structure will be:
	// 0th element -- CLs
	// 1st element -- CLs + 1 sigma
	// 2nd element -- CLs - 1 sigma

	vector<TH2F*> finalHists(3, NULL);
	TString n_CLs = "sigp1expclsf";
	TString n_CLsUp = "sigclsu1s";
	TString n_CLsDn = "sigclsd1s";

	// Need to read in the histograms for the inputs
	vector<TH2F*> h_CLs;
	vector<TH2F*> h_CLsUp;
	vector<TH2F*> h_CLsDn;

	// Loop and save the histograms
	for (uint i = 0; i < inputs.size(); ++i){
		TFile* input = inputs.at(i);
		h_CLs.push_back((TH2F*)input->Get(n_CLs.Data()));
		h_CLsUp.push_back((TH2F*)input->Get(n_CLsUp.Data()));
		h_CLsDn.push_back((TH2F*)input->Get(n_CLsDn.Data()));

	}

	// Set the final histograms structure
	finalHists[0] = h_CLs[0]->Clone("final_CLs");
	finalHists[1] = h_CLs[0]->Clone("final_CLsUp");
	finalHists[2] = h_CLs[0]->Clone("final_CLsDn");
	for (uint i = 0; i < finalHists.size(); ++i)
		finalHists[i]->Reset();

	setBestHists(finalHists, h_CLs, h_CLsUp, h_CLsDn);

	return finalHists;

}

//------------------------------------------------------------//
// Get Best observed
//------------------------------------------------------------//
vector<TH2F*> getBestObserved(vector<TFile*> nominal,
	vector<TFile*> sysUp,
	vector<TFile*> sysDn)
{

	// Nominal -- NoSys Option
	// sysUp   -- Signal uncertainty up
	// sysDn   -- Signal uncertainty dn

	// The return structure will be the same
	// 0th element -- CLs
	// 1st element -- CLs + 1 sigma
	// 2nd element -- CLs - 1 sigma

	vector<TH2F*> finalHists(3, NULL);
	TString pname = "sigp1clsf";

	// Need to read in the histograms for the inputs
	vector<TH2F*> h_Obs;
	vector<TH2F*> h_ObsUp;
	vector<TH2F*> h_ObsDn;

	// Loop and save the histograms
	for (uint i = 0; i < nominal.size(); ++i){
		TFile* nom = nominal.at(i);
		TFile* up = sysUp.at(i);
		TFile* down = sysDn.at(i);
		h_Obs.push_back((TH2F*)nom->Get(pname.Data())->Clone(Form("nom_%i", i)));
		h_ObsUp.push_back((TH2F*)up->Get(pname.Data())->Clone(Form("sysUp_%i", i)));
		h_ObsDn.push_back((TH2F*)down->Get(pname.Data())->Clone(Form("sysDn_&i", i)));
	}

	// Set the final histograms structure
	finalHists[0] = h_Obs[0]->Clone("final_Obs");
	finalHists[1] = h_Obs[0]->Clone("final_ObsUp");
	finalHists[2] = h_Obs[0]->Clone("final_ObsDn");
	for (uint i = 0; i < finalHists.size(); ++i)
		finalHists[i]->Reset();

	setBestHists(finalHists, h_Obs, h_ObsUp, h_ObsDn);

	return finalHists;

}

//------------------------------------------------------------//
// Get Best observed
//------------------------------------------------------------//
void getBestNew(vector<TH2F*> &exp,
	vector<TH2F*> &obs,
	vector<TFile*> nominal,
	vector<TFile*> sysUp,
	vector<TFile*> sysDn,
	TH2F* &finalCLsObs,
	int Grid)
{

	// Nominal -- NoSys Option
	// sysUp   -- Signal uncertainty up
	// sysDn   -- Signal uncertainty dn

	// The return structure will be the same
	// 0th element -- CLs
	// 1st element -- CLs + 1 sigma
	// 2nd element -- CLs - 1 sigma

	exp = vector<TH2F*>(3, NULL);
	obs = vector<TH2F*>(3, NULL);
	//vector<TH2F*> finalHists (3,NULL);
	TString n_Obs = "sigp1clsf";
	TString n_CLs = "sigp1expclsf";
	TString n_CLsUp = "sigclsu1s";
	TString n_CLsDn = "sigclsd1s";
	TString n_CLsObs = "hCLs";

	// Need to read in the histograms for the inputs
	vector<TH2F*> h_Obs;
	vector<TH2F*> h_ObsUp;
	vector<TH2F*> h_ObsDn;
	vector<TH2F*> h_CLs;
	vector<TH2F*> h_CLsUp;
	vector<TH2F*> h_CLsDn;
	vector<TH2F*> h_CLsObs;

	// Loop and save the histograms
	for (uint i = 0; i < nominal.size(); ++i){
		TFile* nom = nominal.at(i);
		TFile* up = sysUp.at(i);
		TFile* down = sysDn.at(i);
		h_Obs.push_back((TH2F*)nom->Get(n_Obs.Data())->Clone(Form("obs_nom_%i", i)));
		h_ObsUp.push_back((TH2F*)up->Get(n_Obs.Data())->Clone(Form("obs_sysUp_%i", i)));
		h_ObsDn.push_back((TH2F*)down->Get(n_Obs.Data())->Clone(Form("obs_sysDn_&i", i)));
		h_CLs.push_back((TH2F*)nom->Get(n_CLs.Data())->Clone(Form("cls_nom_%i", i)));
		h_CLsUp.push_back((TH2F*)nom->Get(n_CLsUp.Data())->Clone(Form("cls_sysUp_%i", i)));
		h_CLsDn.push_back((TH2F*)nom->Get(n_CLsDn.Data())->Clone(Form("cls_sysDn_&i", i)));
		h_CLsObs.push_back((TH2F*)nom->Get(n_CLsObs.Data())->Clone(Form("cls_obs_&i", i)));
	}

	// Set the final histograms structure
	obs[0] = h_Obs[0]->Clone("final_Obs");
	obs[1] = h_Obs[0]->Clone("final_ObsUp");
	obs[2] = h_Obs[0]->Clone("final_ObsDn");
	exp[0] = h_CLs[0]->Clone("final_Cls");
	exp[1] = h_CLs[0]->Clone("final_ClsUp");
	exp[2] = h_CLs[0]->Clone("final_ClsDn");
	for (uint i = 0; i < obs.size(); ++i){
		obs[i]->Reset();
		exp[i]->Reset();
	}
	finalCLsObs = (TH2F*)h_CLsObs[0]->Clone("final_CLsObs");
	finalCLsObs->Reset();

	setBestHistsNew(exp, h_CLs, h_CLsUp, h_CLsDn, obs, h_Obs, h_ObsUp, h_ObsDn, finalCLsObs, h_CLsObs, Grid);

	//return finalHists;

}

//------------------------------------------------------------//
// Set the best based on best region
//------------------------------------------------------------//
void setBestHists(vector<TH2F*> &finalHists,
	vector<TH2F*> h_CLs,
	vector<TH2F*> h_CLsUp,
	vector<TH2F*> h_CLsDn)
{


	// Now loop over the bins and set the appropriate
	// values for the CLs and +/- 1 sigma values.
	int nxbins = finalHists[0]->GetXaxis()->GetNbins();
	int nybins = finalHists[0]->GetYaxis()->GetNbins();

	for (int xbin = 1; xbin <= nxbins; ++xbin){
		for (int ybin = 1; ybin <= nybins; ++ybin){

			// Variables needed
			float maxCLs = 0;
			float sigUp = 0;
			float sigDn = 0;

			for (uint i = 0; i<h_CLs.size() - 1; ++i){
				for (uint j = i; j<h_CLs.size(); ++j){
					float ith_cls = h_CLs[i]->GetBinContent(xbin, ybin);
					float jth_cls = h_CLs[j]->GetBinContent(xbin, ybin);

					if (ith_cls > jth_cls && ith_cls > maxCLs){
						maxCLs = ith_cls;
						sigUp = h_CLsUp[i]->GetBinContent(xbin, ybin);
						sigDn = h_CLsDn[i]->GetBinContent(xbin, ybin);
					}
					else if (jth_cls > ith_cls && jth_cls > maxCLs){
						maxCLs = jth_cls;
						sigUp = h_CLsUp[j]->GetBinContent(xbin, ybin);
						sigDn = h_CLsDn[j]->GetBinContent(xbin, ybin);
					}

				}// end loop over j
			}// end loop over i

			// Set the final results
			finalHists[0]->SetBinContent(xbin, ybin, maxCLs);
			finalHists[1]->SetBinContent(xbin, ybin, sigUp);
			finalHists[2]->SetBinContent(xbin, ybin, sigDn);

		}// end loop over ybins
	}// end loop over xbins

}

//------------------------------------------------------------//
// New set best
//------------------------------------------------------------//
void setBestHistsNew(vector<TH2F*> &final_exp,
	vector<TH2F*> h_CLs,
	vector<TH2F*> h_CLsUp,
	vector<TH2F*> h_CLsDn,
	vector<TH2F*> &final_obs,
	vector<TH2F*> h_Obs,
	vector<TH2F*> h_ObsUp,
	vector<TH2F*> h_ObsDn,
	TH2F* &       final_ObsCLs,
	vector<TH2F*> h_ObsCLs,
	int Grid)
{


	// Now loop over the bins and set the appropriate
	// values for the CLs and +/- 1 sigma values.
	int nxbins = final_exp[0]->GetXaxis()->GetNbins();
	int nybins = final_exp[0]->GetYaxis()->GetNbins();

	// There are a few issues with some of the points
	// on the grid where the fit fails, but since the
	// other significances are negative, it assigns
	// the best grid to the failed fit.  Add a hack here


	for (int xbin = 1; xbin <= nxbins; ++xbin){
		for (int ybin = 1; ybin <= nybins; ++ybin){

			// Variables needed
			float maxCLs = -999;
			float sigUp = 0;
			float sigDn = 0;
			float maxObs = 0;
			float obsUp = 0;
			float obsDn = 0;
			float obsCLs = 0;

			// Adding a hack to deal with one of the
			// Mode C points where expected signif from
			// SRmT2b is larger than SRmT2a even though
			// there is 0 signal events in SRmT2b...
			// Solution: Force SRmT2a - Matt 17/9/2013     
			//float troubleBin_x = h_CLs[0]->GetXaxis()->FindBin(130);
			//float troubleBin_y = h_CLs[0]->GetYaxis()->FindBin(30);
			//if( Grid == SG_ModeCwslep && troubleBin_x == xbin &&
			//troubleBin_y == ybin){
			int hack = hackPoint(h_CLs[0], Grid, xbin, ybin);
			if (hack >= 0){
				maxCLs = h_CLs[hack]->GetBinContent(xbin, ybin);
				sigUp = h_CLsUp[hack]->GetBinContent(xbin, ybin);
				sigDn = h_CLsDn[hack]->GetBinContent(xbin, ybin);
				maxObs = h_Obs[hack]->GetBinContent(xbin, ybin);
				obsUp = h_ObsUp[hack]->GetBinContent(xbin, ybin);
				obsDn = h_ObsDn[hack]->GetBinContent(xbin, ybin);
				obsCLs = h_ObsCLs[hack]->GetBinContent(xbin, ybin);
			}
			else{
				for (uint i = 0; i < h_CLs.size() - 1; ++i){

					for (uint j = i + 1; j<h_CLs.size(); ++j){
						float ith_cls = h_CLs[i]->GetBinContent(xbin, ybin);
						float jth_cls = h_CLs[j]->GetBinContent(xbin, ybin);

						if (ith_cls >= jth_cls && ith_cls > maxCLs){
							maxCLs = ith_cls;
							sigUp = h_CLsUp[i]->GetBinContent(xbin, ybin);
							sigDn = h_CLsDn[i]->GetBinContent(xbin, ybin);
							maxObs = h_Obs[i]->GetBinContent(xbin, ybin);
							obsUp = h_ObsUp[i]->GetBinContent(xbin, ybin);
							obsDn = h_ObsDn[i]->GetBinContent(xbin, ybin);
							obsCLs = h_ObsCLs[i]->GetBinContent(xbin, ybin);
						}
						else if (jth_cls > ith_cls && jth_cls > maxCLs){
							maxCLs = jth_cls;
							sigUp = h_CLsUp[j]->GetBinContent(xbin, ybin);
							sigDn = h_CLsDn[j]->GetBinContent(xbin, ybin);
							maxObs = h_Obs[j]->GetBinContent(xbin, ybin);
							obsUp = h_ObsUp[j]->GetBinContent(xbin, ybin);
							obsDn = h_ObsDn[j]->GetBinContent(xbin, ybin);
							obsCLs = h_ObsCLs[j]->GetBinContent(xbin, ybin);
						}

					}// end loop over j
				}// end loop over i
			}// end else

			if (maxCLs == -999){
				cout << "Max: " << maxCLs << endl;
				for (uint i = 0; i < h_CLs.size(); ++i)
					cout << "\t" << h_CLs[i]->GetBinContent(xbin, ybin) << endl;
			}


			// Set the final results
			final_exp[0]->SetBinContent(xbin, ybin, maxCLs);
			final_exp[1]->SetBinContent(xbin, ybin, sigUp);
			final_exp[2]->SetBinContent(xbin, ybin, sigDn);
			final_obs[0]->SetBinContent(xbin, ybin, maxObs);
			final_obs[1]->SetBinContent(xbin, ybin, obsUp);
			final_obs[2]->SetBinContent(xbin, ybin, obsDn);
			final_ObsCLs->SetBinContent(xbin, ybin, obsCLs);


		}// end loop over ybins
	}// end loop over xbins

}

//------------------------------------------------------------//
// Get best histogram
//------------------------------------------------------------//
vector<TPolyMarker*> getBestCLsPerPoint(int SR,
	int Grid,
	vector<TFile*> inputs,
	TPolyMarker* points,
	vector<TString> &names)
{

	// This will be the return value
	vector<TPolyMarker*> bestSRs;

	// Need to specify names and colors
	vector<int> colors;
	vector<int> markers;

	if (!(SR == SR_All || SR == SR_4 || SR == SR_12)) return bestSR;

	if (SR == SR_All || SR == SR_4){
		names.push_back(SRProperNames[SR_4a]);
		names.push_back(SRProperNames[SR_4b]);
		names.push_back(SRProperNames[SR_4c]);
		colors.push_back(SRColors[SR_4a]);
		colors.push_back(SRColors[SR_4b]);
		colors.push_back(SRColors[SR_4c]);
		markers.push_back(SRMarkers[SR_4a]);
		markers.push_back(SRMarkers[SR_4b]);
		markers.push_back(SRMarkers[SR_4c]);
	}
	if (SR == SR_All || SR == SR_12){
		names.push_back(SRProperNames[SR_1]);
		names.push_back(SRProperNames[SR_2a]);
		names.push_back(SRProperNames[SR_2b]);
		colors.push_back(SRColors[SR_1]);
		colors.push_back(SRColors[SR_2a]);
		colors.push_back(SRColors[SR_2b]);
		markers.push_back(SRMarkers[SR_1]);
		markers.push_back(SRMarkers[SR_2a]);
		markers.push_back(SRMarkers[SR_2b]);
	}


	// Need to read in the histograms for the inputs
	vector<TH2F*> h_CLs;
	vector<TH2F*> h_Obs;
	TString n_CLs = "sigp1expclsf";
	TString n_Obs = "sigp1clsf";
	// Loop and save the histograms
	for (uint i = 0; i < inputs.size(); ++i){
		TFile* input = inputs.at(i);
		h_CLs.push_back((TH2F*)input->Get(n_CLs.Data()));
		h_Obs.push_back((TH2F*)input->Get(n_Obs.Data()));
	}

	// Now loop over the points and choose the histogram with 
	// best value.
	int nPoints = points->GetN();
	Double_t* xpts = points->GetX();
	Double_t* ypts = points->GetY();

	// Keep track of n, x, y for each SR
	// This technically shouldn't work but CINT
	// is too stupid to pick it up. Fixed just
	// in case this is converted to compilable code
	// in the future.
	if (SR == SR_All){
		const int nMaxSR = 6;
		int nSR[nMaxSR] = { 0, 0, 0, 0, 0, 0 };
	}
	else {
		const int nMaxSR = 3;
		int   nSR[nMaxSR] = { 0, 0, 0 };
	}
	//const int nMaxSR = (SR==SR_All) ? 6 : 3;
	//int nSR[nMaxSR];
	//for(int iSR=0; i<nMaxSR; ++i) nSR[i] = 0;

	//vector<int>     nSR (nMaxSR, 0);
	double xSR[nMaxSR][1000];
	double ySR[nMaxSR][1000];

	for (int np = 0; np < nPoints; ++np){
		float xval = xpts[np];
		float yval = ypts[np];

		float cls_bin = h_CLs[0]->FindBin(xval, yval);
		float cls_min = -999;
		int bestIndex = -1;

		for (uint i = 0; i<h_CLs.size() - 1; ++i){
			for (uint j = i + 1; j<h_CLs.size(); ++j){
				float ith_cls = h_CLs[i]->GetBinContent(cls_bin);
				float jth_cls = h_CLs[j]->GetBinContent(cls_bin);

				if (ith_cls >= jth_cls && ith_cls > cls_min){
					bestIndex = i;
					cls_min = ith_cls;
				}
				else if (jth_cls > ith_cls && jth_cls > cls_min){
					bestIndex = j;
					cls_min = jth_cls;
				}

			}// end loop over j
		}// end loop over i

		// Increment and store the info
		if (bestIndex >= 0){

			// Hack to fix weird point in Mode C w/slepton 
			// grid.
			if (xval == 130 && yval == 30 && Grid == SG_ModeCwslep)
				bestIndex = 0;

			// Hack to fix two points in Slepton grid
			if ((Grid == SG_Slepton || Grid == SG_Smuon || Grid == SG_LHSlepton || Grid == SG_LHSmuon)
				&& ((xval == 98 && yval == 60) || (xval == 195 && yval == 160)))
				bestIndex = 0;

			int current = nSR[bestIndex]; //.at(bestIndex);
			xSR[bestIndex][current] = xval;
			ySR[bestIndex][current] = yval;
			nSR[bestIndex] += 1;
		}
		//if( 230 < xval &&  xval < 270 && 100 < yval && yval < 140 ){
		if ((xval == 369 && yval == 15) ||
			(xval == 369 && yval == 135))
		{

			cout << "m(Sl): " << xval << " m(n1): " << yval << endl;
			cout << "\tSRmTa: " << h_Obs[0]->GetBinContent(cls_bin) << endl;
			cout << "\tSRmTb: " << h_Obs[1]->GetBinContent(cls_bin) << endl;
			cout << "\tSRmTc: " << h_Obs[2]->GetBinContent(cls_bin) << endl;
			cout << endl;
			cout << "\tSRmTa: " << h_CLs[0]->GetBinContent(cls_bin) << endl;
			cout << "\tSRmTb: " << h_CLs[1]->GetBinContent(cls_bin) << endl;
			cout << "\tSRmTc: " << h_CLs[2]->GetBinContent(cls_bin) << endl;
			cout << endl;
		}
	}// end loop over points

	// Now we have the points.  We can save the TPoly objects and
	// set all the relevant attributes.
	//cout<<"About to add SR"<<endl;
	//for(uint i=0; i<nSR.size(); ++i){
	for (int k = 0; k < nMaxSR; ++k){
		TPolyMarker* temp = new TPolyMarker(nSR[k], xSR[k], ySR[k]);
		bestSRs.push_back(temp);
		bestSRs.at(k)->SetMarkerStyle(20);
		bestSRs.at(k)->SetMarkerSize(1.1);
		bestSRs.at(k)->SetMarkerColor(colors.at(k));
		bestSRs.at(k)->SetMarkerStyle(markers.at(k));
	}

	return bestSRs;

}

//------------------------------------------------------------//
// Method to clean things up
//------------------------------------------------------------//
void cleanUp()
{

	// Remove unwanted Root dictionary crap
	//system("rm *.so *cxx*");

}

//------------------------------------------------------------//
// Check command line options to make sure they are kosher
//------------------------------------------------------------//
bool checkOptions(TString base, int SR, int runOpt, bool IncludeObs,
	int Grid, int saveOpt, int AuxOption)
{

	// Check to make sure the TString isn't empty
	if (base.Length() == 0 && runOpt == RO_SingleSR){
		cout << endl;
		cout << "******* ERROR *******" << endl;
		cout << endl;
		cout << "Input base file name is empty" << endl;
		cout << "Please input a file name" << endl;
		cout << "This should be the same as 'analysisName'" << endl;
		cout << "that was set in LimitConfig.py" << endl;
		return false;
	}

	if (base.Length() == 0 && runOpt != RO_SingleSR){
		cout << endl;
		cout << "******* ERROR *******" << endl;
		cout << endl;
		cout << "The input base is empty. This should be" << endl;
		cout << "the same as 'analysisName', except" << endl;
		cout << "it doesn't need to contain the SR at" << endl;
		cout << "the front." << endl;
		return false;
	}

	// Check to make sure the run option is set
	if (runOpt >= RO_N){
		cout << "******* ERROR *******" << endl;
		cout << "Run option " << runOpt << " is not supported" << endl;
		cout << "Options are: " << endl;
		cout << "\t 0 -- To run single SR limit" << endl;
		cout << "\t 1 -- To combine multipel SR limits" << endl;
		return false;
	}

	// Check the grid
	if (Grid >= SG_N){
		cout << "******* ERROR *******" << endl;
		cout << "Grid " << Grid << " is not supported" << endl;
		cout << "Options are: " << endl;
		cout << "\t 0 -- Mode C w/sleptons" << endl;
		cout << "\t 1 -- Sleptons" << endl;
		cout << "\t 2 -- RH Slepton" << endl;
		return false;
	}

	// Current supported save options
	if (saveOpt >= SO_N){
		cout << "******* ERROR *******" << endl;
		cout << "Save option " << saveOpt << " is not supported" << endl;;
		cout << " Options are: " << endl;
		cout << "\t 0 -- Do Nothing" << endl;
		cout << "\t 1 -- Save to .eps" << endl;
		cout << "\t 2 -- Save to .root" << endl;
		return false;
	}

	// Check the auxiliary plotting options
	if (AuxOption >= Aux_N){
		cout << "******* ERROR *******" << endl;
		cout << "Aux plotting option " << AuxOption << " is not supported " << endl;;
		cout << " Options are: " << endl;
		cout << "\t 0 -- Do Nothing" << endl;
		cout << "\t 1 -- Plot Conf Result" << endl;
		cout << "\t 2 -- Plot Grid" << endl;
		cout << "\t 3 -- Plot CLs" << endl;
		cout << "\t 4 -- Plot XS Limit" << endl;
		return false;
	}

	return true;

}

//------------------------------------------------------------//
// Set the constants based on the grid chosen
//------------------------------------------------------------//
void setConstants(int Grid)
{

	// Options for Mode C w/slep
	if (Grid == SG_ModeCwslep){

		// Specify grid coordinates
		m_min_x = 100;
		m_max_x = 600;
		m_min_y = 0;
		m_max_y = 400;

		// Specify the process text
		m_process = chi1p + chi1n + " #rightarrow 2 #times " + slep + nu + "(" + snu + lep + ") #rightarrow 2 #times " + lep + nu + neut1;

		// Specify forbidden text
		m_forbidText = "m(" + chi1pm + ") < m(" + neut1 + ")";

		// For the observed band, we have to add on 
		// the +/- 1 sigma bands for the legend
		//m_ObsLbl_x0 = 290.0;
		//m_ObsLbl_x1 = 330.0;
		//m_ObsLbl_y0 = 455.0;
		//m_ObsLbl_y1 = 472.5;
		m_ObsLbl_x0 = 341.5;
		m_ObsLbl_x1 = 375.0;
		m_ObsLbl_y0 = 365.0;
		m_ObsLbl_y1 = 377.5;

	}
	else if (Grid == SG_DGemt100 || Grid == SG_DGemt140 || Grid == SG_DGemt250){

		// Specify grid coordinates
		m_min_x = 100;
		m_max_x = 500;
		m_min_y = 100;
		m_max_y = 500;

		m_ObsLbl_x0 = 105;
		m_ObsLbl_x1 = 125;
		m_ObsLbl_y0 = 283;
		m_ObsLbl_y1 = 295;

		if (Grid == SG_DGemt100) m_process = "DGemt 100";
		if (Grid == SG_DGemt140) m_process = "DGemt 140";
		if (Grid == SG_DGemt250) m_process = "DGemt 250";

	}
	else if (Grid == SG_SSWH){

		// Specify grid coordinates
		m_min_x = 100;
		m_max_x = 250;
		m_min_y = 0;
		m_max_y = 100;

		m_process = chi1pm + neut2 + " #rightarrow W^{#pm}h " + neut1 + neut1 + " #rightarrow SS + MET";

		// Specify forbidden text
		m_forbidText = "m(" + chi1pm + " , " + neut2 + ") - m(" + neut1 + ") < 125 GeV";

		// For the observed band, we have to add on 
		// the +/- 1 sigma bands for the legend
		m_ObsLbl_x0 = 172.5;
		m_ObsLbl_x1 = 182.5;
		m_ObsLbl_y0 = 89.5;
		m_ObsLbl_y1 = 94;

	}

	else{

		// Specify grid coordinates
		m_min_x = 90;
		m_max_x = 400;
		m_min_y = 0;
		m_max_y = 350;

		// Specify the process text
		if (Grid == SG_Slepton) m_process = slep + "^{#pm}_{L,R}" + slep + "^{#mp}_{L,R} #rightarrow l^{#pm}" + neut1 + "l^{#mp}" + neut1;
		if (Grid == SG_RHSlepton) m_process = slep + "^{#pm}_{R}" + slep + "^{#mp}_{R} #rightarrow l^{#pm}" + neut1 + "l^{#mp}" + neut1;
		if (Grid == SG_LHSlepton) m_process = slep + "^{#pm}_{L}" + slep + "^{#mp}_{L} #rightarrow l^{#pm}" + neut1 + "l^{#mp}" + neut1;
		if (Grid == SG_Selectron) m_process = selec + "^{#pm}_{L,R}" + selec + "^{#mp}_{L,R} #rightarrow e^{#pm}" + neut1 + "e^{#mp}" + neut1;
		if (Grid == SG_RHSelectron) m_process = selec + "^{#pm}_{R}" + selec + "^{#mp}_{R} #rightarrow e^{#pm}" + neut1 + "e^{#mp}" + neut1;
		if (Grid == SG_LHSelectron) m_process = selec + "^{#pm}_{L}" + selec + "^{#mp}_{L} #rightarrow e^{#pm}" + neut1 + "e^{#mp}" + neut1;
		if (Grid == SG_Smuon) m_process = smuon + "^{#pm}_{L,R}" + smuon + "^{#mp}_{L,R} #rightarrow #mu^{#pm}" + neut1 + "#mu^{#mp}" + neut1;
		if (Grid == SG_RHSmuon) m_process = smuon + "^{#pm}_{R}" + smuon + "^{#mp}_{R} #rightarrow #mu^{#pm}" + neut1 + "#mu^{#mp}" + neut1;
		if (Grid == SG_LHSmuon) m_process = smuon + "^{#pm}_{L}" + smuon + "^{#mp}_{L} #rightarrow #mu^{#pm}" + neut1 + "#mu^{#mp}" + neut1;

		// Specify forbidden text
		m_forbidText = "m(" + sleppm + ") < m(" + neut1 + ")";

		// For the observed band, we have to add on 
		// the +/- 1 sigma bands for the legend
		m_ObsLbl_x0 = 240;
		m_ObsLbl_x1 = 260;
		m_ObsLbl_y0 = 315;
		m_ObsLbl_y1 = 329;

	}

}

//------------------------------------------------------------//
// Plot conference note results
//------------------------------------------------------------//
void plotConf(TCanvas* &c, int Grid)
{
	c->cd();
	TH2D* h_confExp = getConfResults(Grid, true);
	TH2D* h_confObs = getConfResults(Grid, false);
	h_confExp->Draw("samecont3");
	h_confObs->Draw("samecont3");

	TLegend* legConf = makeLegend(0.72, 0.8, 0.75, 0.85);
	legConf->AddEntry(h_confExp, "Conf. Expected", "l");
	legConf->AddEntry(h_confObs, "Conf. Observed", "l");
	legConf->Draw("same");

}

//------------------------------------------------------------//
// Draw best grid on a canvas
//------------------------------------------------------------//
void plotBestGrid(TCanvas* &c, int SR, vector<TFile*> files, int Grid)
{

	vector<TString> names;
	TPolyMarker* gridPoints = GetGridPoints(Grid);

	vector<TPolyMarker*> tempGrids = getBestCLsPerPoint(SR, Grid, files, gridPoints, names);
	vector<TPolyMarker*> bestGrids;

	// Map slepton points. Right now assuming that
	// no other grid is used.
	if (Grid != SG_ModeCwslep && !(Grid == SG_DGemt100 || Grid == SG_DGemt140 || Grid == SG_DGemt250)){
		for (uint ig = 0; ig < tempGrids.size(); ++ig)
			bestGrids.push_back(mapSleptonGrid(tempGrids.at(ig), Grid));
	}
	else bestGrids = tempGrids;

	double legendOffset = 0.05;
	if (Grid == SG_ModeCwslep) legendOffset = 0.0;
	TLegend* legGrid = makeLegend(0.16, 0.40, 0.57 + legendOffset, 0.67 + legendOffset);
	for (uint i = 0; i < bestGrids.size(); ++i){
		TPolyMarker* grid = bestGrids.at(i);
		grid->Draw("same");
		legGrid->AddEntry(grid, names.at(i), "p");
	}
	legGrid->Draw("same");

}

//------------------------------------------------------------//
// Plot the CLs values for the observed contour
//------------------------------------------------------------//
void plotCLs(TCanvas* &c, int Grid, TH2F* contour)
{

	// Get the grid points
	TPolyMarker* gridPoints = GetGridPoints(Grid);
	int nP = gridPoints->GetN();
	double* xP = gridPoints->GetX();
	double* yP = gridPoints->GetY();

	TH2F* h_CLs = new TH2F("temp", "", 200, m_min_x, m_max_x, 200, m_min_y, m_max_y);

	//cout<<"------------------------"<<endl;

	// Now loop over and get the CLs values
	for (int i = 0; i < nP; ++i){
		float x = xP[i];
		float y = yP[i];
		int bin = contour->FindBin(x, y);
		float CLs = contour->GetBinContent(bin);
		if (hackCLs(Grid, x, y)) CLs = 0;
		h_CLs->Fill(x, y, CLs);
		cout << "x: " << x << " y: " << y << " CLs: " << CLs << endl;
	}

	if (Grid != SG_ModeCwslep && !(Grid == SG_DGemt100 || Grid == SG_DGemt140 || Grid == SG_DGemt250))
		h_CLs = mapSleptonHist(h_CLs, Grid);

	c->cd();
	gStyle->SetPaintTextFormat("04.1f");
	h_CLs->SetMarkerSize(0.75);
	//h_CLs->Draw("same text45");
	h_CLs->Draw("same text");

}

//------------------------------------------------------------//
// Hack some of the CLs values due to stupid ROOT features
//------------------------------------------------------------//
bool hackCLs(int Grid, float x, float y)
{

	if (Grid == SG_Slepton){
		if (x == 130 && y == 70) return true;
		if (x == 150 && y == 90) return true;
	}

	if (Grid == SG_LHSlepton){
		if (x == 130 && y == 70) return true;
		if (x == 90 && y == 30)  return true;
		if (x == 110 && y == 50) return true;

	}

	if (Grid == SG_Selectron){
		if (x == 130 && y == 70) return true;
	}
	if (Grid == SG_RHSelectron){
		if (x == 130 && y == 50) return true;
	}

	if (Grid == SG_Smuon){
		if (x == 130 && y == 70) return true;
		if (x == 110 && y == 50) return true;
		if (x == 150 && y == 90) return true;
	}

	if (Grid == SG_LHSmuon){
		if (x == 110 && y == 50) return true;
		if (x == 130 && y == 70) return true;
	}
	if (Grid == SG_RHSmuon){
		if (x == 110 && y == 30) return true;
		if (x == 140 && y == 40) return true;
		if (x == 170 && y == 50) return true;
		if (x == 90 && y == 0) return true;
	}

	return false;
}


//------------------------------------------------------------//
// Plot systematic for each signal point
//------------------------------------------------------------//
void plotSys(TCanvas* &c, vector<TFile*> inputs, int Grid, int SR, TString base)
{

	// Create Histograms and holders
	TH2F* h_final = new TH2F("xs_hist", "", 200, m_min_x, m_max_x, 200, m_min_y, m_max_y);

	// Need to specify names and colors
	vector<int> SRs;
	vector<TH2F*> sysHists;

	if (SR == SR_4){
		SRs.push_back(SR_4a);
		SRs.push_back(SR_4b);
		SRs.push_back(SR_4c);

		sysHists.push_back(new TH2F("sys_SR4a", "", 200, m_min_x, m_max_x, 200, m_min_y, m_max_y));
		sysHists.push_back(new TH2F("sys_SR4b", "", 200, m_min_x, m_max_x, 200, m_min_y, m_max_y));
		sysHists.push_back(new TH2F("sys_SR4c", "", 200, m_min_x, m_max_x, 200, m_min_y, m_max_y));

		getSysPerPoint(base, SR_4a, Grid, sysHists[0]);
		getSysPerPoint(base, SR_4b, Grid, sysHists[1]);
		getSysPerPoint(base, SR_4c, Grid, sysHists[2]);
	}

	else return h_final;

	// Need to read in the histograms for the inputs
	vector<TH2F*> h_CLs;
	TString n_CLs = "sigp1expclsf";
	// Loop and save the histograms
	for (uint i = 0; i < inputs.size(); ++i){
		TFile* input = inputs.at(i);
		h_CLs.push_back((TH2F*)input->Get(n_CLs.Data()));
	}

	// Loop over to determine the best CLs value
	// and then fill h_final with the corresponding
	// correct sys value.
	TPolyMarker* points = GetGridPoints(Grid);
	int nPoints = points->GetN();
	Double_t* xpts = points->GetX();
	Double_t* ypts = points->GetY();

	for (int np = 0; np < nPoints; ++np){
		float xval = xpts[np];
		float yval = ypts[np];

		float cls_bin = h_CLs[0]->FindBin(xval, yval);
		float cls_min = -999;
		int bestIndex = -1;

		for (uint i = 0; i<h_CLs.size() - 1; ++i){
			for (uint j = i + 1; j<h_CLs.size(); ++j){
				float ith_cls = h_CLs[i]->GetBinContent(cls_bin);
				float jth_cls = h_CLs[j]->GetBinContent(cls_bin);

				if (ith_cls >= jth_cls && ith_cls > cls_min){
					bestIndex = i;
					cls_min = ith_cls;
				}
				else if (jth_cls > ith_cls && jth_cls > cls_min){
					bestIndex = j;
					cls_min = jth_cls;
				}

			}// end loop over j
		}// end loop over i

		// Increment and store the info
		if (bestIndex >= 0){

			// Hack to fix weird point in Mode C w/slepton 
			// grid.
			if (xval == 130 && yval == 30 && Grid == SG_ModeCwslep)
				bestIndex = 0;

			int bin = h_final->FindBin(xval, yval);
			float sys = sysHists.at(bestIndex)->GetBinContent(bin);
			h_final->SetBinContent(bin, sys);
		}

	}// end loop over points

	// Map Slepton results
	if (Grid != SG_ModeCwslep) h_final = mapSleptonHist(h_final, Grid);

	c->cd();
	if (Grid == SG_ModeCwslep)
		gStyle->SetPaintTextFormat(".1f");
	else
		gStyle->SetPaintTextFormat(".1f");
	h_final->SetMarkerSize(0.75);
	h_final->Draw("same text");

}

//------------------------------------------------------------//
// Plot excluded XS
//------------------------------------------------------------//
void plotXS(TCanvas* &c, vector<TFile*> inputs, int Grid, int SR, TString base)
{

	// Create Histograms and holders
	TH2F* h_final = new TH2F("xs_hist", "", 200, m_min_x, m_max_x, 200, m_min_y, m_max_y);

	// Need to specify names and colors
	vector<int> SRs;
	vector<TH2F*> xsHists;

	if (SR == SR_4){
		SRs.push_back(SR_4a);
		SRs.push_back(SR_4b);
		SRs.push_back(SR_4c);
		xsHists.push_back(new TH2F("xs_SR4a", "", 200, m_min_x, m_max_x, 200, m_min_y, m_max_y));
		xsHists.push_back(new TH2F("xs_SR4b", "", 200, m_min_x, m_max_x, 200, m_min_y, m_max_y));
		xsHists.push_back(new TH2F("xs_SR4c", "", 200, m_min_x, m_max_x, 200, m_min_y, m_max_y));

		getXSUpperLimit(base, SR_4a, Grid, xsHists[0]);
		getXSUpperLimit(base, SR_4b, Grid, xsHists[1]);
		getXSUpperLimit(base, SR_4c, Grid, xsHists[2]);
	}

	else return h_final;

	// Need to read in the histograms for the inputs
	vector<TH2F*> h_CLs;
	TString n_CLs = "sigp1expclsf";
	// Loop and save the histograms
	for (uint i = 0; i < inputs.size(); ++i){
		TFile* input = inputs.at(i);
		h_CLs.push_back((TH2F*)input->Get(n_CLs.Data()));
	}

	// Loop over to determine the best CLs value
	// and then fill h_final with the corresponding
	// correct xs value.
	TPolyMarker* points = GetGridPoints(Grid);
	int nPoints = points->GetN();
	Double_t* xpts = points->GetX();
	Double_t* ypts = points->GetY();

	for (int np = 0; np < nPoints; ++np){
		float xval = xpts[np];
		float yval = ypts[np];

		float cls_bin = h_CLs[0]->FindBin(xval, yval);
		float cls_min = -999;
		int bestIndex = -1;

		for (uint i = 0; i<h_CLs.size() - 1; ++i){
			for (uint j = i + 1; j<h_CLs.size(); ++j){
				float ith_cls = h_CLs[i]->GetBinContent(cls_bin);
				float jth_cls = h_CLs[j]->GetBinContent(cls_bin);

				if (ith_cls >= jth_cls && ith_cls > cls_min){
					bestIndex = i;
					cls_min = ith_cls;
				}
				else if (jth_cls > ith_cls && jth_cls > cls_min){
					bestIndex = j;
					cls_min = jth_cls;
				}

			}// end loop over j
		}// end loop over i

		// Increment and store the info
		if (bestIndex >= 0){

			// Hack to fix weird point in Mode C w/slepton 
			// grid.
			if (xval == 130 && yval == 30 && Grid == SG_ModeCwslep)
				bestIndex = 0;

			int bin = h_final->FindBin(xval, yval);
			float xs = xsHists.at(bestIndex)->GetBinContent(bin);
			h_final->SetBinContent(bin, xs);
			//cout<<"Setting for mass: "<<xval<<" "<<yval<<" xs: "<<xs<<endl;
		}

	}// end loop over points

	// Map Slepton results
	if (Grid != SG_ModeCwslep) h_final = mapSleptonHist(h_final, Grid);

	c->cd();
	if (Grid == SG_ModeCwslep)
		gStyle->SetPaintTextFormat(".2f");
	else
		gStyle->SetPaintTextFormat(".2f");
	h_final->SetMarkerSize(0.75);
	h_final->Draw("same text");

}
