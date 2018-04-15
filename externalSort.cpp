#include "externalSort.h"
#include <algorithm>
#include <sstream>
#include <map>
using namespace std;
typedef std::pair <string, string> stringPair;

bool next(Seq* seq) {
    if (seq->source.good()) {
        seq->source >> seq->value.first >> seq->value.second;
        return true;
    }
    
    return false;
}

		static int counter = 0;

void externalSort::sort1(){
			int number = 0;
			std::ifstream in(this->filename);
			while(!in.eof()){
				std::vector<stringPair> values = std::vector<stringPair>();
				string s1, s2;
				while(number<pages*8096 && !in.eof()){
					std::map<string,string> mymap;
					string line;
					getline(in, line);
					istringstream ss(line);
					ss >> s1;
					while(ss>>s2){
						if(mymap.find(s2)==mymap.end()){
							mymap[s2] = s1;					
							std::pair <string,string> foo;
		        			foo = make_pair(s2, s1);
		            		values.push_back(foo);
		            		number+=16;
		            	}
					}
				}
				//cout << "vector size is " << values.size() <<endl;
				number = 0;
				std::sort(values.begin(),values.end());
				string outputfile = "input" + to_string(counter) + ".txt";
				counter++;
				//cout << "counter is " << counter <<endl;
				std::ofstream out;
				out.open(outputfile, std::ofstream::out | std::ofstream::app);
				if(!out) {cout << "Cannot open output file\n" ;}
				for(int i=0; i<values.size(); i++) {
					if(i==values.size()-1){
	        			out << values[i].first << " " << values[i].second;
	        			}
	        			else{
	        			out << values[i].first << " " << values[i].second <<endl;
					}
	    			}
	    			out.close();
	    			inputfile.push_back(outputfile);
    			}
			in.close();
			cout << "seprating file done!\n" ;
		}
		
		void externalSort::merge(std::vector<Seq*>& sequences, std::ofstream& output) {
    		auto comp = []( const Seq* lhs, const Seq* rhs ) { return lhs->value.first > rhs->value.first; };
    		std::priority_queue<Seq*, std::vector<Seq*>, decltype(comp)> pq(sequences.begin(), sequences.end(), comp);
		string s1 = " ";
    		while(!pq.empty()) {
        		Seq* top = pq.top();
        		pq.pop();
			if(top->value.first == s1)
        			output << " " <<top->value.second;
			else
				output<<endl << top->value.first <<" " << top->value.second;

			s1 = top->value.first;
        		if(next(top)) {
            		pq.push(top);
        		}
    		}
		}
		
		void externalSort::externalMerge(){
			sort1();
			string outputFilename = "sortedIndex.txt";
			std::ofstream output(outputFilename);
			std::vector<Seq*> sequences = std::vector<Seq*>();
			for(int i=0; i<inputfile.size(); i++){
				//cout << inputfile[i] <<endl;
				Seq* seq = new Seq(inputfile[i]);
        		if(next(seq)) {
            		sequences.push_back(seq);
        		}
			}
			merge(sequences, output);
    		output.close();
		}