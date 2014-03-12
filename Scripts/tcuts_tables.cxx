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
#define FILE_PREFIX "/gdata/atlas/suneetu/Documents/WHLimits2014/Inputs/20140308/Raw/NOM_"

#define SHOW_EACH_MCSAMPLE false 

const int n_process_file = 6;
string process_file[] = { "WZ_ZZ_Sherpa", "WW_Sherpa", "top_MCNLO", "Zjets_AlpgenPythia", "Higgs", "Fakes" };
string process_name[] = { "ZV", "WW", "Top", "Z+jets", "Higgs", "Fakes" };

////const int n_process_file = 3;
////string process_file[] = { "GenuineSS_diBSherpaM", "ChargeFlip_diBSherpaM", "Higgs" };
////string process_name[] = { "S-S", "C-Flip", "Higgs" };

int n_signals = 6;
string signal_region[] = { "eeSR1jet", "mmSR1jet", "emSR1jet", "eeSR23jets", "mmSR23jets", "emSR23jets" };
// index                    0           1             2           3             4           5
//int n_fakes = 6;
//double fakes[] = { 2.64, 0.00, 1.33, 2.04, 0.00, 0.25};

// Function Prototypes
string trim(string s);
template <class T> T string2(string s);
vector<string> split(const string &s, char delim);

vector<int> open_pid_list(int process_);

const string LeptonPtAsymm = "lept1Pt>30000. && lept2Pt>20000.";
const string LeptonPtSymm = "lept1Pt>30000. && lept2Pt>30000.";
const string OneJet = "L2nCentralLightJets==1";
const string TwoORThreeJets = "(L2nCentralLightJets==2 || L2nCentralLightJets==3)";
const string Zveto = "!(L2Mll>(91200.-10000.) && L2Mll<(91200.+10000.))";
const string deltaEta = "(deltaEtaLl<1.5 && deltaEtaLl>-1.5)";

int main(int argc, char* argv[]) {

	gROOT->ProcessLine("#include <vector>");

	// define the all-important TCut strings
	map <string, string> tcuts;
	stringstream tcuts_join;

	//eeSR1jet
	tcuts_join << "((!isOS || L2qFlipWeight!=1.0) && isEE   && " << OneJet << " && " << LeptonPtAsymm << " && " << Zveto << " && mlj<90000.  && L2METrel>55000. && Ht>200000.)";
	tcuts["eeSR1jet"] = tcuts_join.str(); tcuts_join.str("");// clear stringstream

	//mmSR1jet
	tcuts_join << "((!isOS || L2qFlipWeight!=1.0) && isMUMU && " << OneJet << " && " << LeptonPtAsymm << " && " << deltaEta << " && mlj<90000.  && Ht>200000. && mtmax>100000.)";
	tcuts["mmSR1jet"] = tcuts_join.str(); tcuts_join.str("");//  && mlj<90000. 

	//emSR1jet
	tcuts_join << "((!isOS || L2qFlipWeight!=1.0) && isEMU  && " << OneJet << " && " << LeptonPtSymm << " && " << deltaEta << " && mlj<90000.  && mtmax>110000. && mtllmet>120000. )";
	tcuts["emSR1jet"] = tcuts_join.str(); tcuts_join.str("");

	//eeSR23jets
	tcuts_join << "((!isOS || L2qFlipWeight!=1.0) && isEE   && " << TwoORThreeJets << " && " << LeptonPtAsymm << " && " << Zveto << " && mljj<120000.  && L2METrel>30000. && mtmax>100000.)";
	tcuts["eeSR23jets"] = tcuts_join.str(); tcuts_join.str("");

	//mmSR23jets
	tcuts_join << "((!isOS || L2qFlipWeight!=1.0) && isMUMU && " << TwoORThreeJets << " && " << LeptonPtSymm << " && " << deltaEta << " && mljj<120000.  && Ht>220000.)";
	tcuts["mmSR23jets"] = tcuts_join.str(); tcuts_join.str("");// && mljj<120000. 

	//emSR23jets
	tcuts_join << "((!isOS || L2qFlipWeight!=1.0) && isEMU  && " << TwoORThreeJets << " && " << LeptonPtSymm << " && " << deltaEta << " && mljj<120000.  )";
	tcuts["emSR23jets"] = tcuts_join.str(); tcuts_join.str("");

	vector< vector<double> > table_totals;

	for (int sr_ = 0; sr_ < n_signals; sr_++) {

		string SR_ = tcuts[signal_region[sr_]];

		vector<double> process_totals;

		for (int p_ = 0; p_ < n_process_file; p_++) {
			vector<int> process_list = open_pid_list(p_);

			cout << process_name[p_] << " - " << signal_region[sr_] << endl;

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
			TCanvas* tc = new TCanvas();// disposable

			for (int i = 0; i < process_list.size(); i++) {
				TCut sel("");// key TCut
				TCut weight("eventweight");// selects eventweight leaf
				sel += TCut((SR_).data());// string.data()

				// name of TTree in .root file
				stringstream chain_name;
				chain_name << "id_" << process_list[i];

				TChain* nt;
				nt = new TChain(chain_name.str().data());

				stringstream filename_;
				filename_ << FILE_PREFIX << process_list[i] << ".root";
				nt->Add(filename_.str().data());

				if (!nt->IsZombie()) {
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

					if (SHOW_EACH_MCSAMPLE) cout << process_list[i] << ":\traw:" << integral_raw << "\tweighted:" << integral_ << endl;

					total += integral_;
				}

				delete nt;
			}
			process_totals.push_back(total);

			cout << "Total: " << total << endl;
			cout << process_name[p_] << " - " << signal_region[sr_] << endl << endl;

			for (int i = 0; i < process_list.size(); i++) {
				delete new_hist[i];
				delete new_hist_raw[i];
			}
			delete[] new_hist;
			delete[] new_hist_raw;

			delete tc;
		}
		double col_total = 0;
		for (int col_ = 0; col_ < process_totals.size(); col_++) col_total += process_totals[col_];

		//col_total += fakes[sr_];
		process_totals.push_back(col_total);
		table_totals.push_back(process_totals);
	}

	cout.precision(2);
	cout.setf(ios::fixed, ios::floatfield);
	cout << endl << "TABLE:" << endl;


	cout << "process\t|";
	for (int sr_ = 0; sr_ < n_signals; sr_++) {
		cout << signal_region[sr_] << "\t";
	}
	cout << endl;
	//cout << "Fakes" << "\t|";
	//for (int sr_ = 0; sr_ < n_signals; sr_++) {
	//	cout << fakes[sr_] << "\t\t";
	//}
	//cout << endl;
	for (int p_ = 0; p_ < n_process_file; p_++) {
		cout << process_name[p_] << "\t|";
		for (int sr_ = 0; sr_ < n_signals; sr_++) {
			cout << table_totals[sr_][p_] << "\t\t";
		}
		cout << endl;
	}
	cout << "Total:" << "\t|";
	for (int sr_ = 0; sr_ < n_signals; sr_++) {
		cout << table_totals[sr_][n_process_file] << "\t\t";
	}

	cout << endl;

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