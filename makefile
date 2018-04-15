CC=g++

dbs: bpt.cpp externalSort.cpp reader.cpp
	$(CC) -o database bpt.cpp externalSort.cpp reader.cpp

