#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <sys/wait.h>

#define MAX 500
#define MIN 100

using namespace std;

int main(int argc, char* argv[]){
	if(argc != 16 && argc != 17){
		cout << "Wrong number of arguments" << endl;
		exit(1);
	}
	FILE* input, *temporary;
	DIR* tempdir;
	char inputname[MIN], tempname[MIN], utility[MIN], path[MIN], filename[MIN];
	char arg1[MIN], arg2[MIN], arg3[MIN], arg4[MIN], ofarg[MIN], parg[MIN], exearg[MIN];
	int workers, i, expoints, utargs, offset, start, pointstoread, points = 0;
	bool in, temp, util, work, nump, of;
	pid_t hpid;
	in = temp = util = work = nump = of = false;
	i = 1;
	while(i < argc){
		if(!strcmp(argv[i], "-i") && !in){
			input = fopen(argv[i+1], "rb");
			strcpy(inputname, argv[i+1]);
			in = true;
			i += 2;
		}
		else if(!strcmp(argv[i], "-d") && !temp){
			tempdir = opendir(argv[i+1]);
			if(!tempdir){
				mkdir(argv[i+1], 0777); //todo:modes
				chmod(argv[i+1], 0777);
				tempdir = opendir(argv[i+1]);
				closedir(tempdir);
			}
			strcpy(tempname, argv[i+1]);
			temp = true;
			i += 2;
		}
		else if(!strcmp(argv[i], "-u") && !util){
			strcpy(utility, argv[i+1]);
			strcpy(arg1, argv[i+2]);
			strcpy(arg2, argv[i+3]);
			strcpy(arg3, argv[i+4]);
			util = true;
			if(argc == 17){
				strcpy(arg4, argv[i+5]);
				utargs = 4;
				i += 6;
			}
			else{
				utargs = 3;
				i += 5;
			}
		}
		else if(!strcmp(argv[i], "-w") && !work){
			workers = atoi(argv[i+1]);
			work = true;
			i += 2;
		}
		else if(!strcmp(argv[i], "-f") && !of){
			offset = atoi(argv[i+1]);
			of = true;
			i += 2;
		}
		else if(!strcmp(argv[i], "-n") && !nump){
			pointstoread = atoi(argv[i+1]);
			nump = true;
			i += 2;
		}
		else{
			cout << "Wrong arguments." << endl;
			exit(1);
		}
	}
	hpid = getpid();
	sprintf(filename, "%s/%d.out", tempname, hpid);
	temporary = fopen(filename, "w");
	expoints = pointstoread % workers;
	pid_t *wpids = new pid_t[workers];
	int *fdp = new pid_t[workers];
	for(int i = 0 ; i < workers ; i++){
		sprintf(path, "%s/%d_w%d.fifo", tempname, hpid, i);
		if(mkfifo(path, 0777) < 0){
			perror("Fifo could not be made.");
			exit(1);
		}
		if(i == 0)
			start = 0;
		else
			start = start + (points);
		if(expoints != 0){
			expoints--;
			points = offset + 1;
		}
		else
			points = offset;
		sprintf(parg, "%d", points);
		sprintf(ofarg, "%d", start);
		fdp[i] = open(path, O_RDWR | O_NONBLOCK);
		pid_t pid = fork();
		if(pid < 0){
			perror("Failed to fork.");
			exit(1);
		}
		else if(pid == 0){
			if(utargs == 3){
				sprintf(exearg, "./%s", utility);
				if(execl(utility, exearg, "-i", inputname, "-o", path, "-a", arg1 , arg2, arg3, "-f", ofarg , "-n" , parg, NULL) < 0){
					cout << "Exec error," << endl;
				}
			}
			else{
				if(execl(utility, exearg, "-i", inputname, "-o", path, "-a", arg1 , arg2, arg3, arg4, "-f", ofarg , "-n" , parg, NULL) < 0){
					cout << "Exec error." << endl;
				}
			}
			exit(0);
		}
		else
			wpids[i] = pid;
	}
	pid_t pid;
	int wnum = 0;
	char rline[MIN];
	memset(rline, 0, MIN*sizeof(char));
	for(int i = 0 ; i < workers ; i++){ //wait for each worker to exit and read its pipe's contents writing them in the .out file
		pid = waitpid(-1, NULL, 0);
		for(int j = 0 ; j < workers ; j++){
			if(wpids[j] == pid)
				wnum = j;
		}
		while(read(fdp[wnum], rline, MIN) >= 0){
			fputs(rline, temporary);
			memset(rline, 0, MIN*sizeof(char));

		}
		close(fdp[wnum]);
	}
	delete[] wpids;
	delete[] fdp;
	fclose(temporary);
}