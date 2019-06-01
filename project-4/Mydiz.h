#ifndef MYDIZ_H
#define MYDIZ_H

#include "DinodeList.h"
#include "FileSpace.h"
#include "Queue.h"

class Mydiz {
public:
	Mydiz();
	~Mydiz();
	bool init(char *filename);
	int createFile(char*, Queue *, bool);
	bool newContent(char*, char*, bool);
	int append(char *, Queue *, bool);
	void extract(Queue *);
	void printMetadata();
	void exist(Queue *);
	void printHierarchy();
	//Helpers
	int getDirContents(char *, FileQueue *, FileQueue *, bool);
	int getTopContents(char *, Queue *);
	void getPerms(char *, struct stat);
private:
	BF *bf_;
	DinodeList *dList_;
	FileSpace *fileSpace_;
};

#endif
