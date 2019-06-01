#include <iostream>
#include <cstring>
#include <libgen.h>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <dirent.h>

#include "sizes.h"
#include "Queue.h"
#include "FileQueue.h"
#include "BF.h"
#include "Mydiz.h"

using namespace std;

Mydiz::Mydiz(){
	bf_ = new BF();
	fileSpace_ = new FileSpace(bf_);
	dList_ = new DinodeList(bf_);
	fileSpace_->setList(dList_);

}

bool Mydiz::init(char *filename){
	if(bf_->openFile(filename) < 0)
		return false;
	if(!fileSpace_->init())
		return false;
	int diListBlock = fileSpace_->getMetadata();
	return (dList_->init(diListBlock));
}

Mydiz::~Mydiz(){
	delete bf_;
	delete fileSpace_;
	delete dList_;
}

int Mydiz::createFile(char *file, Queue *arguments, bool compress){
	char *filename1 = (char *) malloc(MAXBUFFER * sizeof(char));
	char *filename2 = (char *) malloc(MAXBUFFER * sizeof(char));
	char *dummy1 = (char *) malloc(MAXBUFFER * sizeof(char));
	char name[BUFFER], owner[BUFFER], group[BUFFER], perms[BUFFER], timestamp[BUFFER], zipName[BUFFER], command[BUFFER], *str, tempstr[BUFFER];
	int dummy2, fileSize, fileLocation;
	bool dummy3;
	int args = arguments->getCount();
	bool isDir;
	FileQueue *fileQueue = new FileQueue();
	FileQueue *fileQueueCopy = new FileQueue();
    struct stat fileStat;
    struct group *grp;
	struct passwd *pwd;
	for(int i = 0; i < args; i++){
		arguments->peek(i, filename1);
		stat(filename1,&fileStat);
		if(S_ISREG(fileStat.st_mode)){
			if(compress){
				sprintf(command, "gzip %s", filename1);
				system(command);
				sprintf(zipName, "%s.gz", filename1);
				strcpy(filename1, zipName);
				stat(filename1,&fileStat);
			}
			strcpy(name, filename1);
			//Get stat data
			grp = getgrgid(fileStat.st_gid);
			strcpy(group, grp->gr_name);
			pwd = getpwuid(fileStat.st_uid);
			strcpy(owner, pwd->pw_name);
			getPerms(perms, fileStat);
			strftime(timestamp, BUFFER, "%d - %m - %y", localtime(&(fileStat.st_ctime)));
			fileSize = fileStat.st_size;
			FILE *fp = fopen(filename1, "rb");
			char *buffer = (char *) malloc(fileSize * sizeof(char) + 1);
			fread(buffer, fileSize, 1, fp);
			fclose(fp);
			buffer[fileSize] = 0;
			strcpy(tempstr, name);
			fileLocation = fileSpace_->addFile(fileSize, buffer);
			fileQueue->add(false, compress, name, owner, group, perms, timestamp, fileLocation, fileSize);
			fileQueueCopy->add(false, compress, name, owner, group, perms, timestamp, fileLocation, fileSize);
		}
		else{
			strcpy(name, filename1);
			//Get stat data
			grp = getgrgid(fileStat.st_gid);
			strcpy(group, grp->gr_name);
			pwd = getpwuid(fileStat.st_uid);
			strcpy(owner, pwd->pw_name);
			getPerms(perms, fileStat);
			strftime(timestamp, BUFFER, "%d - %m - %y", localtime(&(fileStat.st_ctime)));
			fileSize = fileStat.st_size;
			strcpy(tempstr, name);
			fileQueue->add(true, false, name, owner, group, perms, timestamp, fileLocation, fileSize);
			fileQueueCopy->add(true, false, name, owner, group, perms, timestamp, fileLocation, fileSize);
			getDirContents(filename1, fileQueue, fileQueueCopy, compress);
		}
		if(!dList_->allocateDinodes(fileQueue)){
			delete fileQueue;
			delete fileQueueCopy;
			free(dummy1);
			free(filename1);
			free(filename2);
			return -1;
		}
		int count = fileQueueCopy->getCount();
		char* base;
		for(int j = 0; j < count; j++){
			fileQueueCopy->remove(isDir, dummy3, filename2, dummy1, dummy1, dummy1, dummy1, dummy2, dummy2);
			if(isDir){
				Queue *q = new Queue();
				getTopContents(filename2, q);
				if(!dList_->allocateDirectnodes(filename2, q)){
					delete q;
					delete fileQueue;
					delete fileQueueCopy;
					free(dummy1);
					free(filename1);
					free(filename2);
					return -1;
				}
				delete q;
			}
		}
	}
	//Save root metadata
	FileQueue *rootQueue = new FileQueue();
	strcpy(filename1, "root");
	rootQueue->add(true, compress, filename1, filename1, filename1, filename1, filename1, -1, 0);
	if(!dList_->allocateDinodes(rootQueue)){
		delete rootQueue;
		delete fileQueue;
		delete fileQueueCopy;
		free(dummy1);
		free(filename1);
		free(filename2);
		return -1;
	}
	delete rootQueue;
	if(!dList_->allocateDirectnodes(filename1, arguments)){
		delete fileQueue;
		delete fileQueueCopy;
		free(dummy1);
		free(filename1);
		free(filename2);
		return -1;
	}
	delete fileQueue;
	delete fileQueueCopy;
	free(dummy1);
	free(filename1);
	free(filename2);
	return 0;
}

bool Mydiz::newContent(char* parent, char* content, bool compress)	{
	char *filename1 = (char *) malloc(MAXBUFFER * sizeof(char));
	char *filename2 = (char *) malloc(MAXBUFFER * sizeof(char));
	char *dummy1 = (char *) malloc(MAXBUFFER * sizeof(char));
	char name[BUFFER], owner[BUFFER], group[BUFFER], perms[BUFFER], timestamp[BUFFER], zipName[BUFFER], command[BUFFER], *str, tempstr[BUFFER];
	int dummy2, fileSize, fileLocation;
	bool dummy3;
	bool isDir;
	FileQueue *fileQueue = new FileQueue();
	FileQueue *fq = new FileQueue();
	Queue *fileQueueCopy = new Queue();

    struct stat fileStat;
    struct group *grp;
	struct passwd *pwd;
	stat(content, &fileStat);
	if(S_ISREG(fileStat.st_mode)){
		if(compress){
			sprintf(command, "gzip %s", content);
			system(command);
			sprintf(zipName, "%s.gz", content);
			strcpy(content, zipName);
			stat(content,&fileStat);
		}
		strcpy(name, content);
		grp = getgrgid(fileStat.st_gid);
		strcpy(group, grp->gr_name);
		pwd = getpwuid(fileStat.st_uid);
		strcpy(owner, pwd->pw_name);
		getPerms(perms, fileStat);
		strftime(timestamp, BUFFER, "%d - %m - %y", localtime(&(fileStat.st_ctime)));
		fileSize = fileStat.st_size;
		FILE *fp = fopen(content, "rb");
		char *buffer = (char *) malloc(fileSize * sizeof(char) + 1);
		fread(buffer, fileSize, 1, fp);
		fclose(fp);
		buffer[fileSize] = 0;
		strcpy(tempstr, name);
		fileLocation = fileSpace_->addFile(fileSize, buffer);
		fileQueue->add(false, compress, name, owner, group, perms, timestamp, fileLocation, fileSize);
		fq->add(false, compress, name, owner, group, perms, timestamp, fileLocation, fileSize);
		fileQueueCopy->add(name);
	}
	else{
		strcpy(name, content);
		//Get stat data
		grp = getgrgid(fileStat.st_gid);
		strcpy(group, grp->gr_name);
		pwd = getpwuid(fileStat.st_uid);
		strcpy(owner, pwd->pw_name);
		getPerms(perms, fileStat);
		strftime(timestamp, BUFFER, "%d - %m - %y", localtime(&(fileStat.st_ctime)));
		fileSize = fileStat.st_size;
		strcpy(tempstr, name);
		fileQueue->add(true, false, name, owner, group, perms, timestamp, 0, 0);
		fq->add(true, false, name, owner, group, perms, timestamp, 0, 0);
		fileQueueCopy->add(name);
		getDirContents(content, fileQueue, fq, compress);

	}

	if(!dList_->allocateDinodes(fileQueue)){
		delete fileQueue;
		delete fq;
		delete fileQueueCopy;
		free(dummy1);
		free(filename1);
		free(filename2);
		return -1;
	}
	int count = fq->getCount();
	char* base;
	for(int j = 0; j < count; j++){
		fq->remove(isDir, dummy3, filename2, dummy1, dummy1, dummy1, dummy1, dummy2, dummy2);
		if(isDir){
			Queue *q = new Queue();
			getTopContents(filename2, q);
			if(!dList_->allocateDirectnodes(filename2, q)){
				delete q;
				delete fq;
				delete fileQueue;
				delete fileQueueCopy;
				free(dummy1);
				free(filename1);
				free(filename2);
				return -1;
			}
			delete q;
		}
	}

	if(!dList_->allocateDirectnodes(parent, fileQueueCopy)){
		delete fileQueue;
		delete fq;
		delete fileQueueCopy;
		free(dummy1);
		free(filename1);
		free(filename2);
		return -1;
	}

	delete fileQueue;
	delete fq;
	delete fileQueueCopy;
	return 0;
}

int Mydiz::append(char *filename, Queue *arguments, bool compress){
	char filename1[MAXBUFFER], filename2[MAXBUFFER], copyFilename[MAXBUFFER], parent[MAXBUFFER], buildPath[MAXBUFFER], owner[BUFFER], group[BUFFER], perms[BUFFER], timestamp[BUFFER], zipName[MAXBUFFER];
	char fullPath[MAXBUFFER], command[MAXBUFFER], name[BUFFER], part1[MAXBUFFER], part2[MAXBUFFER], str[MAXBUFFER];
	int fileSize, fileLocation;
	struct stat fileStat;
    struct group *grp;
	struct passwd *pwd;
	memset(filename1, 0, MAXBUFFER);
	memset(filename2, 0, MAXBUFFER);
	memset(copyFilename, 0, MAXBUFFER);
	memset(parent, 0, MAXBUFFER);
	memset(buildPath, 0, MAXBUFFER);
	strcpy(parent, "root");
	int count = arguments->getCount();
	char *token;
	bool flag = false;
	const char s[2] = "/";
	for(int i = 0; i < count; i++){
		Queue *q = new Queue();
		arguments->peek(i, filename1);
		strcpy(str, filename1);
		while(!dList_->exists(filename1)){
 			strcpy (part1, filename1);
  			char *pos = strrchr(part1, '/');
  			if(pos == NULL){
  				flag = true;
  				break;
  			}
  			else{
  				*pos = '\0';
  				char *part2 = strdup(pos + 1);
  				q->addFront(part2);
  			}
  			strcpy(filename1, part1);
		}
		if(flag || !strcmp(filename1, "\0")){
  			strcpy(parent, "root");
		}
  		else
  		    strcpy(parent, filename1);

		int c = q->getCount();
		if(flag)
			c = 1;
		for(int j = 0; j < c; j++){ //Assume at least a part of the path is non existent
			q->remove(filename2);
			if(strcmp(parent, "root"))
				sprintf(buildPath, "%s/%s", parent, filename2);
			else
				strcpy(buildPath, str);
			newContent(parent, buildPath, compress);
			strcpy(parent, buildPath);
		}
	}
	return 0;
}

void Mydiz::extract(Queue *arguments){
	char *filename = (char *) malloc(MAXBUFFER * sizeof(char));
	int size;
	bool zip;
	int count = arguments->getCount();
	for(int i = 0; i < count; i++){
		arguments->peek(i, filename);
		dList_->extractFile(filename);
	}
	free(filename);
}

				

void Mydiz::exist(Queue *arguments){
	bool found = false;
	char *filename1 = (char *) malloc(MAXBUFFER * sizeof(char));
	int queueCount = arguments->getCount();
	for(int j = 0; j < queueCount; j++){
		arguments->remove(filename1);
		cout << filename1 << ": ";
		if(dList_->exists(filename1))
			cout << "yes" << endl;
		else
			cout << "no" << endl;
	}
	free(filename1);
}

void Mydiz::printMetadata(){
	dList_->printMetaData();
}

void Mydiz::printHierarchy(){
	dList_->printHierarchy();
}


//Helpers

int Mydiz::getDirContents(char *path, FileQueue *fileQ, FileQueue *fileQCopy, bool compress) {
    struct dirent *entry;
    struct stat path_stat;
    struct passwd  *pwd;
	struct group   *grp;
    int fileSize, fileLocation;
	char owner[BUFFER], group[BUFFER], perms[BUFFER], timestamp[BUFFER], *name, temp[BUFFER], zipName[BUFFER], command[BUFFER];
    DIR *dp;
    char *fullPath = (char *) malloc(MAXBUFFER * sizeof(char));

    dp = opendir(path);
    if(dp == NULL) {
        perror("opendir: Path does not exist or could not be read");
        return -1;
    }
    while((entry = readdir(dp)) != NULL){
        sprintf(fullPath, "%s/%s", path, entry->d_name);
        stat(fullPath, &path_stat);
        if(S_ISDIR(path_stat.st_mode)){
        	strcpy(temp, fullPath);
            name = basename(temp);
			//Get stat data
			grp = getgrgid(path_stat.st_gid);
			strcpy(group, grp->gr_name);
			pwd = getpwuid(path_stat.st_uid);
			strcpy(owner, pwd->pw_name);
			getPerms(perms, path_stat);
			strftime(timestamp, BUFFER, "%d - %m - %y", localtime(&(path_stat.st_ctime)));
			if(strcmp(name, ".") && strcmp(name, "..")){
				fileQ->add(true, false, fullPath, owner, group, perms, timestamp, -1, 0);
				fileQCopy->add(true, false, fullPath, owner, group, perms, timestamp, -1, 0);
			}
            if(strcmp(name, ".") && strcmp(name, "..")){
                getDirContents(fullPath, fileQ, fileQCopy, compress);
            }
        }
        else{
        	if(compress){
				sprintf(command, "gzip %s", fullPath);
				system(command);
				sprintf(zipName, "%s.gz", fullPath);
				strcpy(fullPath, zipName);
				stat(fullPath, &path_stat);
			}
			strcpy(temp, fullPath);
			//Get stat data
			grp = getgrgid(path_stat.st_gid);
			strcpy(group, grp->gr_name);
			pwd = getpwuid(path_stat.st_uid);
			strcpy(owner, pwd->pw_name);
			getPerms(perms, path_stat);
			strftime(timestamp, BUFFER, "%d - %m - %y", localtime(&(path_stat.st_ctime)));
			fileSize = path_stat.st_size;
			FILE *fp = fopen(fullPath, "rb");
			char *buffer = (char *) malloc(fileSize * sizeof(char) + 1);
			fread(buffer, fileSize, 1, fp);
			fclose(fp);
			buffer[fileSize] = 0;
			fileLocation = fileSpace_->addFile(fileSize, buffer);
			fileQ->add(false, compress, fullPath, owner, group, perms, timestamp, fileLocation, fileSize);
			fileQCopy->add(false, compress, fullPath, owner, group, perms, timestamp, fileLocation, fileSize);   
        }
    }
    free(fullPath);
    closedir(dp);
    return 0;
}

int Mydiz::getTopContents(char *path, Queue *queue){
	char full[MAXBUFFER];
    struct dirent *entry;
    struct stat path_stat;
    char str[BUFFER], temp[BUFFER], *name;
    DIR *dp = opendir(path);
    if(dp == NULL){
        perror("opendir: Path does not exist or could not be read");
        return -1;
    }
    while((entry = readdir(dp)) != NULL){
    	strcpy(str, entry->d_name);
    	name = basename(str);
    	if(strcmp(name, ".") && strcmp(name, "..")){
    		sprintf(full, "%s/%s", path, entry->d_name);
	        queue->add(full);
    	}
    }
    closedir(dp);
    return 0;
}

void Mydiz::getPerms(char *perms, struct stat fileStat){
	char temp[BUFFER];
	temp[0] = '\0'; 
	sprintf(temp, (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
	strcpy(perms, temp);
    sprintf(temp, (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    strcat(perms, temp);
    sprintf(temp, (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    strcat(perms, temp);
    sprintf(temp, (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    strcat(perms, temp);
    sprintf(temp, (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    strcat(perms, temp);
    sprintf(temp, (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    strcat(perms, temp);
    sprintf(temp, (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    strcat(perms, temp);
    sprintf(temp, (fileStat.st_mode & S_IROTH) ? "r" : "-");
    strcat(perms, temp);
    sprintf(temp, (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    strcat(perms, temp);
    sprintf(temp, (fileStat.st_mode & S_IXOTH) ? "x" : "-");
    strcat(perms, temp);
}

