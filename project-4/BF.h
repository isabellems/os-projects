#ifndef BF_H
#define BF_H

#include "sizes.h"

struct RandomStruct {
	char array[40];
	int num;
	RandomStruct() {
		num = 0;
	}
};

class BF {
public:
	BF();
	int createFile(char *);
	int openFile(char *);
	bool closeFile();
	int getBlockCounter();
	int allocateBlock();
	bool readBlock(int, void*);
	bool writeBlock(int, void*);
	bool readData(int, int, char*);
	bool writeData(int, int, char*);
	bool writeHeader(int);
	int readHeader();
private:
	char filename_[MAXBUFFER];
	int fd_;
	int blockCounter_;
	bool open_;
};

#endif