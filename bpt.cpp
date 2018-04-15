#include "bpt.h"
#include "console.h"
#include <iostream>
#include <math.h>
#include <fstream>
#include <locale>
#include <codecvt>
#include <ctime>
#include <map>
using namespace std;
#define INITIALIZE -1
#define TEST_FILE "hashIndex.txt"
long Node::lowerBound = 0;
long Node::upperBound = 0;
long Node::pageSize = 0;
long Node::fileCount = 0;
Node* root = NULL;
Node* nearestParent = NULL;
Node* newParent = NULL;
ofstream out;

	Node::Node() {

		parent = INITIALIZE;
		next = INITIALIZE;
		prev = INITIALIZE;
		is_leaf = true;
		index = ++fileCount;

	}

	Node::Node(long _index) {
		index = _index;
		read();
	}

	void Node::initialize(long pagesize) {

		pageSize = pagesize;
		long headerSize = sizeof(index)
			+ sizeof(is_leaf)
			+ sizeof(parent)
			+ sizeof(next)
			+ sizeof(prev);
		long nodeSize = sizeof(index);
		long keySize = sizeof(long);
		lowerBound = floor((pageSize - headerSize - nodeSize) / (2 * (keySize + nodeSize)));
		upperBound = 2 * lowerBound;
	}

	long Node::getKeyPosition(long key) {
		if (keys.size() == 0 || key <= keys.front()) {
			return 0;
		}

		for (long i = 1; i < (long)keys.size(); ++i) {
			if (keys[i - 1] < key  && key <= keys[i]) {
				return i;
			}
		}
		return keys.size();
	}

	void Node::insert(long key, long left, long right) {

		long position = getKeyPosition(key);
		keys.insert(keys.begin() + position, key);

		children.insert(children.begin() + position + 1, right);

		commit();

		if ((long)keys.size() > upperBound) {
			splitInternal();
		}

		if (index == root->getFileIndex()) {
			root->read();
		}
	}

	void Node::insert(long key, long right) {
		::out << "insert " << key << " to " << getFileName() << endl;
		keys.push_back(key);
		children.push_back((long)right);

		if (keys.size() > upperBound) {
			printNode();
			splitInternal();
		}
	}


	void Node::splitInternal() {
		::out << "splitting Internal node " << getFileName() << endl;
		Node* temp = new Node();
		temp->is_leaf = false;
		::out << "first_step" << endl;
		long first = *(keys.begin() + lowerBound);
		for (auto pos = keys.begin() + lowerBound + 1; pos < keys.end(); pos++) {
			temp->keys.push_back(*pos);
		}

		keys.resize(lowerBound);
		::out << "second_step" << endl;
		for (auto pos = children.begin() + lowerBound + 1; pos < children.end(); pos++) {
			temp->children.push_back(*pos);
			//::out << *pos << endl;
			Node* temp2 = new Node(*pos);
			temp2->parent = temp->index;
			temp2->commit();
			delete temp2;
		}

		children.resize(lowerBound + 1);
		::out << "third_step" << endl;
		if (parent != INITIALIZE) {
			temp->parent = parent;
			temp->commit();
			commit();
			Node* temp3 = new Node(parent);
			::out << temp3->index << endl;
			temp3->insert(first, index, temp->index);
			delete temp3;
		}
		else {
			Node*newParent = new Node();
			newParent->is_leaf = false;
			temp->parent = newParent->index;
			parent = newParent->index;
			newParent->keys.push_back(first);
			newParent->children.resize(2);
			newParent->children[0] = index;
			newParent->children[1] = temp->index;

			newParent->commit();
			commit();
			temp->commit();
			delete root;
			root = newParent;
		}
		::out << endl << temp->index << " " << temp->parent << endl << endl;
		if (nearestParent->index == index) nearestParent = temp;
		else delete temp;
	}

	void Node::splitLeaf() {
		::out << "split leaf " << getFileIndex() << endl;
		Node *temp = new Node();
		for (auto i = lowerBound; i < (long)keys.size(); ++i) {
			temp->words.push_back(words[i]);
			temp->keys.push_back(keys[i]);
		}

		keys.resize(lowerBound);
		words.resize(lowerBound);

		long nextLeaf = next;
		next = temp->index;
		temp->next = nextLeaf;

		if (nextLeaf != INITIALIZE) {
			Node *temp2 = new Node(nextLeaf);
			temp2->prev = temp->index;
			temp2->commit();
			delete temp2;
		}

		temp->prev = index;

		if (parent != INITIALIZE) {
			// Assign parents
			temp->parent = parent;
			temp->commit();
			commit();

			// Now we push up the splitting one level
			Node *temp3 = new Node(parent);
			temp3->insert(temp->keys.front(), index, temp->index);
			delete temp3;
		}
		else {

			::newParent = new Node();
			::newParent->is_leaf = false;

			temp->parent = ::newParent->index;
			parent = ::newParent->index;

			::newParent->keys.push_back(temp->keys.front());

			::newParent->children.push_back(this->index);
			::newParent->children.push_back(temp->index);

			::newParent->commit();
			temp->commit();
			commit();
			delete root;

			// Reset the root node
			root = newParent;
		}

		delete temp;
	}

	void Node::printNode() {
		::out << getFileName() << " has keys:" << endl;
		for (auto key : keys) {
			::out << key << " ";
		}
		::out << endl;
		if (!is_leaf) {
			::out << getFileName() << " has children:" << endl;
			for (auto child : children) {
				::out << child << " ";
			}
		}
		else {
			::out << getFileName() << " has offsets:" << endl;
			for (auto offset : words) {
				::out << offset << " ";
			}
		}
		::out << endl;
	}
	void insert(Node *root, long key, long offset) {
		if (root->isLeaf()) {
			::out << "insert " << key << " to " << root->getFileName() << endl;
			root->keys.push_back(key);
			root->words.push_back(offset);
			if (root->size() > root->upperBound) {
				root->splitLeaf();
			}
		}
		else {
			long pos = root->getKeyPosition(key);
			Node *nextRoot = new Node(root->children[pos]);
			insert(nextRoot, key, offset);
			delete nextRoot;
		}
	}
	void delete_doc(Node *root, long target, string doc) {
		if (!root->isLeaf()) {
			::out << "entering internal node " + root->getFileName() << endl;
			root->printNode();
			long position = root->getKeyPosition(target);
			Node *nextRoot = new Node(root->children[position]);
			delete_doc(nextRoot, target, doc);
			delete nextRoot;
		}
		else {
			::out << "entering leaf node " + root->getFileName() << endl;
			root->printNode();
			for (long i = 0; i < (long)root->keys.size(); i++) {
				if (root->keys[i] == target) {
					::out << "find the target key " << target << " at " << root->words[i] << endl;
					fstream of;
					of.open(TEST_FILE);
					string word;
					of.seekg(root->words[i]);
					while (of >> word) {
						if (word.compare(doc) == 0)  {
							of.seekg(-1*doc.length(), of.cur);
							for (int i = 0; i < doc.length(); i++) of << " ";
							break;
						}
					};
					of.close();
				}
				break;
			}
		}
	}
	void insert_doc(Node *root, long target, string doc) {
		if (!root->isLeaf()) {
			::out << "entering internal node " + root->getFileName() << endl;
			root->printNode();
			long position = root->getKeyPosition(target);
			Node *nextRoot = new Node(root->children[position]);
			insert_doc(nextRoot, target, doc);
			delete nextRoot;
		}
		else {
			::out << "entering leaf node " + root->getFileName() << endl;
			root->printNode();
			for (long i = 0; i < (long)root->keys.size(); i++) {
				if (root->keys[i] == target) {
					::out << "find the target key " << target << " at " << root->words[i] << endl;
					fstream of;
					of.open(TEST_FILE);
					char word;
					int count = 0;
					of.seekg(root->words[i]);
					while (word = of.get()) {
						if (word == ' ')  count++;
						else count = 0;
						if (count == doc.length() + 2) {
							of.seekg(-1 * (--count), of.cur);
							of << doc;
							break;
						}
					};
					of.close();
				}
				break;
			}
		}
	}

	void search(Node *root, long target) {
		if (!root->isLeaf()) {
			::out << "entering internal node " + root->getFileName() << endl;
			root->printNode();
			long position = root->getKeyPosition(target);
			Node *nextRoot = new Node(root->children[position]);
			search(nextRoot, target);
			if (nextRoot->index != root->index) delete nextRoot;
		}
		else {
			::out << "entering leaf node " + root->getFileName() << endl;
			root->printNode();
			for (long i = 0; i < (long)root->keys.size(); i++) {
				if (root->keys[i] == target) {
					::out << "find the target key " << target << " at " << root->words[i] << endl;
					ifstream is;
					is.open(TEST_FILE);
					string line;
					is.seekg(root->words[i]);
					getline(is, line);
					cout << ": " << line << endl;
					is.close();
				}
			}


			 

				Node *tempNode = new Node(root->next);

				if (tempNode->keys.front() == target) {
					search(tempNode, target);
				}

				delete tempNode;
			
		}
	}

	void rangeSearch(Node *root, long lowerLimit, long upperLimit) {
		if (!root->isLeaf()) {
			long position = root->getKeyPosition(lowerLimit);
			Node *nextRoot = new Node(root->children[position]);
			rangeSearch(nextRoot, lowerLimit, upperLimit);
			delete nextRoot;
		}
		else {
			ifstream is;
			string line;
			long key;
			is.open(TEST_FILE);
			long offset = 0;
			for (long i = 0; i < (long)root->keys.size(); ++i) {
				if (root->keys[i] == lowerLimit) {
					is.seekg(root->words[i]);
					while (is >> key && key <= upperLimit) {
						getline(is, line);
						cout << ": " << line << endl;
					}
					break;
				}
			}
			is.close();
		}
	}

	void Node::commit() {

		long location = 0;
		char* buffer = new char[pageSize];
		memcpy(buffer + location, &index, sizeof(index));
		location += sizeof(index);

		memcpy(buffer + location, &is_leaf, sizeof(is_leaf));
		location += sizeof(is_leaf);

		memcpy(buffer + location, &parent, sizeof(parent));
		location += sizeof(parent);

		memcpy(buffer + location, &prev, sizeof(prev));
		location += sizeof(next);

		memcpy(buffer + location, &next, sizeof(next));
		location += sizeof(next);

		long numKeys = keys.size();
		memcpy(buffer + location, &numKeys, sizeof(numKeys));
		location += sizeof(numKeys);


		for (auto key : keys) {
			memcpy(buffer + location, &key, sizeof(key));
			location += sizeof(key);
		}


		if (!is_leaf) {
			for (auto childIndex : children) {
				memcpy(buffer + location, &childIndex, sizeof(childIndex));
				location += sizeof(childIndex);
			}
		}
		else {
			for (auto offset : words) {
				memcpy(buffer + location, &offset, sizeof(offset));
				location += sizeof(offset);
			}
		}

		ofstream nodeFile;
		nodeFile.open(getFileName(), ios::binary | ios::out);
		nodeFile.write(buffer, pageSize);
		nodeFile.close();
	}

	void Node::read() {

		long location = 0;
		char* buffer = new char[pageSize];
		::out << index << endl;
		ifstream nodeFile;
		nodeFile.open(getFileName(), ios::binary | ios::in);
		nodeFile.read(buffer, pageSize);
		nodeFile.close();

		memcpy((char *)&index, buffer + location, sizeof(index));
		location += sizeof(index);
		::out << index << endl;
		memcpy((char *)&is_leaf, buffer + location, sizeof(is_leaf));
		location += sizeof(is_leaf);

		memcpy((char *)&parent, buffer + location, sizeof(parent));
		location += sizeof(parent);

		memcpy((char *)&prev, buffer + location, sizeof(prev));
		location += sizeof(prev);

		memcpy((char *)&next, buffer + location, sizeof(next));
		location += sizeof(next);

		long numKeys;
		memcpy((char *)&numKeys, buffer + location, sizeof(numKeys));
		location += sizeof(numKeys);

		keys.clear();
		long key;
		for (long i = 0; i < numKeys; ++i) {
			memcpy((char *)&key, buffer + location, sizeof(key));
			location += sizeof(key);
			keys.push_back(key);
		}

		if (!is_leaf) {
			children.clear();
			long childIndex;
			for (long i = 0; i < numKeys + 1; ++i) {
				memcpy((char *)&childIndex, buffer + location, sizeof(childIndex));
				location += sizeof(childIndex);
				children.push_back(childIndex);
			}
		}
		else {
			words.clear();
			long offset;
			for (long i = 0; i < numKeys; ++i) {
				memcpy((char *)&offset, buffer + location, sizeof(offset));
				location += sizeof(offset);
				words.push_back(offset);
			}
		}
	}

	void buildTree(string indexfile) {
		ifstream is;
		is.open(indexfile, ios::binary | ios::in);
		//is.imbue(locale(is.getloc(), new codecvt_utf16<char, 0x10FFFF, consume_header>));
		long offset = 0;
		long key;
		string line;
		nearestParent = root;
		while (is) {
			Node* n = new Node();
			offset = is.tellg();
			while (n->keys.size() < Node::upperBound && is >> key) {
				::out << offset << " ";
				insert(n, key, offset);
				getline(is, line);
				offset = is.tellg();
				::out << key << " " << line << endl;
			}
			::out << "add child " << n->index << " to " << nearestParent->index << endl;
			if (root->children.size() == 0) {
				nearestParent->children.push_back((long)n->index);
				n->parent = nearestParent->index;
				n->commit();
				::out << nearestParent->index << " has the first child " << n->index << endl;
			}
			else {
				n->parent = nearestParent->index;
				if (!n->keys.empty()) {
					n->commit();
					nearestParent->Node::insert(n->keys.front() - 1, (long)n->index);
				}
			}
			::out << nearestParent->index << " has " << nearestParent->children.size() << " children" << endl;
			delete n;
		}
		is.close();
	}

	console::console() {
		num_of_pages = 8096;
	}

	console::console(int size) {
		num_of_pages = size;
	}
	void console::initialize(string indexfile) {
		es = new externalSort(num_of_pages, indexfile);
		es->externalMerge();
		rd = new reader("sortedIndex.txt");
		rd->read();
	}

	void console::load(string indexfile) {
		buildTree(indexfile);
	}

	void console::merge(string indexfile) {
		rd->mergeIndex(indexfile, "sortedIndex.txt");
	}

	void console::quit() {
		delete root;
		delete rd;
		delete es;
	}

	void console::insert_doc(string word, string doc) {
		map<string, long> map = rd->getMap();
		long key = map.at(word);
		cout << "key is " << key <<endl;
		::insert_doc(root, key, doc);
	}

	void console::delete_doc(string word ,string doc) {
		map<string, long> map = rd->getMap();
		long key = map.at(word);
		//cout << "key is " << key;
		::insert_doc(root, key, doc);
	}


	string console::search(string s) {
		map<string, long> map = rd->getMap();
		long key = map.at(s);
		cout << "key is " << key <<endl;
		::search(root, key);
	}

	string console::printpath(string key) {

	}

	string console::page(int num) {

	}

	string console::range_search(string k1, string k2) {
		map<string, long> map = rd->getMap();
		long key1 = map.at(k1);
		long key2 = map.at(k2);
		if (key1 <=key2 ) ::rangeSearch(root, key1, key2);
		else ::rangeSearch(root, key2, key1);
	}


int main() {
	Node::initialize(8096);
	root = new Node();
	root->is_leaf = false;
	root->commit();
	::out.open("log.txt", ios::out);
	cout << "start the database\n";
	cout << "enter [text_file] [index_file] [page_size] to build index\n";
	string text_file, index_file;
	int page_size;
	cin >> text_file;
	cin >> index_file;
	cin >> page_size;
	console con(page_size);
	con.initialize(text_file);
	cout << "initialize successfully\n";
	con.load("hashIndex.txt");
	cout << "building tree successfully!\n";
	char f;
	cout << "  ---enter i to insert---\n";
	cout << "  ---enter d to delete---\n";
	cout << "  ---enter c to search---\n";
	cout << "  ---enter q to quit---\n";
	cin >> f;
	while(f!='q'){
		if(f == 'i'){
			cout << "enter [word] [document] pair to insert\n";
			string s1,s2;
			cin >> s1 >> s2;
			con.insert_doc(s1,s2);
		}
		else if(f == 'd'){
			cout << "enter [word] [document] pair to delete\n";
			string s1,s2;
			con.delete_doc(s1,s2);
		}
		else if( f == 'c'){
			cout << "enter keyword to count the keyword\n";
			string s1;
			cin >> s1;
			con.search(s1);
			exit(0);
		}
		else if( f == 'r'){
			cout << "enter two range words\n";
			string s1, s2;
			cin >> s1 >>s2;
			con.range_search(s1,s2);
		}
		else if( f== 'q'){
			con.quit();
			exit(0);
		}
	}
}
