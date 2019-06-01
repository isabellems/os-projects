#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include "BF.h"

using namespace std;

BF::BF() {
	fd_ = -1;
	blockCounter_ = 0;
	open_ = false;
}

int BF::createFile(char *filename) {
	int fd;
	if((fd = open(filename, O_RDWR | O_CREAT| O_TRUNC , S_IRWXU | S_IRWXG)) >= 0){
		strcpy(filename_, filename);
		fd_ = fd;
		open_ = true;
		if(allocateBlock() < 0)
			return false;
		if(!writeHeader(1))
			return false;
		return fd;
	}
	return -1;
}

int BF::openFile(char *filename) {
	int fd, header;
	strcpy(filename_ ,filename);
	open_ = true;
	if((fd = open(filename, O_RDWR)) >= 0){
		fd_ = fd;
		header = readHeader();
		if(header >=0){
			blockCounter_ = header;
		}
	}
	else{
		fd = createFile(filename);
	}
	return fd;
}

bool BF::closeFile() {
	open_ = false;
	return (close(fd_) == 0);
}

int BF::getBlockCounter() {
	return blockCounter_;
}

int BF::allocateBlock() {
	if(!open_)
		return -1;
	int counter;
	void* block = (void*) malloc(BLOCKSIZE);
	memset(block, 0, BLOCKSIZE);
	if(pwrite(fd_, block, BLOCKSIZE, blockCounter_ * BLOCKSIZE) < BLOCKSIZE)
		counter = -1;
	else{
		if(blockCounter_ != 0)
			writeHeader(blockCounter_+1);
		counter = blockCounter_;
		blockCounter_++;
	}
	free(block);
	return counter;
}

bool BF::readBlock(int blockNumber, void *data) {
	if(blockNumber + 1 > blockCounter_ && blockNumber!= 0){
		return false;
	}
	return (pread(fd_, data, BLOCKSIZE, blockNumber * BLOCKSIZE));
}

bool BF::writeBlock(int blockNumber, void *data) {
	if(blockNumber + 1 > blockCounter_ && blockNumber!= 0)
		return false;
	if(pwrite(fd_, data, BLOCKSIZE, blockNumber * BLOCKSIZE) < BLOCKSIZE)
		return false;
	else
		return true;
}

bool BF::readData(int blockNumber, int size, char* buffer) {
	if(!open_)
		return false;
	if(blockNumber + 1 > blockCounter_)
		return false;
	int number = blockNumber;
	int read = 0, written = 0;
	void *bl = (void*) malloc(BLOCKSIZE);
	memset(bl, 0, BLOCKSIZE);
	while(read < size){
		if(!readBlock(number, bl)){
			free(bl);
			return false;
		}
		if((size-written) / BLOCKSIZE == 0){
			memcpy(buffer+read, bl, size-written);
			written = size;
		}
		else{
			memcpy(buffer+read, bl, BLOCKSIZE);
			written += BLOCKSIZE;
		}
		read += BLOCKSIZE;
		number++;
	}
	free(bl);
	return true;
}

bool BF::writeData(int blockNumber, int size, char* buffer) {
	if(!open_)
		return false;
	int written = 0;
	int number = blockNumber;
	void *bl = (void*) malloc(BLOCKSIZE);
	memset(bl, 0, BLOCKSIZE);
	while(written < size){
		memset(bl, 0, BLOCKSIZE);
		while(number + 1 > blockCounter_){
			number = allocateBlock();
			if(number < 0)
				return false;
		}
		if(((size-written) / BLOCKSIZE) == 0){
			memcpy(bl, buffer+written, size-written);
			written = size;
		}
		else{ 
			memcpy(bl, buffer+written, BLOCKSIZE);
			written += BLOCKSIZE;
		}
		if(!writeBlock(number, bl)){
			free(bl);
			return false;
		}
		number++;
	}
	free(bl);
	return true;
}

bool BF::writeHeader(int blockCount) {
	void *block = (void*) malloc(BLOCKSIZE);
	memset(block, 0, BLOCKSIZE);
	memcpy(block, &blockCount, sizeof(int));
	if(writeBlock(0, block)){
		free(block);
		return true;
	}
	else{
		free(block);
		return false;
	}
}

int BF::readHeader() {
	int *num, ret = -1;
	void *block = (void*) malloc(BLOCKSIZE);
	memset(block, 0, BLOCKSIZE);
	if(readBlock(0, block)){
		char *numstr = (char*) malloc(sizeof(int));
		memcpy(numstr, block, sizeof(int));
		num = (int*) numstr;
		free(block);
		ret = *num;
		free(numstr);
		return ret;
	}
	else{
		free(block);
		return -1;
	}
}