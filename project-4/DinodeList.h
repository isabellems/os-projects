#ifndef DINODELIST_H
#define DINODELIST_H

#include "BF.h"
#include "sizes.h"
#include "Queue.h"
#include "BfsQ.h" 
#include "FileQueue.h"

struct DinodeEntry{
	void setEntry(char *, int);
	void getEntry(char *, int&);
	char name_[MAXBUFFER];
	int dinode_;
};

struct DirectoryNode{
	void setEntry(char*, int);
	void getEntry(int, char*, int&);
	int count_;
	DinodeEntry entries_[ENTRIES];
};

struct Dinode{
	bool active_;
	bool isDir_;
	bool isZip_;
	char name_[BUFFER];
	char owner_[BUFFER];
	char group_[BUFFER];
	char permissions_[BUFFER];
	char timestamp_[BUFFER];
	int fileLocation_;
	int usedPointers_;
	int inUsePointers_;
	int size_;
	int dirNodes_[DIRNODES]; //offsets of directory nodes
};

struct DiListHeader {
	int headerBlock_;
	int size_;
	int dsize_;
	int block_;
	int dirNodes_;
	int dirNodesBlock_;
	int dirNodesSize_;
	int sizeBytes_;
	int count_;
	int dcount_;
};

class DinodeList{
public:
	DinodeList(BF*);
	~DinodeList();
	bool init(int);
	bool allocateDinodes(FileQueue*);
	bool allocateDirectnodes(char* , Queue*);
	bool shiftMetaData(int);
	int getSizeBytes();
	int getNodesSize();
	int getNodesBlock();
	void setBlock(int);
	bool exists(char*);
	void printMetaData();
	void printHierarchy();
	void printLevel(char*, int&);
	int getFileLocation(char*, int&, bool&);
	bool extractFile(char*);
	bool BFS(BfsQ*);
private:
	Dinode *diNodes_;
	DirectoryNode *directories_;
	DiListHeader *header_;
	BF *bf_;
};


#endif
