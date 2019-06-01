#ifndef BFSQ_H
#define BFSQ_H

#include "sizes.h"

struct BfsNode {
	BfsNode(char *, char*);
	char path_[MAXBUFFER];
	char parent_[MAXBUFFER];
	BfsNode *next_;
};

class BfsQ {
public:
	BfsQ();
	~BfsQ();
	void add(char *, char*);
	bool remove(char *, char*);
	int getCount();
private:
	int count_;
	BfsNode* front_;
	BfsNode* rear_;
};

#endif