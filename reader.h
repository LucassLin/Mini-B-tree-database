#ifndef reader_h
#define reader_h 

#include<iostream>
#include<string>
#include<fstream>
#include <stdio.h>
#include<algorithm>
#include <map>
#include<vector>
using namespace std;
typedef std::pair <string, string> stringPair;


class reader{
	private:
		string file;
		ifstream infile;
		map<string,long> hashmap;
	
	public:
		reader(string filename);
		void read();
		void mergeIndex(string file1, string file2);
		map<string, long> getMap() { return hashmap; };

};

#endif