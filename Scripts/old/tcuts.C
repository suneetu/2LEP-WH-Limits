void tcuts(){

	int WW[] = { 177997, 183734, 183736, 183738, 169471, 169472, 169473, 169474, 
			   	 169475, 169476, 169477, 169478, 169479, 126988, 126989, 167006 };
	int nWW = sizeof(WW) / sizeof(int);

	string file_prefix = "/scratch/suneetu/WHLimits2014/Inputs/20140224/Raw/NOM_";
	
	string LeptonPtAsymm = "lept1Pt>30000. && lept2Pt>20000.";
	string LeptonPtSymm = "lept1Pt>30000. && lept2Pt>30000.";
	string OneJet = "L2nCentralLightJets==1";
	string TwoORThreeJets = "(L2nCentralLightJets==2 || L2nCentralLightJets==3)";
	string Zveto = "!(L2Mll>(91187.6-10000.) && L2Mll<(91187.6+10000.))";
	string deltaEta = "(deltaEtaLl<1.5 && deltaEtaLl>-1.5)";

	vector<double> yields;

	cout << "SS-1j-EE" << endl;

	TH1F** new_hist;

	for (int i = 0; i < nWW; i++) {
		stringstream hist_;
		hist_ << "h" << i;

		new_hist[i] = new TH1F(hist_.str().data(), hist_.str().data(), 100, 0.0, 400.0);
	}

	for (int i = 0; i < nWW; i++) {

		TCut sel("");
		TCut weight("eventweight");

		stringstream cuts;
		cuts << "!isOS && isEE   && " << OneJet << " && " << LeptonPtAsymm << " && " << Zveto << " && L2METrel>55000. && mlj<90000. && Ht>200000. ";

		sel += TCut(cuts.str().data());
		sel.Print();

		stringstream chain_name;
		chain_name << "id_" << WW[i];

		TChain* nt;
		nt = new TChain(chain_name.str().data());
		
		stringstream filename;
		filename << file_prefix << WW[i] << ".root";

		nt->Add(filename.str().data());

		if (!nt->IsZombie()) {
			stringstream hist_;

			draw_ << "Ht / 1000. >> h" << i;

			nt->Draw(draw_.str().data(), sel * weight);

			cout << WW[i] << ": " << (new_hist[i]->Integral(0, -1)) << endl;
		}
	}

	cout << "Done." << endl;
}
