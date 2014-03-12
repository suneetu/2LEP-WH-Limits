#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <vector>
#include <map>

#include <TROOT.h>
#include <TDirectory.h>

#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TCut.h>

using namespace std;

#define DIR_0 "inputHistoList/"
#define FILE_PREFIX "/gdata/atlas/suneetu/Documents/WHLimits2014/Inputs/20140304/Raw/NOM_";

const int n_process_file = 5;
string process_file[] = { "Higgs", "top_MCNLO", "WW_Sherpa", "WZ_ZZ_Sherpa", "Zjets_AlpgenPythia" };
string process_name[] = { "Higgs", "Top", "WW", "ZV", "Z + jets" };
// index                   0        1      2     3     4
#define PROCESS 0 // (integer) Be sure to choose a PROCESS

string signal_region[] = { "eeSR1jet", "eeSR23jets", "emSR1jet", "emSR23jets", "mmSR1jet", "mmSR23jets", };
// index                    0           1             2           3             4           5
#define S_REGION 0 // (integer) Don't forget to define the SIGNAL_REGION


// Function Prototypes
string trim(string s);
template <class T> T string2(string s);
vector<string> split(const string &s, char delim);

vector<int> open_pid_list(int process_); 

const string LeptonPtAsymm = "lept1Pt>30000. && lept2Pt>20000.";
const string LeptonPtSymm = "lept1Pt>30000. && lept2Pt>30000.";
const string OneJet = "L2nCentralLightJets==1";
const string TwoORThreeJets = "(L2nCentralLightJets==2 || L2nCentralLightJets==3)";
const string Zveto = "!(L2Mll>(92000.-10000.) && L2Mll<(92000.+10000.))";
const string deltaEta = "(deltaEtaLl<1.5 && deltaEtaLl>-1.5)";

int main(int argc, char* argv[]) {

	gROOT->ProcessLine("#include <vector>");

	int p_ = PROCESS;// set process
	vector<int> process_list = open_pid_list(p_);
	int n_Samples = process_list.size();

	// define the all-important TCut strings
	map <string, stringstream> tcuts;
	tcuts["eeSR1jet"] << "((!isOS || L2qFlipWeight!=1.0) && isEE   && " << OneJet << " && " << LeptonPtAsymm << " && " << Zveto << " && L2METrel>55000. && mlj<90000. && Ht>200000.)";

	cout << process_name[PROCESS] << " - " << signal_region[S_REGION] << endl;

	// We'll use these histograms just for storing 
	// we have to book histograms with unique names because ROOT sucks.
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

		/*
		mgr.cutsDict["eeSR1jet"] = "((!isOS || L2qFlipWeight!=1.0) && isEE   && " << OneJet << " && " << LeptonPtAsymm << " && " << Zveto << " && L2METrel>55000. && mlj<90000. && Ht>200000.)";
		mgr.cutsDict["eeSR23jets"] = "((!isOS || L2qFlipWeight!=1.0) && isEE   && " << TwoORThreeJets << " && " << LeptonPtAsymm << " && " << Zveto << " && L2METrel>30000. && mljj<120000. && mtmax>100000.)";


		mgr.cutsDict["mmSR1jet"] = "((!isOS || L2qFlipWeight!=1.0) && isMUMU && " << OneJet << " && " << LeptonPtAsymm << " && " << deltaEta << " && mlj<90000. && Ht>200000. && mtmax>100000.)";
		mgr.cutsDict["mmSR23jets"] = "((!isOS || L2qFlipWeight!=1.0) && isMUMU && " << TwoORThreeJets << " && " << LeptonPtSymm << " && " << deltaEta << " && mljj<120000. && Ht>220000.)";
				;
				mgr.cutsDict["emSR1jet"] = "((!isOS || L2qFlipWeight!=1.0) && isEMU  && " << OneJet << " && " << LeptonPtSymm << " && " << deltaEta << " && mlj<90000. && mtllmet>120000. && mtmax>110000.)";
				mgr.cutsDict["emSR23jets"] = "((!isOS || L2qFlipWeight!=1.0) && isEMU  && " << TwoORThreeJets << " && " << LeptonPtSymm << " && " << deltaEta << " && mljj<120000. && mtllmet>110000.)";
*/

		sel += TCut(cuts.str().data());

		TCut selRaw("");

		stringstream chain_name;
		chain_name << "id_" << process_list[i];

		TChain* nt;
		nt = new TChain(chain_name.str().data());

		stringstream filename;
		filename << FILE_PREFIX << process_list[i] << ".root";

		nt->Add(filename.str().data());

		stringstream draw_;
		draw_ << "Ht / 1000. >> h" << i;

		stringstream drawraw_;
		drawraw_ << "Ht / 1000. >> hraw" << i;


		nt->Draw(draw_.str().data(), sel * weight);
		double integral_ = new_hist[i]->Integral(0, -1);

		nt->Draw(drawraw_.str().data(), sel);
		double integral_raw = new_hist_raw[i]->Integral(0, -1);

		cout << process_list[i] << ": raw:" << integral_raw << " weighted:" << integral_ << endl;

		total += integral_;
	}

	cout << "Total: " << total << endl;

	cout << "Done." << endl;
	return 0;
}

vector<int> open_pid_list(int process_) {
	stringstream in_filename;
	in_filename << DIR_0 << process_file[process_] << ".txt";

	// open the process list file
	ifstream input_file_(in_filename.str().data());

	bool new_category = false;
	if (!input_file_.is_open()) {
		cout << in_filename.str().data() << ": " << "file not open. Exiting." << endl;
		return 0;
	}
	else {
		cout << "Opened " << in_filename.str().data() << endl;
		new_category = true;
	}

	// read in the process list
	vector<int> process_list;
	while (!input_file_.eof()) {
		string line_ = "";
		getline(input_file_, line_);

		if (line_ == "") continue;

		char firstchar = line_.at(0);
		if (firstchar < 0x30 || firstchar > 0x7A) continue;

		// split line_ on '.'
		vector<string> splits_ = split(trim(line_), '.');
		//split on '_'
		if (splits_.size() > 0) {
			splits_ = split(splits_[1], '_');
		}
		process_list.push_back(string2<int>(trim(splits_[0])));
	}
	input_file_.close();

	return process_list;
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

string trim(string s) {
	string val = s.erase(s.find_last_not_of(" \n\r\t") + 1);
	val.erase(0, s.find_first_not_of(" \n\r\t"));
	return val;
}

template <class T> T string2(string s) {
	T x;
	istringstream stream1(s);
	if (!(stream1 >> x)) {
		cout << "string2 call error";
		exit(1);
	}
	return x;
}