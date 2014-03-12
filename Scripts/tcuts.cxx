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

#include <TCanvas.h>
#include <TH1.h>
#include <TH1F.h>
#include <TCut.h>

using namespace std;

#define DIR_0 "inputHistoList/"
#define FILE_PREFIX "/gdata/atlas/suneetu/Documents/WHLimits2014/Inputs/20140304/Raw/NOM_"

const int n_process_file = 5;
string process_file[] = { "WZ_ZZ_Sherpa", "WW_Sherpa", "top_MCNLO", "Zjets_AlpgenPythia", "Higgs" };
string process_name[] = { "ZV", "WW", "Top", "Z+jets", "Higgs" };
//// index                   0   1      2     3     4
////const int n_process_file = 3;
////string process_file[] = { "GenuineSS_diBSherpaM", "ChargeFlip_diBSherpaM", "Higgs" };
////string process_name[] = { "S-S", "C-Flip", "Higgs" };

#define PROCESS 2 // (integer) Be sure to choose a PROCESS %%%%%%%%

string signal_region[] = { "eeSR1jet", "mmSR1jet", "emSR1jet", "eeSR23jets", "mmSR23jets", "emSR23jets" };
// index                    0           1             2           3             4           5
#define S_REGION 2 // (integer) Don't forget to define the SIGNAL_REGION %%%%%%%%

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

	// define the all-important TCut strings
	map <string, string> tcuts;
	stringstream tcuts_join;

	//eeSR1jet
	tcuts_join << "((!isOS || L2qFlipWeight!=1.0) && isEE   && " << OneJet << " && " << LeptonPtAsymm << " && " << Zveto << " && L2METrel>55000. && mlj<90000. && Ht>200000.)";
	tcuts["eeSR1jet"] = tcuts_join.str(); tcuts_join.str("");// clear stringstream

	//mmSR1jet
	tcuts_join << "((!isOS || L2qFlipWeight!=1.0) && isMUMU && " << OneJet << " && " << LeptonPtAsymm << " && " << deltaEta << " && mlj<90000. && Ht>200000. && mtmax>100000.)";
	tcuts["mmSR1jet"] = tcuts_join.str(); tcuts_join.str("");

	//emSR1jet
	tcuts_join << "((!isOS || L2qFlipWeight!=1.0) && isEMU  && " << OneJet << " && " << LeptonPtSymm << " && " << deltaEta << " && mlj<90000. && mtmax>110000. && mtllmet>120000. )";
	tcuts["emSR1jet"] = tcuts_join.str(); tcuts_join.str("");

	//eeSR23jets
	tcuts_join << "((!isOS || L2qFlipWeight!=1.0) && isEE   && " << TwoORThreeJets << " && " << LeptonPtAsymm << " && " << Zveto << " && L2METrel>30000. && mljj<120000. && mtmax>100000.)";
	tcuts["eeSR23jets"] = tcuts_join.str(); tcuts_join.str("");

	//mmSR23jets
	tcuts_join << "((!isOS || L2qFlipWeight!=1.0) && isMUMU && " << TwoORThreeJets << " && " << LeptonPtSymm << " && " << deltaEta << " && mljj<120000. && Ht>220000.)";
	tcuts["mmSR23jets"] = tcuts_join.str(); tcuts_join.str("");

	//emSR23jets
	tcuts_join << "((!isOS || L2qFlipWeight!=1.0) && isEMU  && " << TwoORThreeJets << " && " << LeptonPtSymm << " && " << deltaEta << " && mljj<120000.)";
	tcuts["emSR23jets"] = tcuts_join.str(); tcuts_join.str("");

	cout << process_name[PROCESS] << " - " << signal_region[S_REGION] << endl;

	// We'll use these histograms just for storing 
	// we have to book histograms with unique names because ROOT sucks.
	TH1F** new_hist = new TH1F*[process_list.size()];
	TH1F** new_hist_raw = new TH1F*[process_list.size()];
	for (int i = 0; i < process_list.size(); i++) {
		stringstream hist_;
		hist_ << "h" << i;
		stringstream histraw_;
		histraw_ << "hraw" << i;
		new_hist[i] = new TH1F(hist_.str().data(), hist_.str().data(), 40, 0.0, 800.0);
		new_hist_raw[i] = new TH1F(histraw_.str().data(), hist_.str().data(), 40, 0.0, 800.0);
	}

	double total = 0;

	for (int i = 0; i < process_list.size(); i++) {
		TCanvas* tc = new TCanvas();// disposable

		TCut sel("");// key TCut
		TCut weight("eventweight");// selects eventweight leaf
		sel += TCut(tcuts[signal_region[S_REGION]].data());// string.data()

		// name of TTree in .root file
		stringstream chain_name;
		chain_name << "id_" << process_list[i];

		TChain* nt;
		nt = new TChain(chain_name.str().data());

		stringstream filename_;
		filename_ << FILE_PREFIX << process_list[i] << ".root";
		nt->Add(filename_.str().data());

		// unique histogram names for ROOT to prevent crashes
		stringstream draw_;
		draw_ << "Ht / 1000. >> h" << i;
		stringstream drawraw_;
		drawraw_ << "Ht / 1000. >> hraw" << i;

		// weighted count
		nt->Draw(draw_.str().data(), sel * weight);
		double integral_ = new_hist[i]->Integral(0, -1);

		// raw (cut) count
		nt->Draw(drawraw_.str().data(), sel);
		double integral_raw = new_hist_raw[i]->Integral(0, -1);

		if (integral_raw > 0.0) cout << process_list[i] << ":\traw:" << integral_raw << "\tweighted:" << integral_ << endl;

		total += integral_;
	}

	cout << "Total: " << total << endl;
	cout << process_name[PROCESS] << " - " << signal_region[S_REGION] << endl;
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
		exit(1);
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

vector<string> &split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
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