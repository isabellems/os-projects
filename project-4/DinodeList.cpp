#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <cstring>
#include <ctime>

#include "DinodeList.h"

using namespace std;

void DinodeEntry::setEntry(char *name, int dinode) {
	strcpy(name_, name);
	dinode_ = dinode;
}

void DinodeEntry::getEntry(char *name, int &dinode) {
	strcpy(name, name_);
	dinode = dinode_;
}

void DirectoryNode::setEntry(char *name, int dinode) {
	entries_[count_].setEntry(name, dinode);
	count_++;
}

void DirectoryNode::getEntry(int pos , char *name, int &dinode) {
	entries_[pos].getEntry(name, dinode);
}

DinodeList::DinodeList(BF *bf) {
	bf_ = bf;
	diNodes_ = NULL;
	directories_ = NULL;
    header_ = (DiListHeader*) malloc(sizeof(DiListHeader));
}

bool DinodeList::init(int block){
    char* buf = (char*) header_;
	if(!bf_->readData(block, sizeof(DiListHeader), buf)){
		header_->size_ = DINODES;
		header_->dsize_ = DIRNODES;
		header_->dcount_ = 0;
		header_->count_ = 0;
		header_->sizeBytes_ = header_->size_ * sizeof(Dinode);
		header_->dirNodes_ = 0;
		header_->dirNodesSize_ = header_->dsize_ * sizeof(DirectoryNode);;
		header_->headerBlock_ = block;
    	diNodes_ = (Dinode*) malloc(header_->size_ * sizeof(Dinode));
    	memset(diNodes_, 0, header_->dsize_ * sizeof(Dinode));
    	directories_ = (DirectoryNode*) malloc(header_->dsize_ * sizeof(DirectoryNode));
    	memset(directories_, 0, header_->dsize_ * sizeof(DirectoryNode));
    	for(int i = 0 ; i < header_->dsize_ ; i++)
    		directories_[i].count_ = 0;
    	int blockNum = sizeof(DiListHeader) / BLOCKSIZE;
		if(sizeof(DiListHeader) % BLOCKSIZE)
			blockNum++;
		header_->block_ = blockNum;
		buf = (char*) header_;
		if(!bf_->writeData(header_->headerBlock_, sizeof(DiListHeader), buf))
			return false;
		buf = (char*) diNodes_;
		if(!bf_->writeData(header_->block_ + header_->headerBlock_, header_->sizeBytes_, buf))
			return false;
		blockNum = header_->sizeBytes_ / BLOCKSIZE;
		if(header_->sizeBytes_ % BLOCKSIZE)
			blockNum++;
		buf = (char*) directories_;
		header_->dirNodesBlock_ = blockNum + header_->block_;
		if(!bf_->writeData(header_->dirNodesBlock_ + header_->headerBlock_, header_->dirNodesSize_, buf))
			return false;
		buf = (char*) header_;
		if(!bf_->writeData(header_->headerBlock_, sizeof(DiListHeader), buf))
			return false;
	}
	else{
    	diNodes_ = (Dinode*) malloc(header_->size_ * sizeof(Dinode));
    	directories_ = (DirectoryNode*) malloc(header_->dsize_ * sizeof(DirectoryNode));
		buf = (char*) diNodes_;
		if(!bf_->readData(header_->block_ + header_->headerBlock_, header_->sizeBytes_, buf))
			return false;
		buf = (char*) directories_;
		if(!bf_->readData(header_->dirNodesBlock_ + header_->headerBlock_, header_->dirNodesSize_, buf))
			return false;
	}
	return true;
}

DinodeList::~DinodeList(){
	if(diNodes_!=NULL)
		free(diNodes_);
	if(directories_!=NULL)
		free(directories_);
	free(header_);
}

bool DinodeList::allocateDinodes(FileQueue *fq){
	int count = fq->getCount();
	int blockNum;
	int offset;
	int temp = header_->size_;
	while(header_->count_ + count > header_->size_){
		header_->size_ *= 2;
		diNodes_ = (Dinode*) realloc(diNodes_, header_->size_ * sizeof(Dinode));
		header_->sizeBytes_ = header_->size_ * sizeof(Dinode);
	}
	if(header_->size_ > temp && header_->dsize_ > 0){
		char* buffer = (char*) directories_;
		blockNum = header_->sizeBytes_ / BLOCKSIZE;
		if(header_->sizeBytes_ % BLOCKSIZE != 0)
			blockNum++; 
		offset = blockNum + header_->headerBlock_ + header_->block_;
		if(!bf_->writeData(offset, header_->dirNodesSize_, buffer))
			return false;
		header_->dirNodesBlock_ = blockNum + header_->block_;
	}
	bool isDir, isZip;
	char* name = (char*) malloc(BUFFER);
	char* owner = (char*) malloc(BUFFER);
	char* group = (char*) malloc(BUFFER);
	char* timestamp = (char*) malloc(BUFFER);
	char* perms = (char*) malloc(BUFFER);
	char* buffer;
	int fileLocation;
	int fileSize;
	Dinode *dinode;
	while(fq->remove(isDir, isZip, name, owner, group, perms, timestamp, fileLocation, fileSize)){
		dinode = (Dinode*) malloc(sizeof(Dinode));
		dinode->active_ = true;
		dinode->isDir_ = isDir;
		dinode->isZip_ = isZip;
		dinode->usedPointers_ = 0;
		dinode->inUsePointers_ = 0;
		strcpy(dinode->name_, name);
		strcpy(dinode->owner_, owner);
		strcpy(dinode->group_, group);
		strcpy(dinode->permissions_, perms);
		strcpy(dinode->timestamp_, timestamp);
		dinode->fileLocation_ = fileLocation;
		dinode->size_ = fileSize;
		for(int i = 0 ; i < DIRNODES ; i++)
			dinode->dirNodes_[i] = -1;
		offset = header_->count_;
		memcpy(&diNodes_[offset], dinode, sizeof(Dinode));
		header_->count_ ++;
		free(dinode);
	}
	buffer = (char*) header_;
	if(!bf_->writeData(header_->headerBlock_, sizeof(DiListHeader), buffer)){
		free(name);
		free(owner);
		free(group);
		free(timestamp);
		free(perms);
		free(dinode);
		return false;
	}
	free(name);
	free(owner);
	free(group);
	free(timestamp);

	buffer = (char*) diNodes_;
	if(!bf_->writeData(header_->headerBlock_ + header_->block_, header_->sizeBytes_, buffer))
		return false;
	return true;
}

bool DinodeList::allocateDirectnodes(char* filename, Queue* files) {
	int pos = -1, filepos = -1;
	for(int i = 0 ; i < header_->count_ ; i++){
		if(!strcmp(diNodes_[i].name_, filename)){
			pos = i;
			break;
		}
	}
	if(pos < 0)
		return false;
	char name[BUFFER];
	int used, inuse, offset, count, dirnodes = 0, entries = 0, temp, start, lastused;
	bool flag = false;
	used = diNodes_[pos].usedPointers_;
	inuse = diNodes_[pos].inUsePointers_;
	if(used >= header_->dsize_)
		return false;
	count = files->getCount();
	if(inuse > used){
		lastused = diNodes_[pos].dirNodes_[used];
		entries = ENTRIES - (directories_[lastused].count_);
	    if(count > entries){
	    	flag = true;
			count -= entries;
	    }
	}
	if(inuse == used || flag == true){
		dirnodes = (count / ENTRIES);
		if(count!=0 && count % ENTRIES!=0){
			dirnodes++;
		}
	}
	temp = header_->dsize_;
	while(dirnodes + header_->dcount_ > header_->dsize_){
		header_->dsize_ = header_->dsize_ * 2;
		directories_ = (DirectoryNode *) realloc(directories_, header_->dsize_ * sizeof(DirectoryNode));
		header_->dirNodesSize_ = header_->dsize_ * sizeof(DirectoryNode);
	}
	if(temp < header_->dsize_){
		int start = header_->dsize_ - temp;
		for(int i = start ; i <= header_->dsize_ ; i++)
			directories_[i].count_ = 0;
	}
	for(int i = 0 ; i < dirnodes ; i++) {
		diNodes_[pos].dirNodes_[inuse+i] = header_->dcount_; 
		header_->dcount_++;
		diNodes_[pos].inUsePointers_++;
	}
	if(entries!=0){
		for(int i = 0 ; i < entries ; i++) {
			int node = 0;
			if(files->remove(name)){
				for(int j = 0 ; j < header_->count_ ; j++){
					if(!strcmp(diNodes_[j].name_, name)){
						node = j;
						break;
					}
				}
				directories_[lastused].setEntry(name, node);
				if(i == entries - 1){
					used++;
					diNodes_[pos].usedPointers_++;
				}
			}
			else
				break;
		}
	}
	for(int i = 0 ; i < dirnodes ; i++){
		lastused = diNodes_[pos].dirNodes_[used];
		for(int j = 0 ; j < ENTRIES ; j++){
			int node = 0;
			if(files->remove(name)){
				for(int k = 0 ; k < header_->count_ ; k++){
					if(!strcmp(diNodes_[j].name_, name)){
						node = j;
						break;
					}
				}
				directories_[lastused].setEntry(name, node);
				if(j == ENTRIES - 1){
					used++;
					diNodes_[pos].usedPointers_++;
				}
			}
			else
				break;
		}
	}
	char* buffer = (char*) header_;
	if(!bf_->writeData(header_->headerBlock_, sizeof(DiListHeader), buffer))
		return false;
	buffer = (char*) diNodes_;
	if(!bf_->writeData(header_->headerBlock_ + header_->block_, header_->sizeBytes_, buffer))
		return false;
	buffer = (char*) directories_;
	if(!bf_->writeData(header_->headerBlock_ + header_->dirNodesBlock_, header_->dirNodesSize_, buffer))
		return false;

	return true;
}

bool DinodeList::shiftMetaData(int blockNum) {
	header_->headerBlock_ = blockNum;
	char* buffer = (char*) header_;
	if(!bf_->writeData(header_->headerBlock_, sizeof(DiListHeader), buffer))
		return false;
	buffer = (char*) diNodes_;
	if(!bf_->writeData(header_->headerBlock_ + header_->block_, header_->sizeBytes_, buffer))
		return false;
	buffer = (char*) directories_;
	if(!bf_->writeData(header_->headerBlock_ + header_->dirNodesBlock_, header_->dirNodesSize_, buffer))
		return false;
	return true;
}

int DinodeList::getSizeBytes() {
	return header_->sizeBytes_;
}

int DinodeList::getNodesSize() {
	return header_->dirNodesSize_;
}

int DinodeList::getNodesBlock() {
	return header_->dirNodesBlock_;
}

void DinodeList::setBlock(int BlockNum) {
	header_->block_ = BlockNum;
	char* buff = (char*) header_;
	bf_->writeData(header_->headerBlock_, sizeof(DiListHeader), buff);
}

bool DinodeList::exists(char* name) {
	for(int i = 0 ; i < header_->count_ ; i++){
		if(!strcmp(diNodes_[i].name_, name))
			return true;
	}
	return false;
}

void DinodeList::printMetaData() {
	char name[MAXBUFFER], *base;
	for(int i = 0 ; i < header_->count_ ; i++){
		if(strcmp(diNodes_[i].name_, "root")){
			strcpy(name, diNodes_[i].name_);
			base = basename(name);
			cout << diNodes_[i].permissions_ << " " << diNodes_[i].owner_ << " " << diNodes_[i].group_ << " " << diNodes_[i].size_ << " " << base << " " << diNodes_[i].timestamp_ << endl;
		}
	}
}

void DinodeList::printHierarchy() {
	char buff[MAXBUFFER];
	int i = 0;
	strcpy(buff, "root");
	printLevel(buff, i);
}

void DinodeList::printLevel(char* filename, int &level) {
	char buffer[MAXBUFFER];
	char tab[MAXBUFFER];
	char *base;
	memset(tab, 0, MAXBUFFER);
	int pos = -1, inuse = 0, count, dir, dummy;
	for(int i = 0 ; i < header_->count_ ; i++){
		if(!strcmp(diNodes_[i].name_, filename)){
			pos = i;
			break;
		}
	}
	memset(tab, ' ', level);
	tab[level] = '\0';
	base = basename(filename);
	cout << tab << base << endl;
	level++;
	if(pos < 0)
		return;
	if(diNodes_[pos].isDir_ == 1){
		inuse = diNodes_[pos].inUsePointers_;
		for(int i = 0; i < inuse ; i++){
			dir = diNodes_[pos].dirNodes_[i];
			count = directories_[dir].count_;
			for(int j = 0 ; j < count ; j++){
				directories_[dir].entries_[j].getEntry(buffer, dummy);
				printLevel(buffer, level);
			}
		}
	}	
	level--;
}

int DinodeList::getFileLocation(char* name, int &size, bool &zip) {
	int pos = -1;
	for(int i = 0 ; i < header_->count_ ; i++){
		if(!strcmp(diNodes_[i].name_, name)){
			pos = i;
			break;
		}
	}
	if(pos < 0)
		return -1;
	if(diNodes_[pos].isDir_ == false){
		size = diNodes_[pos].size_;
		zip = diNodes_[pos].isZip_;
		return diNodes_[pos].fileLocation_;
	}
	else
		return -1;
}	

bool DinodeList::extractFile(char* name) {
	BfsQ *q = new BfsQ();
	char path[MAXBUFFER];
	memset(path, 0, MAXBUFFER);
	q->add(name, path);
	bool ret = BFS(q);
	delete q;
	return ret;
}

bool DinodeList::BFS(BfsQ *q) {
	int pos = -1, block, size, count, dir, entries, dummy;
	char name[MAXBUFFER], path[MAXBUFFER], temp[MAXBUFFER], str[MAXBUFFER], *base;
	memset(name, 0, MAXBUFFER);
	memset(path, 0, MAXBUFFER);
	pid_t pid;
	while(q->remove(name, path)){
		for(int i = 0 ; i < header_->count_ ; i++){
			if(!strcmp(diNodes_[i].name_, name)){
				pos = i;
				break;
			}
		}
		if(pos < 0)
			return false;
		strcpy(str, name);
		base = basename(str);
		sprintf(name, "%s%s", path, base);
		if(!diNodes_[pos].isDir_){
			block = diNodes_[pos].fileLocation_;
			size = diNodes_[pos].size_;
			char* buf = (char*) malloc(size);
			if(!bf_->readData(block, size, buf)){
				free(buf);
				return false;
			}
			FILE *f = fopen(name, "w");
			fwrite(buf, size, 1, f);
			fclose(f);
			free(buf);
			if(diNodes_[pos].isZip_){
				pid = fork();
				if(pid < 0){
					cerr << "Fork Failed." << endl;
					exit(1);
				}
				else if(pid == 0){
					if(execlp("gzip", "gzip", "-d", name, NULL) < 0)
					cout << "Execl error." << endl;
					exit(0);
				}
				waitpid(-1,NULL,0);
			}
		}
		else if(diNodes_[pos].isDir_ == true){
			mkdir(name, S_IRWXU | S_IRWXG | S_IRWXO);
			count = diNodes_[pos].inUsePointers_;
			for(int i = 0 ; i < count ; i++){
				dir = diNodes_[pos].dirNodes_[i];
				entries = directories_[dir].count_;
				for(int j = 0 ; j < entries ; j++){
					char content[MAXBUFFER];
					directories_[dir].entries_[j].getEntry(content, dummy);
					sprintf(path, "%s/", name);
					q->add(content, path);
				}
			}
		}
	}
	return true;
}

