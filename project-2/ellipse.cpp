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

bool inellipse(float h, float k, float a, float b, float co1, float co2){
	return (co1-h)*(co1-h)/(a*a) + (co2-k)*(co2-k)/(b*b) <= 1;
}

int main(int argc, char* argv[]) {
	if(argc!=10 && argc!=12 && argc!=14){
		cout << "Wrong number of arguments." << endl;
		exit(1); 
	}
	FILE* input;
	float h, k, a, b;
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
				return -1;
			}
			i += 2;
			out = true;
		}
		else if(!strcmp(argv[i], "-a") && !arg){
			h = atof(argv[i+1]);
			k = atof(argv[i+2]);
			a = atof(argv[i+3]);
			b = atof(argv[i+4]);
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
			if(inellipse(h, k, a, b, coord[0], coord[1])){
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
			if(inellipse(h, k, a, b, coord[0], coord[1])){
				sprintf(line, "%f\t%f\n", coord[0], coord[1]);
				if(write(fdout, line, MAX) < 0){
					cout << "Write error." << endl;
					break;
				}
				memset(line, 0, MAX*sizeof(char));
			}
			pointsread++;
		}
	}
	fclose(input);
	close(fdout);
}