#ifndef externalSort_h
#define externalSort_h 

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <cstring>
#include <queue>
using namespace std;
typedef std::pair <string, string> stringPair;

struct Seq {
    stringPair value;
    std::ifstream source;
    Seq(string filename) : source(filename) {}
};

class externalSort{
	private:
		string filename;
		int pages;
		vector<string>inputfile;
		
	public:
		externalSort(int p, string indexfile) : pages(p), filename(indexfile) {}
		
		void sort1();
		
		void merge(std::vector<Seq*>& sequences, std::ofstream& output);
		
		void externalMerge();
};

#endif

