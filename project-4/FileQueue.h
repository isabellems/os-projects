#ifndef FILEQUEUE_H
#define FILEQUEUE_H

#include "sizes.h"

struct FileQueueNode {
	FileQueueNode(bool, bool, char*, char *, char *, char *, char *, int, int);
	bool isDir_;
	bool isZip_;
	char name_[BUFFER];
	char owner_[BUFFER];
	char group_[BUFFER];
	char perms_[BUFFER];
	char timestamp_[BUFFER];
	int fileLocation_; //block number
	int fileSize_;
	FileQueueNode *next_;
};

class FileQueue {
public:
	FileQueue();
	~FileQueue();
	void add(bool, bool, char*, char *, char *, char *, char *, int, int);
	bool remove(bool &, bool&, char*, char *, char *, char *, char *, int &, int &);
	int getCount();
private:
	int count_;
	FileQueueNode* front_;
	FileQueueNode* rear_;
};

#endif