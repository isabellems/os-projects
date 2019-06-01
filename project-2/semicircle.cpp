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

bool insemicircle(float x, float y, float r, char* semi, float co1, float co2) {
	if((co1-x)*(co1-x) + (co2-y)*(co2-y) <= r*r)
		return (!strcmp(semi, "N") && co2 >= y) || (!strcmp(semi, "S") && co2 <= y) || (!strcmp(semi, "W") && co1 <= x) || (!strcmp(semi, "E") && co1 >= x);
	else
		return false;
}

int main(int argc, char* argv[]) {
	if(argc!=10 && argc!=12 && argc!=14){
		cout << "Wrong number of arguments." << endl;
		exit(1);
	}
	FILE* input;
	float x, y, r;
	char semi[3];
	float coord[2];
	int offset = 0 , pointstoread = 0;
	int i = 1, fdout;
	bool in, out, arg, ofs, nump;
	char line[MAX];
	in = out = arg = ofs = nump = false;
	while(i < argc){
		if(!strcmp(argv[i], "-i") && in==false){
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
			if((!strcmp(argv[i+4], "N")) || (!strcmp(argv[i+4], "S")) || (!strcmp(argv[i+4], "W")) || (!strcmp(argv[i+4], "E")))
				strcpy(semi, argv[i+4]);
			else{
				cout << "Wrong arguments." << endl;
				exit(1);
			}
			i += 5;
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
			if(insemicircle(x, y, r, semi, coord[0], coord[1])){
				sprintf(line, "%f\t%f\n", coord[0], coord[1]);
				if(write(fdout, line, MAX) < 0){
					cout << "Write error." << endl;
					break;
				}
				memset(line, 0, MAX*sizeof(char));
			}
		}
	}
	else{
		int pointsread = 0;
		while(pointsread + 1 <= pointstoread){
			if(fread(coord, sizeof(float), 2, input) <= 0)
				break;
			if(insemicircle(x, y, r, semi, coord[0], coord[1])){
				sprintf(line, "%f\t%f\n", coord[0], coord[1]);
				if(write(fdout, line, MAX) < 0){
					cout << "Write error." << endl;
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