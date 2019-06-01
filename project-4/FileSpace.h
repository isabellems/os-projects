#ifndef FILESPACE_H
#define FILESPACE_H

#include "sizes.h"
#include "DinodeList.h"

struct Header {
	char name_[MAXBUFFER];
	int metadata_;
	int blockCount_;
	int usedBlocks_;
	int block_;
	Header();
	void getName(char*);
	void setName(char*);
	int getMetadata();
	void setMetadata(int);
};

class FileSpace {
public:
	FileSpace(BF*);
	~FileSpace();
	bool init();
	void writeHeader();
	int addFile(int ,char*);
	void getName(char*);
	void setName(char*);
	int getMetadata();
	void setMetadata(int);
	bool getFileContent(int, int, char*);
	void setList(DinodeList*);
private:
	Header* header_;	
	BF *bf_;
	DinodeList *list_;
};

#endif