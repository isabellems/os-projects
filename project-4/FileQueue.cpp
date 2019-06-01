#include <iostream>
#include <cstdlib>
#include <cstring>

#include "FileQueue.h"

using namespace std;

FileQueueNode::FileQueueNode(bool isDir, bool iszip, char* name, char *owner, char *group, char *perms, char *timestamp, int fileLocation, int fileSize) {
	isDir_ = isDir;
	isZip_ = iszip;
	strcpy(name_, name);
	strcpy(owner_, owner);
	strcpy(group_, group);
	strcpy(perms_, perms);
	strcpy(timestamp_, timestamp);
	fileLocation_ = fileLocation;
	fileSize_ = fileSize;
	next_ = NULL;
}

FileQueue::FileQueue() {
	count_ = 0;
	front_ = rear_ = NULL;
}

FileQueue::~FileQueue() {
	FileQueueNode* temp = NULL;
	for(int i = 0 ; i < count_ ; i++){
		temp = front_;
		front_ = front_->next_;
		delete temp;
	}
}

void FileQueue::add(bool isDir, bool iszip, char* name, char *owner, char *group, char* perms, char *timestamp, int fileLocation, int fileSize) {
	FileQueueNode* add = new FileQueueNode(isDir, iszip, name, owner, group, perms, timestamp, fileLocation, fileSize);
	if(count_ == 0)
		front_ = rear_ = add;
	else{
		rear_->next_ = add;
		rear_ = add;
	}
	count_++;
}

bool FileQueue::remove(bool &isDir, bool &iszip, char* name, char *owner, char *group, char *perms, char *timestamp, int &fileLocation, int &fileSize) {
	if(count_ == 0)
		return false;
	FileQueueNode *temp = front_;
	isDir = temp->isDir_;
	iszip = temp->isZip_;
	strcpy(name, temp->name_);
	strcpy(owner, temp->owner_);
	strcpy(group, temp->group_);
	strcpy(perms, temp->perms_);
	strcpy(timestamp, temp->timestamp_);
	fileLocation = temp->fileLocation_;
	fileSize = temp->fileSize_;
	if(count_ == 1)
		front_ = rear_ = NULL;
	else
		front_ = temp->next_;
	count_--;
	return true;
}

int FileQueue::getCount() {
	return count_;
}
