#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <vector>

#include <TROOT.h>
#include <TDirectory.h>

#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TCut.h>
#include <TClonesArray.h>
#include <TNtuple.h>
#include <TLorentzVector.h>

#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TH1.h>
#include <TH1F.h>
#include <TLatex.h>
#include <TLine.h>

using namespace std; //////////// Start

int main(int argc, char* argv[]) {

	gROOT->ProcessLine("#include <vector>");

	// WW
	//int Samples[] = { 177997, 183734, 183736, 183738, 169471, 169472, 169473, 169474, 169475, 169476, 169477, 169478, 169479, 126988, 126989, 167006 };

	// TOP
	//int Samples[] = { 110001, 108346, 119353, 174830, 174831, 119355, 174832, 174833, 119583, 164439, 158344 };

	//ZV 
	int Samples[] = { 179974, 179975, 183585, 183587, 183589, 183591, 183735, 183737, 183739, 167007, 177999, 183586, 183588, 183590, 126894, 179396, 167008 };


int n_Samples = sizeof(Samples) / sizeof(int);

string file_prefix = "/gdata/atlas/suneetu/Documents/WHLimits2014/Inputs/20140304/Raw/NOM_";

string LeptonPtAsymm = "lept1Pt>30000. && lept2Pt>20000.";
string LeptonPtSymm = "lept1Pt>30000. && lept2Pt>30000.";
string OneJet = "L2nCentralLightJets==1";
string TwoORThreeJets = "(L2nCentralLightJets==2 || L2nCentralLightJets==3)";
string Zveto = "!(L2Mll>(92000.-10000.) && L2Mll<(92000.+10000.))";
string deltaEta = "(deltaEtaLl<1.5 && deltaEtaLl>-1.5)";

vector<double> yields;

cout << "SS-1j-EE" << endl;

TH1F** new_hist = new TH1F*[n_Samples];
TH1F** new_hist_raw = new TH1F*[n_Samples];

for (int i = 0; i < n_Samples; i++) {
	stringstream hist_;
	hist_ << "h" << i;

	stringstream histraw_;
	histraw_ << "hraw" << i;

	new_hist[i] = new TH1F(hist_.str().data(), hist_.str().data(), 40, 0.0, 800.0);
	new_hist_raw[i] = new TH1F(histraw_.str().data(), hist_.str().data(), 40, 0.0, 800.0);
}

double total = 0;

for (int i = 0; i < n_Samples; i++) {

	TCanvas* tc = new TCanvas();
	TCut sel("");
	TCut weight("eventweight");
	stringstream cuts;
	//cuts << "!isOS && isEE   && " << OneJet << " && " << LeptonPtAsymm << " && " << Zveto << " && L2METrel>55000. && mlj<90000. && Ht>200000. ";
	cuts << "(!isOS || L2qFlipWeight!=1.0) && isEE   && " << OneJet << " && " << LeptonPtAsymm << " && " << Zveto << " && L2METrel>55000. && mlj<90000. && Ht>200000. ";

	sel += TCut(cuts.str().data());

	TCut selRaw("");

	stringstream chain_name;
	chain_name << "id_" << Samples[i];

	TChain* nt;
	nt = new TChain(chain_name.str().data());

	stringstream filename;
	filename << file_prefix << Samples[i] << ".root";

	nt->Add(filename.str().data());

	stringstream draw_;
	draw_ << "Ht / 1000. >> h" << i;

	stringstream drawraw_;
	drawraw_ << "Ht / 1000. >> hraw" << i;


	nt->Draw(draw_.str().data(), sel * weight);
	double integ_ = new_hist[i]->Integral(0, -1);

	nt->Draw(drawraw_.str().data(), sel);
	double integ_raw = new_hist_raw[i]->Integral(0, -1);

	cout << Samples[i] << ": raw:" << integ_raw << " weighted:" << integ_ << endl;

	total += integ_;
}

cout << "Total: " << total << endl;

cout << "Done." << endl;
return 0;
}
