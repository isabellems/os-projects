#include <iostream>
#include <cstdlib>
#include <cstring>
#include "BfsQ.h"

using namespace std;

BfsNode::BfsNode(char *path, char *parent) {
	strcpy(path_, path);
	strcpy(parent_, parent);
	next_ = NULL;
}

BfsQ::BfsQ() {
	count_ = 0;
	front_ = rear_ = NULL;
}

BfsQ::~BfsQ() {
	BfsNode* temp = NULL;
	for(int i = 0 ; i < count_ ; i++){
		temp = front_;
		front_ = front_->next_;
		delete temp;
	}
}

void BfsQ::add(char *path, char* parent) {
	BfsNode* add = new BfsNode(path, parent);
	if(count_ == 0){
		front_ = rear_ = add;
	}
	else{
		rear_->next_ = add;
		rear_ = add;
	}
	count_++;
}

bool BfsQ::remove(char *path, char *parent) {
	if(count_ == 0)
		return false;
	BfsNode *temp = front_;
	strcpy(path, temp->path_);
	strcpy(parent, temp->parent_);
	if(count_ == 1)
		front_ = rear_ = NULL;
	else
		front_ = temp->next_;
	count_--;
	delete temp;
	return true;
}

int BfsQ::getCount() {
	return count_;
}
