#ifndef QUEUE_H
#define QUEUE_H

#include "sizes.h"

struct QueueNode {
	QueueNode(char *);
	char path_[MAXBUFFER];
	QueueNode *next_;
};

class Queue {
public:
	Queue();
	~Queue();
	void add(char *);
	bool remove(char *);
	bool peek(int, char*);
	int getCount();
	void addFront(char *);
private:
	int count_;
	QueueNode* front_;
	QueueNode* rear_;
};

#endif