#pragma once
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <sstream>
#include <vector>

//////////////////////////////////////////////////////////////////////////////
// USAGE: vector<int> list_of_processes = get_pid_list("ZV");
// OR vector<int> list_of_processes = get_pid_list(0);
//
// Make sure DIR_0 can reach the data file from your current directory
//////////////////////////////////////////////////////////////////////////////

std::string DIR_0 = "inputHistoList/";

int n_vf_process_file = 5;
std::string vf_process_file[] = { "WZ_ZZ_Sherpa", "WW_Sherpa", "top_MCNLO", "Zjets_AlpgenPythia", "Higgs" };
std::string vf_process_name[] = { "ZV", "WW", "Top", "Z+jets", "Higgs" };
//// index                   0        1      2     3     4

std::vector<int> get_pid_list(int process_);

std::vector<std::string> split(const std::string &s, char delim);
std::string trim(std::string s);
template <class T> T string2(std::string s);

std::vector<int> get_pid_list(std::string process_) {
	std::vector<int> empty;

	for (int i = 0; i < n_vf_process_file; i++) {
		if (process_.compare(vf_process_name[i]) == 0) return get_pid_list(i);
	}
	return empty;
}

std::vector<int> get_pid_list(int process_) {
	std::vector<int> process_list;

	std::stringstream in_filename;
	in_filename << DIR_0 << vf_process_file[process_] << ".txt";

	// open the process list file
	std::ifstream input_file_(in_filename.str().data());

	bool new_category = false;
	if (!input_file_.is_open()) {
		std::cout << in_filename.str().data() << ": " << "file not open. Returing NULL." << std::endl;
		return process_list;
	}
	else {
		std::cout << "Opened " << in_filename.str().data() << std::endl;
		new_category = true;
	}

	// read in the process list
	while (!input_file_.eof()) {
		std::string line_ = "";
		getline(input_file_, line_);

		if (line_ == "") continue;

		char firstchar = line_.at(0);
		if (firstchar < 0x30 || firstchar > 0x7A) continue;

		// split line_ on '.'
		std::vector<std::string> splits_ = split(trim(line_), '.');
		//split on '_'
		if (splits_.size() > 0) {
			splits_ = split(splits_[1], '_');
		}
		process_list.push_back(string2<int>(trim(splits_[0])));
	}
	input_file_.close();

	return process_list;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

std::string trim(std::string s) {
	std::string val = s.erase(s.find_last_not_of(" \n\r\t") + 1);
	val.erase(0, s.find_first_not_of(" \n\r\t"));
	return val;
}

template <class T> T string2(std::string s) {
	T x;
	std::istringstream stream1(s);
	stream1 >> x;
	return x;
}