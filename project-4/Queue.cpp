#include <iostream>
#include <cstdlib>
#include <cstring>
#include "Queue.h"

using namespace std;

QueueNode::QueueNode(char *path) {
	strcpy(path_, path);
	next_ = NULL;
}

Queue::Queue() {
	count_ = 0;
	front_ = rear_ = NULL;
}

Queue::~Queue() {
	QueueNode* temp = NULL;
	for(int i = 0 ; i < count_ ; i++){
		temp = front_;
		front_ = front_->next_;
		delete temp;
	}
}

void Queue::add(char *path) {
	QueueNode* add = new QueueNode(path);
	if(count_ == 0){
		front_ = rear_ = add;
	}
	else{
		rear_->next_ = add;
		rear_ = add;
	}
	count_++;
}

void Queue::addFront(char *path) {
    QueueNode* add = new QueueNode(path);
    if(count_ == 0){
        front_ = rear_ = add;
    }
    else{
        add->next_ = front_;
        front_ = add;
    }
    count_++;
}

bool Queue::remove(char *path) {
	if(count_ == 0)
		return false;
	QueueNode *temp = front_;
	strcpy(path, temp->path_);
	if(count_ == 1)
		front_ = rear_ = NULL;
	else
		front_ = temp->next_;
	count_--;
	delete temp;
	return true;
}

bool Queue::peek(int pos, char* name) {
	if(pos > count_-1)
		return false;
	QueueNode *temp = front_;
	for(int i = 0 ; i < pos ; i++)
		temp = temp->next_;
	strcpy(name, temp->path_);
	return true;
}

int Queue::getCount() {
	return count_;
}
