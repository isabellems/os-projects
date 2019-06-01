#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cmath>

#define MAX 100

using namespace std;

bool insquare(float x, float y, float r, float co1, float co2) {
	return fabsf(co1-x) - fabsf(co2-y) <=r; //???
}

int main(int argc, char* argv[]) {
	if(argc!=9 && argc!=11 && argc!=13){
		cout << "Wrong number of arguments." << endl;
		exit(1);
	}
	FILE* input;
	float x, y, r;
	float coord[2];
	int offset = 0 , pointstoread = 0;
	int i = 1, fdout;
	bool in, out, arg, ofs, nump;
	char line[MAX];
	in = out = arg = ofs = nump = false;
	while(i < argc){
		if(!strcmp(argv[i], "-i") && !in){
			input = fopen(argv[i+1], "rb");
			i += 2;
			in = true;
		}
		else if(!strcmp(argv[i], "-o") && !out){
			fdout = open(argv[i+1], O_RDWR | O_NONBLOCK | O_CREAT, S_IRWXU | S_IRWXG);
			if(fdout < 0){
				cout << "Error in opening file." << endl;
				exit(1);
			}
			i += 2;
			out = true;
		}
		else if(!strcmp(argv[i], "-a") && !arg){
			x = atof(argv[i+1]);
			y = atof(argv[i+2]);
			r = atof(argv[i+3]);
			i += 4;
			arg = true;
		}	
		else if(!strcmp(argv[i], "-f") && !ofs){
			offset = atoi(argv[i+1]) * 2*sizeof(float);
			i += 2;
			ofs = true;
		}
		else if(!strcmp(argv[i], "-n") && !nump){
			pointstoread = atoi(argv[i+1]);
			i +=2;
			nump = true;
		}
		else{
			cout << "Wrong arguments." << endl;
			exit(1);
		}
	}
	if(!in || !out || !arg){
		cout << "Wrong arguments." << endl;
		exit(1);
	}
	if(ofs)
		fseek(input, offset, SEEK_SET);
	memset(line, 0, MAX*sizeof(char));
	if(!nump){
		while(fread(coord, sizeof(float), 2, input)){
			if(insquare(x, y, r, coord[0], coord[1])){
				sprintf(line, "%f\t%f\n", coord[0], coord[1]);
				if(write(fdout, line, MAX) <= 0){
					cout << "Write error." << endl;
					break;
				}
			}
		}
	}
	else{
		int pointsread = 0;
		while(pointsread + 1 <= pointstoread){
			if(fread(coord, sizeof(float), 2, input) <= 0)
				break;
			if(insquare(x, y, r, coord[0], coord[1])){
				sprintf(line, "%f\t%f\n", coord[0], coord[1]);
				cout << line << endl;
				if(write(fdout, line, MAX) <= 0){
					perror("Write error.");
					break;
				}
			}
			memset(line, 0, MAX*sizeof(char));
			pointsread++;
		}
	}
	fclose(input);
	close(fdout);
}