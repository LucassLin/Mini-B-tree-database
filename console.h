#include <iostream>
#include <string>
#include "reader.h"
#include "externalSort.h"
#include "bpt.h"

using namespace std;

class console{
	private:
		int num_of_pages;
		reader* rd;
		externalSort* es;
		
	public:
		void initialize(string indexfile);
		console();
		console(int size);
		void load(string indexfile);
		void merge(string indexfile);
		void quit();
		void insert_doc(string word, string doc_name);
		void delete_doc(string word, string doc_name);
		int count(string key);
		string search(string s);
		string printpath(string key);
		string page(int num);
		string range_search(string k1, string k2);
};
