#include <iostream>
#include <cstdlib>
#include <cstring>

#include "FileSpace.h"

using namespace std;

void Header::getName(char* name) {
	strcpy(name, name_);
}

void Header::setName(char* name) {
	strcpy(name_, name);
}

int Header::getMetadata() {
	return metadata_;
}

void Header::setMetadata(int block) {
	metadata_ = block;
}

FileSpace::FileSpace(BF* bf) {
	bf_ = bf;
	list_ = NULL;
	header_ = (Header*) malloc(sizeof(Header));
}

bool FileSpace::init(){
	char *buff = (char* )header_;
	if(!bf_->readData(1, sizeof(Header), buff)){
		int block = sizeof(Header) / BLOCKSIZE;
		if(sizeof(Header) % BLOCKSIZE)
			block++;
		header_->usedBlocks_ = 0;
		header_->blockCount_ = FILEBLOCKS;
		header_->block_ = block+1;
		header_->metadata_ = FILEBLOCKS+header_->block_;
		if(!bf_->writeData(1, sizeof(Header), buff))
			return false;
		char *buf =(char*) malloc(FILEBLOCKS*BLOCKSIZE);
		if(!bf_->writeData(header_->block_, FILEBLOCKS*BLOCKSIZE, buf)){
			free(buf);
			return false;
		}
		free(buf);
	}
	else{
		if(!bf_->writeData(1, sizeof(Header), buff))
			return false;
	}
	return true;
}

FileSpace::~FileSpace() {
	free(header_);
}

int FileSpace::addFile(int size_, char* buffer) { 
	int blockNum = size_ / BLOCKSIZE;
	if(size_ % BLOCKSIZE)
		blockNum++;
	bool shift = false;
	int temp = header_->blockCount_;
	char* buf;
	while(blockNum + header_->usedBlocks_ > header_->blockCount_){
		shift = true;
		header_->blockCount_  *= 2;
		header_->metadata_ = header_->block_ + header_->blockCount_;
	}
	buf = (char*) header_;
	if(!bf_->writeData(1, sizeof(Header), buf)){
		free(buf);
		return -1;
	}
	if(shift){
		if(!list_->shiftMetaData(header_->metadata_))
			return -1;
		buf = (char*) malloc((header_->blockCount_ - temp)*BLOCKSIZE);
		if(!bf_->writeData(temp+header_->block_, (header_->blockCount_ - temp) * BLOCKSIZE, buf)){
			free(buf);
			return -1;
		}
		free(buf);
	}
	if(!bf_->writeData(header_->usedBlocks_+header_->block_, size_, buffer))
		return -1;
	else{
		temp = header_->usedBlocks_;
		header_->usedBlocks_ += blockNum;
		buf = (char*) header_;
		if(!bf_->writeData(1, sizeof(Header), buf)){
			free(buf);
			return -1;
		}
		return temp + header_->block_;
	}

}

void FileSpace::getName(char* name)	{
	header_->getName(name);
}

void FileSpace::setName(char* name)	{
	header_->setName(name);
	char* buff = (char*) header_;
	bf_->writeData(1, sizeof(Header), buff);
}

int FileSpace::getMetadata() {
	return header_->getMetadata();
}

void FileSpace::setMetadata(int meta) {
	header_->setMetadata(meta);
	char* buff = (char*) header_;
	bf_->writeData(1, sizeof(Header), buff);
}	

bool FileSpace::getFileContent(int location , int size, char* buffer) {
	return bf_->readData(location, size, buffer);
}

void FileSpace::setList(DinodeList* list) {
	list_ = list;
}

