#include <iostream>
#include <cstring>
#include <fstream>

#include "Mydiz.h"
#include "Queue.h"

#define MAXPATH 400

using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 3){
		cerr << "Wromg number of arguments." << endl;
		return -1;
	}
	int i = 1;
	char arc[MAXPATH], list[MAXPATH];
	char command = 'n', sec_command = 'n';
	bool file_given = false;
	bool list_given = false;
	Mydiz diz;
	Queue queue;
	while(i < argc) {
		if(!strcmp(argv[i], "-c")){
			command = 'c';
			i++;
		}
		else if(!strcmp(argv[i], "-a")){
			command = 'a';
			i++;
		}
		else if(!strcmp(argv[i], "-x")){
			command = 'x';
			i++;
		}
		else if(!strcmp(argv[i], "-m")){
			command = 'm';
			i++;
		}
		else if(!strcmp(argv[i], "-d")){
			command = 'd';
			i++;
		}
		else if(!strcmp(argv[i], "-q")){
			command = 'q';
			i++;
		}
		else if(!strcmp(argv[i], "-p")){
			command = 'p';
			i++;
		}
		else if(!strcmp(argv[i], "-j")){
			if(command == 'n' || command == 'c' || command == 'a')
				sec_command = 'j';
			else{
				cerr << "Wrong use of -j argument." << endl;
				return -1;
			}
			i++;
		}
		else if(!file_given){
			strcpy(arc, argv[i]);
			if(command == 'c')
				remove(arc);
			file_given = true;
			i++;
		}
		else{	
			list_given = true;
			while(i < argc){
				queue.add(argv[i]);
				i++;
			}
		}
	}
	if(sec_command == 'j' && command != 'a' && command != 'c'){
		cerr << "Wrong use of -j argument." << endl;
		return -1;
	}
	diz.init(arc);
	if(command == 'c'){
		bool zip = false;
		if(sec_command == 'j')
			zip = true;
		diz.createFile(arc, &queue, zip);
	}
	else if(command == 'a'){
		bool zip = false;
		if(sec_command == 'j')
			zip = true;
		diz.append(arc, &queue, zip);
	}
	else if(command == 'x'){
		char root[BUFFER];
		if(list_given == false){
			strcpy(root, "root");
			queue.add(root);
		}
		diz.extract(&queue);
	}
	else if(command == 'm'){
		diz.printMetadata();
	}
	else if(command == 'q'){
		diz.exist(&queue);
	}
	else if(command == 'p'){
		diz.printHierarchy();
	}
}