#include "kmer_DB.h"
#include <fstream>

using google::dense_hash_set;
using namespace std;


Kmer_DB::Kmer_DB(std::string dir_path, std::string db_name):
	m_db_name(db_name),
	m_dir_path(dir_path) {}


bool lookup_x(const kmer_set& Set, const uint64& kmer)
{
	kmer_set::const_iterator it  = Set.find(kmer);
	return (it != Set.end()); 
}


// go over the KMC DB and output to file only the kmers in the kmers set inputed
void Kmer_DB::intersect_kmers(const kmer_set& kmers_to_use, std::string file_name) {
	string output_file = m_dir_path + "/" +  file_name;
	string output_log_file = output_file + ".log";

	ofstream of(output_file, ios::binary);
	ofstream of_log(output_log_file);

	cerr << output_file << endl;
	CKMCFile kmer_database = get_KMC_handle(); //open DB
	CKmerAPI_YV kmer_obj(KMER_LEN);

	double t0,t1;
	t0 = get_time();
	unsigned int counter;
	uint64 cnt_f = 0, cnt_nf = 0, kmer;

	vector<uint64> kmers;
	while(kmer_database.ReadNextKmer(kmer_obj, counter)) {
		kmer = kmer_obj.to_uint();
		if(lookup_x(kmers_to_use, kmer)) {
			cnt_f++;
			kmers.push_back(kmer);
		} else { cnt_nf++;}
	}
	// Sort kmers 
	sort(kmers.begin(), kmers.end());
	for(vector<uint64>::const_iterator it = kmers.begin(); it != kmers.end(); ++it) {
		of.write(reinterpret_cast<const char *>(&(*it)), sizeof(*it));
	}
	t1 = get_time();
	of_log << m_db_name << "\ttime\t" << t1-t0 << "\tfound\t" << cnt_f << "\tnot_found\t" << cnt_nf << endl;
	of.close();
	of_log.close();
}


// Counts how many times each k-mer appeared and plot to std::cout
vector<std::size_t> Kmer_DB::calculate_kmers_counts_histogram() {
	CKMCFile kmer_database = get_KMC_handle();
	CKmerAPI kmer_obj(KMER_LEN);

	vector<std::size_t> counters(0, 0);
	unsigned int counter;
	while(kmer_database.ReadNextKmer(kmer_obj, counter)) {
		if(counter >= counters.size()) { counters.resize(counter+1,0);}
		counters[counter] += 1;		
	}
	return counters;
}

// get a hanle to the KMC DB to run over its k-mers 
CKMCFile Kmer_DB::get_KMC_handle() {
	CKMCFile kmer_database;
	kmer_database.OpenForListing(m_dir_path + "/" +  m_db_name);

	return kmer_database;
}


