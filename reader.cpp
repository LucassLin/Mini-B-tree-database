#include "reader.h"
#include <sstream>
#include <string>
#include <map>
#include <set>
typedef std::pair <string, string> stringPair;

reader::reader(string filename){
	file = filename;
}
            
void reader::read(){
	infile.open(file);
	ofstream out;
	out.open("hashIndex.txt", ofstream::out);
	int count = 0;
	string k;
	getline(infile,k);
	while(!infile.eof()){
		string line, key, token;
		getline(infile,line);
		istringstream ss(line);
		ss >> key;
		while(key[0]=='\n') {getline(infile,line);}
		this->hashmap[key] = count;
		out << count <<" ";
		while(ss>>token){
			out << token << " ";
		}
		for(int i=0; i<100; i++) {out <<' ';}
		out << endl;
		count++;
	}
	out.close();
}

void reader::mergeIndex(string infile1, string infile2){
	ifstream in1;
	ifstream in2;
	ofstream out("MergedIndex.txt", ofstream::out);
	in1.open(infile1, ifstream::in);
	in2.open(infile2, ifstream::in);
	long counter = 0;
	string final;
	while(!in1.eof() || !in2.eof()){
		if(in1.eof()){
			in1.close();
			while(getline(in2,final)) out << final <<endl;
			in2.close();
			out.close();
			break;
		}
		if(in2.eof()){
			in2.close();
			while(getline(in1,final)) out << final <<endl;
			in1.close();
			out.close();
		}
		string key1 = "";
		string key2 = "";
		int count1 = 0;
		int count2 = 0;
		int counter = 0;
		string line1, line2;
		char c, d;
		c = in1.get();
		while(c != ' '){
			key1+=c;
			count1++;
			c = in1.get();
		}
		d = in2.get();
		while(d != ' '){
			key2+=d;
			count2++;
			d = in2.get();
		}
		for(count1; count1>=0; count1--) in1.unget();
		for(count2; count2>=0; count2--) in2.unget();
		cout << "equal" <<endl;
		if(key1==key2){
			getline(in1,line1);
			getline(in2,line2);
			istringstream ss1(line1);
			istringstream ss2(line2);
			std::map<string,string> mymap;
			string value;
			ss1 >> key1;
			ss2 >> key2;
			out << key1 <<" ";
			while(ss1>>value){
					mymap[value] = counter;
					counter++;
					out << value <<" ";
			}
			while(ss2>>value){
				if(mymap.find(value)==mymap.end()){
					out << value <<" ";
				}
			}
			out << endl;
		}
		else if(key1 < key2){
			getline(in1,line1);
			out <<line1;
			out << endl;		
		}
		else{
			getline(in2,line2);
			out << line2;
			out << endl;
		}
	}
	out.close();
}

