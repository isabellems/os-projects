#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
// #include <editline/history.h>
// #include <editline/editline.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>

#include "functions.h"

#define MAX 200
#define MIN 100

using namespace std;

int main(int argc, char* argv[]) {
	if(argc != 7){
		cerr << "Wrong number of arguments." << endl;
		exit(1);
	}
	FILE *input, *commands, *gnup;
	DIR *tempdir;
	float coord[2];
	bool in, out, workers, temp;
	int numworkers = 0, numhandlers = 0, i = 1, offset = 0;
	int numofdata = 0, numofcommand, validcom = 0;
	char uline[MIN], inputname[MIN], tempname[MIN], path[MIN], *tok, utility[MIN] , line[MAX];
	char args1[MIN], args2[MIN], args3[MIN], args4[MIN], warg[MIN], narg[MIN], farg[MIN] , gnupname[MIN], gnupline[200], rmstring[MAX];
	pid_t pid;
	in = out = workers = temp = false;
	while(i < argc){
		if(!strcmp(argv[i], "-i") && !in){
			input = fopen(argv[i+1], "rb");
			strcpy(inputname, argv[i+1]);
			in = true;
		}
		else if(!strcmp(argv[i], "-w") && !workers){
			numworkers = atoi(argv[i+1]);
			workers = true;
		}
		else if(!strcmp(argv[i], "-d") && !temp){
			tempdir = opendir(argv[i+1]);
			if(!tempdir){
				mkdir(argv[i+1], 0777); 
				chmod(argv[i+1], 0777);
				tempdir = opendir(argv[i+1]);
			}
			closedir(tempdir);
			strcpy(tempname, argv[i+1]);
			temp = true;
		}
		else{
			cerr << "Wrong arguments." << endl;
			exit(1);
		}
		i += 2;
	}
	while(fread(coord, sizeof(float), 2, input))
		numofdata++;
	fclose(input);
	offset = numofdata / numworkers;
	sprintf(path, "%s/commands.txt", tempname);
	numofcommand = 0;
	cout << "> ";
	cin.getline(line, MAX);
	cout << endl;
	while(strcmp(line, "exit")){
		// add_history(line);
		validcom = 0;
		numofcommand++;
		numhandlers = 0;
		commands = fopen(path, "w");
		tok = strtok(line, ",;");
		while(tok != NULL){ //split commands and store them in a specific file in the temp folder
			sprintf(uline, "%s\n", tok);
			whitespaceig(uline);
			fputs(uline, commands); 
			numhandlers++;
			tok = strtok(NULL, ",;\n");
		}
		fclose(commands);
		commands = fopen(path, "r");
		pid_t *pids = new pid_t[numhandlers];
		char **colors = new char*[numhandlers];
		for(int i = 0 ; i < numhandlers ; i++){
			int num;
			colors[i] = new char[MAX];
			fgets(uline, MAX, commands);
			remlastword(uline, colors[i]);
			if(!parseargs(uline, utility, args1, args2, args3, args4, num)){
				cout << "Utility arguments are not valid." << endl;
				pids[i] = -1;
				continue;
			}
			validcom++;
			sprintf(warg, "%d", numworkers);
			sprintf(farg, "%d", offset);
			sprintf(narg, "%d", numofdata);
			pid_t pid = fork();
			if(pid < 0){
				perror("Fork failed.");
				exit(1);
			}
			if(pid == 0){ 
				if(num == 3){ //number of arguments depends on the utility
					if(execl("handler", "./handler", "-i", inputname, "-d", tempname, "-u", utility, args1, args2, args3, "-w", warg, "-f", farg, "-n", narg, NULL) < 0){
						cout << "Execl error." << endl;
						exit(1);
					}
				}
				else{
					if(execl("handler", "./handler", "-i", inputname, "-d", tempname, "-u", utility, args1, args2, args3, args4, "-w", warg, "-f", farg, "-n", narg, NULL) < 0){
						cout << "Execl error." << endl;
						exit(1);
					}
				}
				exit(0);
			}
			else
				pids[i] = pid; //stores handler pid for later use
		}
		for(int i = 0; i < numhandlers; i++){ //waits for all the handlers to exit
			waitpid(-1, NULL, 0);
		}
		if(validcom > 0){
			sprintf(gnupname,"%s/%d_script.gnuplot", tempname, numofcommand); //creates gnuplot script containing all handlers' output files for plotting
			gnup = fopen(gnupname, "w");
			sprintf(gnupline,"set terminal png\nset size ratio -1\nset output \"./%d_image.png\"\nplot \\\n",numofcommand);
			fputs(gnupline, gnup);
			int j = 0;
			char fname[MIN];
			for(int i = 0 ; i < numhandlers ; i++){
				if(pids[i]!=-1){
					sprintf(fname, "%s/%d.out", tempname, pids[i]);
					if(isempty(fname)) //empty files are not included in the script
						continue;
					if(j==0)
						sprintf(gnupline,"\"%s/%d.out\" notitle with points pointsize 0.5 linecolor rgb \"%s\" " , tempname, pids[i], colors[i]);
					else
						sprintf(gnupline,",\\\n\"%s/%d.out\" notitle with points pointsize 0.5 linecolor rgb \"%s\"", tempname, pids[i], colors[i]);
					fputs(gnupline, gnup);
					j++;
				}
			}
			fclose(gnup);
			pid = fork();
			if(pid < 0){
				perror("Fork failed.");
				exit(1);
			}
			else if(pid == 0){ //execute the script
				if(execlp("/usr/bin/X11/gnuplot", "gnuplot", gnupname, NULL) < 0)
					cout << "Execl error." << endl;
				exit(0);
			}
			waitpid(-1, NULL, 0);
		}
		delete[] pids;
		for(int i = 0; i < numhandlers; i++)
			delete[] colors[i];
		delete[] colors;
		fclose(commands);
		waitpid(-1, NULL, 0);
		cout << "> ";
		cin.getline(line, MAX);
		cout << endl;
		// line = readline("> ");
	}
	sprintf(rmstring, "rm -rf %s/*", tempname); //empty temp dir
	pid = fork();
	if(pid < 0){
		perror("Fork failed.");
		exit(1);
	}
	else if(pid == 0){
		if(system(rmstring) < 0){
			cout << "System error." << endl;
		} 
		exit(0);
	}
}