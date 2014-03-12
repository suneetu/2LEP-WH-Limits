
//////////////////////////////////////////////////////////////////
// Script to contain all of the formatting options for the      //
// plotting macros.  This should be made as generic as possible //
//////////////////////////////////////////////////////////////////

#include "borrowed/CombinationGlob.C"
#include <fstream>
#include "macros/PlotLimits.h"


//------------------------------------------------------------//
// Add decorations to the plot
//------------------------------------------------------------//
void addDecorations(TCanvas* &c, TString atlas, TString process,
		    TString lumi, TString massInfo, TString forbidden,
		    int Grid)
{

  c->cd();
 
  double dx = 0.16, idx = 0.05, dy = 0.86, idy = 0.04;
  
  float textSize = 0.027;

  ATLASLabel(dx,dy,atlas.Data());

  myText(dx,dy-1.2*idy,kBlack,lumi.Data(),textSize);

  // Put this guy in top right
  if(Grid == SG_ModeCwslep){
    myText(dx+0.35, dy-3.7*idy, kBlack,"All limits at 95% CL",0.025);
    //myText(dx+0.35, dy-4.7*idy, kBlack,"All limits at 95% CL",0.025); // For C1C1 + slepslep
    myText(dx, dy-2.5*idy, kBlack,process.Data(),textSize);
    myText(dx, dy-3.7*idy, kBlack,massInfo.Data(),textSize);
  }
  else{
    myText(dx+0.35, dy-3.7*idy, kBlack,"All limits at 95% CL",0.025);
    myText(dx, dy-2.5*idy, kBlack,process.Data(),textSize);
  }

  // Draw text to specify what the forbidden line means
  if(forbidden.Length() != 0){
    TLatex l;
    l.SetTextSize(textSize);
    l.SetNDC();
    l.SetTextColor(kBlack);
    if( Grid == SG_ModeCwslep){
      l.SetTextAngle(48);
      //l.DrawLatex(0.34,0.42,forbidden.Data());
      l.DrawLatex(0.20,0.47,forbidden.Data());
    }
    //if( Grid == SG_Slepton || Grid == SG_RHSlepton){
    else if( Grid == SG_SSWH ){
      l.SetTextAngle(55);
      l.DrawLatex(0.35,0.35,forbidden.Data());
    }
    else{
      l.SetTextAngle(39);
      l.DrawLatex(0.24,0.47,forbidden.Data());
    }
  }

  c->Update();
}


//------------------------------------------------------------//
// Make legend
//------------------------------------------------------------//
TLegend* makeLegend(float x0, float x1, float y0, float y1)
{

  TLegend *leg = new TLegend(x0,y0,x1,y1);
  
  leg->SetTextSize( CombinationGlob::DescriptionTextSize );
  leg->SetTextFont( 42 );
  leg->SetFillColor( 0 );
  //leg->SetFillStyle(1001);
  leg->SetFillStyle(0);
  leg->SetBorderSize( 0 );
  leg->SetTextSize(0.030);
  

  return leg;

}

//------------------------------------------------------------//
// Make Frame
//------------------------------------------------------------//
TH2F* makeFrame(int Grid, int aux=0)
{

  TH2F* frame = new TH2F("frame","#scale[0.8]{SR-mT2a / ee & e#mu & #mu#mu}",
			 800, m_min_x, m_max_x, 800, m_min_y, m_max_y);
  
  // Set the common frame style
  CombinationGlob::SetFrameStyle2D(frame, 1.0);

  if(Grid == SG_ModeCwslep) frame->SetXTitle("m_{#tilde{#chi}_{1}^{#pm}} [GeV]");
  else if(Grid == SG_SSWH)  frame->SetXTitle("m_{#tilde{#chi}_{1}^{#pm},#tilde{#chi}_{2}^{0}} [GeV]");
  else                      frame->SetXTitle("m_{#tilde{l}_{1}^{#pm}} [GeV]");
  //if(Grid == SG_Slepton)    frame->SetXTitle("m(#tilde{l}_{1}^{#pm}) [GeV]");
  //if(Grid == SG_RHSlepton)  frame->SetXTitle("m(#tilde{l}_{1}^{#pm}) [GeV]");
  frame->SetYTitle("m_{#tilde{#chi}_{1}^{0}} [GeV]"); 

  frame->GetXaxis()->SetTitleFont( 42 );
  frame->GetYaxis()->SetTitleFont( 42 );
  frame->GetXaxis()->SetLabelFont( 42 );
  frame->GetYaxis()->SetLabelFont( 42 );

  frame->GetXaxis()->SetTitleSize( 0.04 );
  frame->GetYaxis()->SetTitleSize( 0.04 );
  frame->GetXaxis()->SetLabelSize( 0.04 );
  frame->GetYaxis()->SetLabelSize( 0.04 );

  // Modify slightly what is in SetFramStyle2D
  gPad->SetRightMargin(0.08);
  gPad->SetLeftMargin(0.13);
  frame->GetYaxis()->SetTitleOffset(1.57);
  
  return frame;

}


//------------------------------------------------------------//
// Get Canvas
//------------------------------------------------------------//
TCanvas* makeCanvas(TString name)
{
  TCanvas* c = new TCanvas(name,name,750,700);

  return c;

  // Old way
  //return new TCanvas(name,name,0,0,CombinationGlob::StandardCanvas[0],		     
  //CombinationGlob::StandardCanvas[1]);
  
}


//------------------------------------------------------------//
// Fix and set the borders for contour
//------------------------------------------------------------//
TH2F* FixAndSetBorders(const TH2& hist, const char* name=0, const char* title=0, Double_t val=0 )
{
  TH2F* hist0 = hist.Clone(); // histogram we can modify
  
  MirrorBorders( *hist0 );    // mirror values of border bins into overflow bins
  
  TH2F* hist1 = AddBorders( *hist0, "hist1", "hist1" );   
  // add new border of bins around original histogram,
  // ... so 'overflow' bins become normal bins
  SetBorders( *hist1, val );                              
  // set overflow bins to value 1
  
  TH2F* histX = AddBorders( *hist1, "histX", "histX" );   
  // add new border of bins around original histogram,
  // ... so 'overflow' bins become normal bins
  
  TH2F* hist3 = histX->Clone();
  hist3->SetName( name!=0 ? name : "hist3" );
  hist3->SetTitle( title!=0 ? title : "hist3" );
  
  delete hist0; delete hist1; delete histX;
  //hist3->Smooth();
  return hist3; // this can be used for filled contour histograms
}

//------------------------------------------------------------//
// Mirror Borders
//------------------------------------------------------------//
void MirrorBorders( TH2& hist )
{
  int numx = hist.GetNbinsX();
  int numy = hist.GetNbinsY();
  
  Float_t val;
  // corner points
  hist.SetBinContent(0,0,hist.GetBinContent(1,1));
  hist.SetBinContent(numx+1,numy+1,hist.GetBinContent(numx,numy));
  hist.SetBinContent(numx+1,0,hist.GetBinContent(numx,1));
  hist.SetBinContent(0,numy+1,hist.GetBinContent(1,numy));
  
  for(int i=1; i<=numx; i++){
    hist.SetBinContent(i,0,	   hist.GetBinContent(i,1));
    hist.SetBinContent(i,numy+1, hist.GetBinContent(i,numy));
  }
  for(int i=1; i<=numy; i++) {
    hist.SetBinContent(0,i,      hist.GetBinContent(1,i));
    hist.SetBinContent(numx+1,i, hist.GetBinContent(numx,i));
  }
}

//------------------------------------------------------------//
// Add Borders
//------------------------------------------------------------//
TH2F* AddBorders( const TH2& hist, const char* name=0, const char* title=0)
{
  int nbinsx = hist.GetNbinsX();
  int nbinsy = hist.GetNbinsY();
  
  double xbinwidth = ( hist.GetXaxis()->GetBinCenter(nbinsx) - hist.GetXaxis()->GetBinCenter(1) ) / double(nbinsx-1);
  double ybinwidth = ( hist.GetYaxis()->GetBinCenter(nbinsy) - hist.GetYaxis()->GetBinCenter(1) ) / double(nbinsy-1);
  
  double xmin = hist.GetXaxis()->GetBinCenter(0) - xbinwidth/2. ;
  double xmax = hist.GetXaxis()->GetBinCenter(nbinsx+1) + xbinwidth/2. ;
  double ymin = hist.GetYaxis()->GetBinCenter(0) - ybinwidth/2. ;
  double ymax = hist.GetYaxis()->GetBinCenter(nbinsy+1) + ybinwidth/2. ;
  
  TH2F* hist2 = new TH2F(name, title, nbinsx+2, xmin, xmax, nbinsy+2, ymin, ymax);
  
  for (Int_t ibin1=0; ibin1 <= hist.GetNbinsX()+1; ibin1++) {
    for (Int_t ibin2=0; ibin2 <= hist.GetNbinsY()+1; ibin2++)
      hist2->SetBinContent( ibin1+1, ibin2+1, hist.GetBinContent(ibin1,ibin2) );
  }
  
  return hist2;
}
//------------------------------------------------------------//
// Set Borders
//------------------------------------------------------------//
void SetBorders( TH2 &hist, Double_t val=0 )
{
  int numx = hist.GetNbinsX();
  int numy = hist.GetNbinsY();
  
  for(int i=0; i <= numx+1 ; i++){
    hist.SetBinContent(i,0,val);
    hist.SetBinContent(i,numy+1,val);
  }
  for(int i=0; i <= numy+1 ; i++) {
    hist.SetBinContent(0,i,val);
    hist.SetBinContent(numx+1,i,val);
  }
}

//------------------------------------------------------------//
// Draw the contour with 95% confidence intervals
//------------------------------------------------------------//
void DrawContourLine95( TLegend *leg, TH2F* hist, const TString& text="", Int_t linecolor=0, Int_t linestyle=2, Int_t linewidth=2)
{
  // contour plot
  TH2F* h = new TH2F( *hist );
  h->SetContour( 1 );
  double pval = CombinationGlob::cl_percent[1];
  //h->SetContourLevel( 0, pval );
  double signif = TMath::NormQuantile(1-pval);
  h->SetContourLevel( 0, signif );

  h->SetLineColor( linecolor );
  h->SetLineWidth( linewidth );
  h->SetLineStyle( linestyle );
  h->Draw( "samecont3" );

  if (!text.IsNull()) leg->AddEntry(h,text.Data(),"l"); 
}

//------------------------------------------------------------//
// Get Contour Graph
//------------------------------------------------------------//
TGraph* ContourGraph(TH2F* hist)
{

  TCanvas* c_temp       = makeCanvas("c_temp");
  TGraph* gr0  		= new TGraph();
  TH2F* histInternal  	= (TH2F*)  hist->Clone();
  TGraph* gr1  		= (TGraph*) gr0->Clone(histInternal->GetName());


  histInternal 		->SetContour( 1 );
  double pval  		= CombinationGlob::cl_percent[1];
  double signif 	= TMath::NormQuantile(1-pval);
  histInternal		->SetContourLevel( 0, signif );
  histInternal		->Draw("CONT LIST");

  histInternal 		->SetDirectory(0);
  gPad 			->Update();

  TObjArray *contours 	= gROOT->GetListOfSpecials()->FindObject("contours");
  Int_t ncontours     	= contours->GetSize();

  TList *list 		= (TList*)contours->At(0);
  delete c_temp;
  if( list->GetSize() == 0) return gr1;
  gr1 			= (TGraph*)list->First();
  gr1 			->SetName(hist->GetName());

  Double_t x, y = 0;
  for(int ip=0; ip<gr1->GetN(); ++ip){
    gr1->GetPoint(ip,x,y);
    //cout<<"point: "<<ip<<" ("<<x<<","<<y<<")"<<endl;
  }

  return gr1;
}

//------------------------------------------------------------//
// Draw Expected Band
//------------------------------------------------------------//
TGraph* DrawExpectedBand( TGraph* gr1,  TGraph* gr2, Int_t fillColor, Int_t fillStyle, Int_t cut = 0)
{
  //  TGraph* gr1 = new TGraph( *graph1 );
  //  TGraph* gr2 = new TGraph( *graph2 );

  int number_of_bins = max(gr1->GetN(),gr2->GetN());
  
  const Int_t gr1N = gr1->GetN();
  const Int_t gr2N = gr2->GetN();

  if( gr1N == 0 || gr2N == 0 ) return NULL;

  const Int_t N = number_of_bins;
  Double_t x1[N], y1[N], x2[N], y2[N];

  Double_t xx0, yy0;

  for(int j=0; j<gr1N; j++) {
    gr1->GetPoint(j,xx0,yy0);
      x1[j] = xx0;
      y1[j] = yy0;
  }
  if (gr1N < N) {
    for(int i=gr1N; i<N; i++) {
      x1[i] = x1[gr1N-1];
      y1[i] = y1[gr1N-1];
    }      
  }

  Double_t xx1, yy1;

  for(int j=0; j<gr2N; j++) {
    gr2->GetPoint(j,xx1,yy1);
      x2[j] = xx1;
      y2[j] = yy1;
  }
  if (gr2N < N) {
    for(int i=gr2N; i<N; i++) {
      x2[i] = x2[gr1N-1];
      y2[i] = y2[gr1N-1];
    }      
  }

  
  TGraph *grshade = new TGraphAsymmErrors(2*N);

  for (int i=0;i<N;i++) {
    if (x1[i] > cut)
      grshade->SetPoint(i,x1[i],y1[i]);
    if (x2[N-i-1] > cut)
      grshade->SetPoint(N+i,x2[N-i-1],y2[N-i-1]);
  }

  // Apply the cut in the shade plot if there is something that doesn't look good...
  int Nshade = grshade->GetN();
  double x0, y0;
  double x00, y00;

  for(int j=0; j<Nshade; j++) {
    grshade->GetPoint(j,x0,y0);
    if ((x0 != 0) && (y0 != 0)) {
      x00 = x0;
      y00 = y0;
      break;
    }
  }

  for(int j=0; j<Nshade; j++) {
    grshade->GetPoint(j,x0,y0);
    if ((x0 == 0) && (y0 == 0)) 
      grshade->SetPoint(j,x00,y00);
  }


  // Now draw the plot... 
  grshade->SetFillStyle(fillStyle);
  grshade->SetFillColor(fillColor);
  //  grshade->SetMarkerStyle(21);
  grshade->Draw("F");
  return grshade;
}

//------------------------------------------------------------//
// Make Lep2 exclusion for mode C
//------------------------------------------------------------//
TGraph* DrawLEPLimit(int Grid)
{

  // Build TGraph Object
  TGraph* LEP2 = new TGraphAsymmErrors(3);
  LEP2->SetFillColor(kOrange+8);
  LEP2->SetLineColor(kOrange+8);

  // Take care of ModeC
  if( Grid == SG_ModeCwslep ){
    LEP2->SetPoint(0,0,0);
    LEP2->SetPoint(1,m_LEP2_MODECwslep,m_LEP2_MODECwslep);
    LEP2->SetPoint(2,m_LEP2_MODECwslep,0);
    LEP2->SetName(m_LEP2_MODECwslep_Label);
  }
  else if(Grid == SG_DGemt100){
	  TFile* fLEP = TFile::Open( "data/DGemt_LEPlimitC1_to500_M1eq100.root", "READ" );
	  TGraph* histLEPSwapGraph = (TGraph*)fLEP->Get("LEPlimit_C1_M1eq100");
	  fLEP->Close();

	  int nbins = histLEPSwapGraph->GetN();
	  Double_t *vx = histLEPSwapGraph->GetX();
	  Double_t *vy = histLEPSwapGraph->GetY();

	  LEP2->SetPoint(0, 100, 100);
	  LEP2->SetPoint(1, 100, vy[0]);
	  for(Int_t i=0; i<nbins; i++)
		  LEP2->SetPoint(i+2, vx[i], vy[i]);

	  LEP2->SetPoint(nbins+2, vx[nbins-1], 100);
  }
  else if(Grid == SG_DGemt140){
	  TFile* fLEP = TFile::Open( "data/DGemt_LEPlimitC1_to500_M1eq140.root", "READ" );
	  TGraph* histLEPSwapGraph = (TGraph*)fLEP->Get("LEPlimit_C1_M1eq140");
	  fLEP->Close();

	  int nbins = histLEPSwapGraph->GetN();
	  Double_t *vx = histLEPSwapGraph->GetX();
	  Double_t *vy = histLEPSwapGraph->GetY();

	  LEP2->SetPoint(0, 100, 100);
	  LEP2->SetPoint(1, 100, vy[0]);
	  for(Int_t i=0; i<nbins; i++)
		  LEP2->SetPoint(i+2, vx[i], vy[i]);

	  LEP2->SetPoint(nbins+2, vx[nbins-1], 100);
  }
  else if(Grid == SG_DGemt250){
	  TFile* fLEP = TFile::Open( "data/DGemt_LEPlimitC1_to500_M1eq250.root", "READ" );
	  TGraph* histLEPSwapGraph = (TGraph*)fLEP->Get("LEPlimit_C1_M1eq250");
	  fLEP->Close();

	  int nbins = histLEPSwapGraph->GetN();
	  Double_t *vx = histLEPSwapGraph->GetX();
	  Double_t *vy = histLEPSwapGraph->GetY();

	  LEP2->SetPoint(0, 100, 100);
	  LEP2->SetPoint(1, 100, vy[0]);
	  for(Int_t i=0; i<nbins; i++)
		  LEP2->SetPoint(i+2, vx[i], vy[i]);

	  LEP2->SetPoint(nbins+2, vx[nbins-1], 100);
  }
  
  //if( Grid == SG_Slepton || Grid == SG_RHSlepton ){
  else{
    // Read in points from file
    ifstream input("data/slepton_muR_lep2Limit.txt",ios_base::in);
    int n = 0;
    float x = 0;
    float y = 0;
    while( input.good() ){
      input >> x >>  y;
      LEP2->SetPoint(n,x,y);
      n++;
    }
    LEP2->SetName(m_LEP2_Slepton_Label);
  }


  LEP2->Draw("F");
  return LEP2;

}

//------------------------------------------------------------//
// Dummy Legend Expected
//------------------------------------------------------------//
void DummyLegendExpected(TLegend* leg, TString what,  Int_t fillColor, Int_t fillStyle, Int_t lineColor, Int_t lineStyle, Int_t lineWidth)
{

  TGraph* gr = new TGraph();
  gr->SetFillColor(fillColor);
  gr->SetFillStyle(fillStyle);
  gr->SetLineColor(lineColor);
  gr->SetLineStyle(lineStyle);
  gr->SetLineWidth(lineWidth);
  leg->AddEntry(gr,what,"LF");
}

//------------------------------------------------------------//
// Get C1C1 Grid Points
//------------------------------------------------------------//
TPolyMarker* GetGridPoints(int Grid)
{

  int n = 0;
  double xx[500], yy[500];  
  double x;
  double y;
  double z;

  // Mode C
  if(Grid == SG_ModeCwslep){
    ifstream input("data/ModeC_mC1_mN1.txt",ios_base::in);  
    while( input.good() ){
      input >> x >>  y;
      xx[n] = x;
      yy[n] = y;
      n++;
    }
  }
  // SSWH
  if(Grid == SG_SSWH){
    ifstream input("data/SSWH.txt",ios_base::in);  
    while( input.good() ){
      input >> x >>  y;
      xx[n] = x;
      yy[n] = y;
      n++;
    }
  }
  if(Grid == SG_Slepton || Grid == SG_RHSlepton || Grid == SG_LHSlepton ||
     Grid == SG_Selectron || Grid == SG_RHSelectron || Grid == SG_LHSelectron || 
     Grid == SG_Smuon || Grid == SG_RHSmuon || Grid == SG_LHSmuon ){
    //ifstream input("data/Slepton_mSl_mN.txt",ios_base::in);
    ifstream input("data/Slepton_mSl_mN_Sparse.txt",ios_base::in);
    while( input.good() ){
      
      input >> x >>  y;
      
      xx[n] = x;
      yy[n] = y;
      n++;
    }
  }
  if(Grid == SG_DGemt100){
	  ifstream input("data/pMSSM_DGemt_Masses.txt",ios_base::in);
	  while( input.good() ){
		  input >> z >>  x >> y;
		  if(z == 100){
			  xx[n] = x;
			  yy[n] = y;
			  n++;
		  }
	  }
  }
  if(Grid == SG_DGemt140){
	  ifstream input("data/pMSSM_DGemt_Masses.txt",ios_base::in);
	  while( input.good() ){
		  input >> z >>  x >> y;
		  if(z == 140){
			  xx[n] = x;
			  yy[n] = y;
			  n++;
		  }
	  }
  }
  if(Grid == SG_DGemt250){
	  ifstream input("data/pMSSM_DGemt_Masses.txt",ios_base::in);
	  while( input.good() ){
		  input >> z >>  x >> y;
		  if(z == 250){
			  xx[n] = x;
			  yy[n] = y;
			  n++;
		  }
	  }
  }

  TPolyMarker* gridpoints = new TPolyMarker(n,xx,yy);
  
  return gridpoints;
}

//------------------------------------------------------------//
// Map Slepton results
//------------------------------------------------------------//
TPolyMarker* mapSleptonGrid(TPolyMarker* current, int Grid)
{
  
  int n = 0;
  double xx[500], yy[500];
  double x, y;
  
  ifstream input("data/DLiSlep_EWK_Masses.txt");
  
  // New input format has 6 quantities per row
  int DSID         = 0;
  float GUT_mSl    = 0;
  float GUT_mN1    = 0;
  float EWK_mSl_LH = 0;
  float EWK_mSl_RH = 0;
  float EWK_mN1    = 0;

  // Get the current points
  int cur_n          = current->GetN();
  Double_t* cur_mSls = current->GetX();
  Double_t* cur_mN1s = current->GetY();

  // Threshold
  float threshold = 0.1;

  while( input.good() ){

    input >> DSID >> GUT_mSl >> GUT_mN1 
	  >> EWK_mSl_LH >> EWK_mSl_RH >> EWK_mN1;

    bool keep = false;
    for(int i=0; i<cur_n; ++i){
      double cur_mSl = cur_mSls[i];
      double cur_mN1 = cur_mN1s[i];
      
      if( !(fabs(GUT_mSl-cur_mSl) < threshold) ) continue;
      if( !(fabs(GUT_mN1-cur_mN1) < threshold) ) continue;
      //cout<<"Mapping point: "<<GUT_mSl<<" --> "<<EWK_mSl_RH<<endl;
      keep = true;
    }

    // If this point is in the set then
    // keep it and make sure to pick
    // RH or LH appropriately 
    if( keep ){

      if(Grid == SG_Slepton || Grid == SG_Selectron || Grid == SG_Smuon){
	if( EWK_mSl_LH < EWK_mSl_RH ) x = EWK_mSl_LH;
	else                          x = EWK_mSl_RH;
      }
      else if(Grid == SG_RHSlepton || Grid == SG_RHSelectron || Grid == SG_RHSmuon){
	x = EWK_mSl_RH;
      }
      else if(Grid == SG_LHSlepton || Grid == SG_LHSelectron || Grid == SG_LHSmuon){
	x = EWK_mSl_LH;
      }
      
      // Neutralino mass has only one option
      y = EWK_mN1;

      // Save
      xx[n] = x;
      yy[n] = y;
      n++;
    }// end if keep

  }// end if input is good

  // Create marker object
  TPolyMarker* gridpoints = new TPolyMarker(n,xx,yy);
  gridpoints->SetMarkerStyle(20);
  gridpoints->SetMarkerSize(1.1);
  gridpoints->SetMarkerColor( current->GetMarkerColor() );
  gridpoints->SetMarkerStyle( current->GetMarkerStyle() );

  return gridpoints;

}

//------------------------------------------------------------//
// Map Slepton histogram
//------------------------------------------------------------//
TH2F* mapSleptonHist(TH2F* current, int Grid)
{

  // Clone the histogram 
  TH2F* newHist = (TH2F*) current->Clone("cloned_sleptonHist");
  newHist->Reset();
  
  ifstream input("data/DLiSlep_EWK_Masses.txt");
  
  // New input format has 6 quantities per row
  int DSID         = 0;
  float GUT_mSl    = 0;
  float GUT_mN1    = 0;
  float EWK_mSl_LH = 0;
  float EWK_mSl_RH = 0;
  float EWK_mN1    = 0;

  while( input.good() ){

    input >> DSID >> GUT_mSl >> GUT_mN1 
	  >> EWK_mSl_LH >> EWK_mSl_RH >> EWK_mN1;


    // Find the bin location for the GUT masses
    int bin = current->FindBin(GUT_mSl, GUT_mN1);
    
    // Get the value stored in this bin
    float bc  = current->GetBinContent(bin);

    // Set the new histogram point based on RH/LH 
    // nature of the grid being scanned.
    float mSl_fill = 0;
    if(Grid == SG_Slepton || Grid == SG_Selectron || Grid == SG_Smuon){
      if( EWK_mSl_LH < EWK_mSl_RH ) mSl_fill = EWK_mSl_LH;
      else                          mSl_fill = EWK_mSl_RH;
    }
    else if(Grid == SG_RHSlepton || Grid == SG_RHSelectron || Grid == SG_RHSmuon){
      mSl_fill = EWK_mSl_RH;
    }
    else if(Grid == SG_LHSlepton || Grid == SG_LHSelectron || Grid == SG_LHSmuon){
      mSl_fill = EWK_mSl_LH;
    }
    
    // Fill new bin
    newHist->Fill(mSl_fill, EWK_mN1, bc);
    //cout<<"Filling new Hist: "<<mSl_fill<<" "<<EWK_mN1<<" "<<bc<<endl;

  }// end if input good
  
  return newHist;

}

//------------------------------------------------------------//
TGraph* mapSlepToCharginoGraph(TGraph* gr)
{

  double m_char[1000];
  double m_N1[1000];
  int npts = gr->GetN();
  cout<<"Npts: "<<npts<<endl;

  Double_t xpt = 0;
  Double_t ypt = 0;
  for(int ip=0; ip<npts; ++ip){
    
    gr->GetPoint(ip,xpt,ypt);
    m_char[ip] = 2 * (xpt - 0.5 * ypt);
    m_N1[ip] = ypt;

  }

  TGraph* gr_new = new TGraph(npts, m_char, m_N1);
  return gr_new;
  
}
  

//------------------------------------------------------------//
// Map Slepton histogram
//------------------------------------------------------------//
TH2F* mapSleptonToCharginoHist(TH2F* current, TString tag)
{

  // Clone the histogram 
  TH2F* newHist = (TH2F*) current->Clone(("cloned_sleptonHist_"+tag).Data());
  newHist->Reset();
  
  ifstream input("data/DLiSlep_EWK_Masses.txt");
  
  // New input format has 6 quantities per row
  int DSID         = 0;
  float GUT_mSl    = 0;
  float GUT_mN1    = 0;
  float EWK_mSl_LH = 0;
  float EWK_mSl_RH = 0;
  float EWK_mN1    = 0;

  while( input.good() ){

    input >> DSID >> GUT_mSl >> GUT_mN1 
	  >> EWK_mSl_LH >> EWK_mSl_RH >> EWK_mN1;


    // Find the bin location for the GUT masses
    int bin = current->FindBin(GUT_mSl, GUT_mN1);
    
    // Get the value stored in this bin
    float bc  = current->GetBinContent(bin);

    // Get CHargino Mass
    float m_char = 2 * (EWK_mSl_LH - 0.5 * EWK_mN1);
    
    // Fill new bin
    newHist->Fill(m_char, EWK_mN1, bc);
    //cout<<"Filling new Hist: "<<mSl_fill<<" "<<EWK_mN1<<" "<<bc<<endl;

  }// end if input good
  
  return newHist;

}

//------------------------------------------------------------//
// Map slepton to Chargino
//------------------------------------------------------------//
TH2F* mapSleptonToCharginoHist2(TH2F* current, TString tag)
{

  // Clone the histogram 
  TH2F* newHist = (TH2F*) current->Clone(("cloned_sleptonHist_toChargino"+tag).Data());
  newHist->Reset();
  
  int nxbins = current->GetNbinsX();
  int nybins = current->GetNbinsY();
  for(int xbin = 1; xbin<=nxbins; ++xbin){
    for(int ybin = 1; ybin<=nybins; ++ybin){
      
      // Get Slepton Mass
      float xval = current->GetXaxis()->GetBinCenter(xbin);
      // Get Neutralino Mass
      float yval = current->GetYaxis()->GetBinCenter(ybin);
      // Get Chargino Mass
      float newx = 2 * (xval - 0.5*yval);
      
      // Get weight
      float bc = current->GetBinContent(xbin,ybin);
      
      newHist->Fill(newx, yval, bc);
    }// end loop over ybin
  }// end loop over xbin
  
  return newHist;
  
}

//------------------------------------------------------------//
// Get the forbidden line.  Sounds sexy.
//------------------------------------------------------------//
TLine* getForbiddenLine(float beginx, float beginy, float endx, float endy, int Grid)
{
  
  float slope = 1;
  float y = slope*beginx;
  TLine* forbidden = new TLine(beginx,y,endy,endy*slope);
  forbidden->SetLineStyle(9);
  forbidden->SetLineWidth(2);
  forbidden->SetLineColor(kBlack);
  return forbidden;


}

//------------------------------------------------------------//
// Get the conf results
//------------------------------------------------------------//
TH2D* getConfResults(int Grid, bool isExpected)
{

  // Specify the histogram
  TString h_name = isExpected ? "sigp1expclsf" : "sigp1clsf";

  // Get right file
  TString path = "data/2L_Output_hypotest__1_harvest_list_";
  
  if(Grid == SG_ModeCwslep) path += "ModeC.root";
  if(Grid == SG_Slepton)    path += "LHplusRH.root";
  if(Grid == SG_RHSlepton)  path += "RHOnly.root";
  if(Grid == SG_LHSlepton)  path += "LHOnly.root";

  // Open file and get results
  TFile* file = new TFile( path.Data() );  
  TH2D* confResult = (TH2D*) file->Get( h_name.Data() );
  confResult->SetDirectory(0);
  file->Close();

  // Set some attributes
  double signif = TMath::NormQuantile(0.95);
  confResult->SetContour(1);
  confResult->SetContourLevel(0,signif);
  int color = isExpected ? kGreen + 2 : kMagenta;
  confResult->SetLineColor(color);
  confResult->SetLineWidth(2);

  return confResult;
}
  
//------------------------------------------------------------//
// Get the 7 TeV Paper result
//------------------------------------------------------------//
TH2F* getATLAS7TeVResult()
{
  
  // Open file under data/ and read in the 7 TeV result
  TFile* input    = new TFile("data/ATLAS7TeV.root");
  TGraph2D* graph = (TGraph2D*) input->Get("oldcont");
  TH2F* histo     = (TH2F*) graph->GetHistogram();

  histo->SetDirectory(0);
  histo->SetLineColor(kBlue);

  //delete graph;
  //input->Close();
  //delete input;

  return histo;

}

//------------------------------------------------------------//
// Get upper limit on XS
//------------------------------------------------------------//
void getXSUpperLimit(TString base, int SR, int Grid, TH2F* &hist)
{
  
  // Setup the text file to read in the xs
  // and the mass info
  TString f_name = "data/";
  if(Grid == SG_ModeCwslep) f_name += "ModeCXSInfo.txt";
  else                      f_name += "SleptonXSInfo.txt";

  //cout<<"Opening file: "<<f_name<<endl;

  fstream input (f_name.Data(),fstream::in);
  
  // Now setup the root file to grab from
  TString rf_name = SRNames[SR]+"_"+base+"_"+SGNames[Grid]+"_NoSys";
  TFile* rf_input = new TFile(("limitResults/"+rf_name+"/"+rf_name+"_Output_upperlimit.root").Data()); 
  
  char dummy[180];
  //cout<<"SR: "<<SRNames[SR]<<endl;
  // First line is dummy
  if( Grid == SG_ModeCwslep ){

    float mC1 = 0;
    float mN1 = 0;
    float xs     = 0;
    int dsid     = 0;

    input >> dummy >> dummy >> dummy >> dummy;

    while( input.good() ){
      
      // Get XS and mass
      input >> dsid >> mC1 >> mN1 >> xs;
      //xs = xs*1000; // convert to fb
      
      // Get the Upper limit
      TString* masses = new TString(Form("%.1f_%.1f",mC1,mN1));
      float upperLimit = getUpperLimit(rf_input,Grid,*masses);
      
      //cout<<mC1<<" "<<mN1<<" "<<xs<<" "<<upperLimit<<" "<<xs*upperLimit<<endl;
      delete masses;
      
      hist->Fill(mC1,mN1,xs*upperLimit);
      
    }

  }

  else{
    
    //float mSl   = 0;
    //float mN1   = 0;
    char mSl[180];
    char mN1[180];
    float LHxs  = 0;
    float RHxs  = 0;
    int dsid    = 0;

    input >> dummy >> dummy >> dummy >> dummy >> dummy;

    while( input.good() ){
      
      // Get XS and mass
      input >> dsid >> mSl >> mN1 >> RHxs >> LHxs;
      //xs = xs*1000; // convert to fb
      
      // Get the Upper limit
      //TString* masses = new TString(Form("%.1f_%.1f",mSl,mN1));
      TString* masses = new TString(Form("%s_%s",mSl,mN1));
      float upperLimit = getUpperLimit(rf_input,Grid,*masses);
      
      //cout<<mSl<<" "<<mN1<<" "<<RHxs<<" "<<LHxs<<" "<<upperLimit<<endl;
      delete masses;
      
      // Determine what XS to use for this case
      float fillvalue = 0;
      if( Grid == SG_Slepton || Grid == SG_Selectron || Grid == SG_Smuon )
	fillvalue = (LHxs+RHxs) * upperLimit;
      else if( Grid == SG_RHSlepton || Grid == SG_RHSelectron || Grid == SG_RHSmuon )
	fillvalue = RHxs * upperLimit;
      else if( Grid == SG_LHSlepton || Grid == SG_LHSelectron || Grid == SG_LHSmuon )
	fillvalue = RHxs * upperLimit;
      else
	cout<<"Grid not supported, filling 0 for xs limit"<<endl;
      
      hist->Fill(atof(mSl),atof(mN1),fillvalue * 1000);
      
    }

  }


}

//------------------------------------------------------------//
// Get Sys error for each point
//------------------------------------------------------------//
void getSysPerPoint(TString base, int SR, int Grid, TH2F* &hist)
{
  
  // Setup the text file to read in the xs
  // and the mass info
  TString f_name = "data/";
  if(Grid == SG_ModeCwslep)       f_name += "SMCwslep_" + SRNames[SR] + "_tot.txt";
  else if(Grid == SG_Slepton)     f_name += "SparseDLiSlep_" + SRNames[SR] + "_tot.txt";
  else if(Grid == SG_RHSlepton)   f_name += "SparseDLiSlep_ROnly_" + SRNames[SR] + "_tot.txt";
  else if(Grid == SG_LHSlepton)   f_name += "SparseDLiSlep_LOnly_" + SRNames[SR] + "_tot.txt";
  else if(Grid == SG_Selectron)   f_name += "SparseDLiSlep_" + SRNames[SR] + "_ee.txt";
  else if(Grid == SG_RHSelectron) f_name += "SparseDLiSlep_ROnly_" + SRNames[SR] + "_ee.txt";
  else if(Grid == SG_LHSelectron) f_name += "SparseDLiSlep_LOnly_" + SRNames[SR] + "_ee.txt";
  else if(Grid == SG_Smuon)       f_name += "SparseDLiSlep_" + SRNames[SR] + "_mm.txt";
  else if(Grid == SG_RHSmuon)     f_name += "SparseDLiSlep_ROnly_" + SRNames[SR] + "_mm.txt";
  else if(Grid == SG_LHSmuon)     f_name += "SparseDLiSlep_LOnly_" + SRNames[SR] + "_mm.txt";
  else return;


  // The input style is the same
  // m1 m2 sys
  fstream input (f_name.Data(),fstream::in);
  //cout<<"-----------------------------"<<endl;
  //cout<<"Loading input: "<<f_name<<endl;

  float m1  = 0;
  float m2  = 0;
  float sys = 0;
  
  while( input.good() ){
    
    input >> m1 >> m2 >> sys;

    hist->Fill(m1, m2, sys);
    //cout<<"Filling: "<<m1<<" "<<m2<<" weight: "<<sys<<endl;
  }

}

//------------------------------------------------------------//
// Get the upper limit given an input file
//------------------------------------------------------------//
float getUpperLimit(TFile* file, int Grid, TString masses)
{

  TString name = "hypo_";
  
  if(Grid == SG_ModeCwslep) name += "SMCwslep8TeV";
  else                      name += "DLiSlep";    
  //if(Grid == SG_Slepton)    name += "DLiSlep";
  //if(Grid == SG_RHSlepton)  name += "DLiSlep";
  //if(Grid == SG_LHSlepton)  name += "DLiSlep";

  name += "_" + masses;
  //cout<<"Getting: "<<name<<endl;
  RooStats::HypoTestInverterResult* result = (RooStats::HypoTestInverterResult*) file->Get(name.Data());
  
  float limit = result ? result->UpperLimit() : 0.0;
  if( !result ) cout<<"No Result for: "<<masses<<endl;
  
  delete result;
  return limit;

}

//------------------------------------------------------------//
// Draw hacked in Z axis
//------------------------------------------------------------//
void drawZaxis(TCanvas* &c, int aux, int Grid)
{

  c->cd();
  
  TString title = "";
  float x = 0;
  float y = 0;
  if( aux == Aux_CLs){ 
    title = "Numbers give CL_{S} values";
    x = 0.97;
    y = 0.6;
  }
  else if( aux == Aux_XS){
    if(Grid == SG_ModeCwslep) title = "Numbers give 95% excluded model cross sections [pb]";
    else                      title = "Numbers give 95% excluded model cross sections [fb]";
    x = 0.97;
    y = 0.2;
  }
  else if( aux == Aux_Sys ){ 
    title = "Numbers give total error [%]";
    x = 0.97;
    y = 0.55;
  }
  else return;

  TLatex l =  TLatex();
  l.SetNDC();
  l.SetTextSize(0.030);
  l.SetNDC();
  l.SetTextColor(kBlack);  
  l.SetTextAngle(90);
  l.DrawLatex(x, y, title.Data());

  return;
}


//------------------------------------------------------------//
// Hack Limits
//------------------------------------------------------------//
void hackLimits(vector<TH2F*> &hists, int Grid, bool isExpected)
{

  // This method is to remove islands in the figures.
  // This doesn't need to be run for all grids and points
  // so user will just add the exceptions in here...

  vector< pair<float,float> > masses;

  // Turn off ModeC hacking for now. Using
  // smoothing to fill in the gaps
  if( false && Grid == SG_ModeCwslep ){

    // Next get rid of islands inside the figure
    masses.clear();
    masses.push_back( pair<float,float> (395,135) );
    masses.push_back( pair<float,float> (410,150) );
    masses.push_back( pair<float,float> (425,165) );
    resetHists(hists, masses, !isExpected, 1.65, true);

    masses.push_back( pair<float,float> (395,125) );
    masses.push_back( pair<float,float> (407,136) );
    masses.push_back( pair<float,float> (420,155) );
    masses.push_back( pair<float,float> (425,122) );

    resetHists(hists, masses, isExpected, 1.65);





  }

  if( Grid == SG_LHSelectron ){


    // First loop over points we want to remove
    // that are outside the exclusion curve.
    masses.clear();
    masses.push_back( pair<float,float> (250,130) );
    masses.push_back( pair<float,float> (251,129) );
    resetHists(hists, masses, isExpected, 1.0);

    // Next get rid of islands inside the figure
    masses.clear();
    masses.push_back( pair<float,float> (254,64) );
    masses.push_back( pair<float,float> (251,60) );
    masses.push_back( pair<float,float> (250,60) );
    masses.push_back( pair<float,float> (249,60) );
    masses.push_back( pair<float,float> (258,13) );
    masses.push_back( pair<float,float> (254,8) );
    masses.push_back( pair<float,float> (255,13) );

    resetHists(hists, masses, isExpected, 1.65);

  }// end if LH Selectron

  if( Grid == SG_RHSmuon){


    // First take care of expected
    masses.clear();
    masses.push_back( pair<float,float> (156,40) );
    resetHists(hists, masses, (!isExpected), 1.0);

    // Take care of observed
    masses.clear();
    masses.push_back( pair<float,float> (156,30) );
    masses.push_back( pair<float,float> (165,50) );
    masses.push_back( pair<float,float> (170,50) );
    masses.push_back( pair<float,float> (163,46) );
    resetHists(hists, masses, isExpected, 1.65);

    masses.clear();
    masses.push_back( pair<float, float> (205,20) );
    masses.push_back( pair<float, float> (220,20) );
    resetHists(hists,masses, isExpected, 1.55);

  }// end if RH Smuon

}

//------------------------------------------------------------//
// Loop and reset histograms
//------------------------------------------------------------//
void resetHists(vector<TH2F*> &hists, vector< pair<float,float> > masses,
		bool NoResetThisHistSet, float newBC,
		bool skipInner==false)
{

  for(uint im=0; im<masses.size(); ++im){
    float mX = masses.at(im).first;  // X-axis
    float mY = masses.at(im).second;  // Y-axis
    
    for(uint ih=0; ih<hists.size(); ++ih){
      if(NoResetThisHistSet) continue;
      if(skipInner && ih == 1) continue;
      resetBin(hists.at(ih), mX, mY, newBC);
    }
  }

}

//------------------------------------------------------------//
// Hack a particular grid point
//------------------------------------------------------------//
int hackPoint(TH2F* hist, int Grid, int xbin, int ybin)
{

  // Deal with SMCwslep first
  if( Grid == SG_ModeCwslep ){
    
    vector< pair<float,float> > points;             vector<int> whichGrid;
    points.push_back( pair<float,float> (130, 30) ); whichGrid.push_back(0);
    
    for(uint ip=0; ip<points.size(); ++ip){
      int p_xbin = hist->GetXaxis()->FindBin( points.at(ip).first );
      int p_ybin = hist->GetYaxis()->FindBin( points.at(ip).second );
      
      if( p_xbin == xbin && p_ybin == ybin) return whichGrid.at(ip);
      
    }// end loop over points
    
  }// end if mode C
  
  // Deal with Slepton Grids
  if( Grid == SG_Slepton || Grid == SG_Smuon || Grid == SG_LHSlepton ||
      Grid == SG_LHSmuon){
    
    vector< pair<float,float> > points;             vector<int> whichGrid;
    points.push_back( pair<float,float> (98, 60) ); whichGrid.push_back(0);
    points.push_back( pair<float,float> (195, 160) ); whichGrid.push_back(0);
    
    for(uint ip=0; ip<points.size(); ++ip){
      int p_xbin = hist->GetXaxis()->FindBin( points.at(ip).first );
      int p_ybin = hist->GetYaxis()->FindBin( points.at(ip).second );
      
      if( p_xbin == xbin && p_ybin == ybin) return whichGrid.at(ip);
      
    }// end loop over points
    
  }// end if mode C
  
  return -1;
  
}

//------------------------------------------------------------//
// Reset bin
//------------------------------------------------------------//
void resetBin(TH2F* &hist, float xpos, float ypos, float newBC)
{

  int bin = hist->FindBin(xpos,ypos);
  hist->SetBinContent(bin, newBC);

}

//------------------------------------------------------------//
// Dump the bins
//------------------------------------------------------------//
void dumpBins(TH2F* hist)
{

  int nxbins = hist->GetNbinsX();
  int nybins = hist->GetNbinsY();
  
  for(int binx=1; binx<=nxbins; ++binx){
    for(int biny=1; biny<=nybins; ++biny){
      float x = hist->GetXaxis()->GetBinCenter(binx);
      float y = hist->GetYaxis()->GetBinCenter(biny);
      cout<<"\tx: "<<x<<" y: "<<y<<" bc: "<<hist->GetBinContent(binx,biny)<<endl;
    }
  }

}
