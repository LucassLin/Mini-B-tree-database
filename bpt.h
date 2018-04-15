#ifndef BPT_H					// avoid repeated expansion
#define BPT_H

#include <string>
#include <vector>
using namespace std;

class Node {
public:
	static long fileCount;              
	static long lowerBound;
	static long upperBound;
	static long pageSize;
	long index;                     
	bool is_leaf;           
public:
	long parent;
	long next;
	long prev;    
	vector<long> keys;
	vector<long> children;          
	vector<long> words;
	Node();
	Node(long _fileIndex);
	bool isLeaf() { return is_leaf; }
	string getFileName() { return "release/node_" + to_string(index); }
	long getFileIndex() { return (long)index; }
	long size() { return keys.size(); }
	static void initialize(long pagesize);
	long getKeyPosition(long key);
	void commit();
	void read();
	void printNode();
	void insert(long key, long left, long right);
	void insert(long key, long right);
	void split();
	void splitLeaf();
	void splitInternal();
};

#endif 
