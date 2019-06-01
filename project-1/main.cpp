#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <cctype>
#include "students.h"
#include "stucs.h"
#define MAX 200
using namespace std;

bool isnum(char* str){ //checks is a string is numeric
	int len = strlen(str);
	for(int i=0; i<len ; i++){
		if(!isdigit(str[i])){
			return false;
		}
	}
	return true;
}

bool isalph(char *str){ //checks if a string is alphabetic
	int len = strlen(str);
	for(int i=0; i<len ; i++){
		if(isdigit(str[i])){
			return false;
		}
	}
	return true;
}

bool isfloat(char *str){ //checks if a string is a float number
	int len = strlen(str);
	for(int i=0 ; i<len ; i++){
		if(i==0 && !isdigit(str[i]))
			return false;
		else if(i>0 && i<len-1 && !isdigit(str[i]) && str[i]!='.')
			return false;
		else if(i==len-1 && !isdigit(str[i]))
			return false;
	}
	return true;
}

float convert(char* str){ //converts string to float with 2 decimal places
	char buff[MAXSIZE];
	int len = strlen(str);
	for(int i=0 ; i<len ; i++){
		buff[i] = str[i];
		if(str[i]=='.'){
			buff[i+1] = str[i+1];
			buff[i+2] = str[i+2];
			break;
		}
	}
	return strtof(buff,NULL);
}

void operate(char *line,Stucs *stuc,bool &flag,bool file){
	char* tok=NULL;
	char line2[MAX];
	strcpy(line2,line);
	tok = strtok(line," ");
	if(tok == NULL) return;
	if(!strcmp(tok,"i") || !strcmp(tok,"insert")){
		int studid,noc;
		char lastname[MAXSIZE],firstname[MAXSIZE],deprt[MAXSIZE],postcode[6];
		float gpa;
		for(int i=0 ; i<7 ; i++){
			if((tok = strtok(NULL," \n"))==NULL){
				cerr << "wrong number of arguments" << endl;
				return;
			}
			if(i==0)
				studid = atoi(tok);
			else if(i==1){
				if(isalph(tok))
					strcpy(lastname,tok);
				else{
					cerr << "lastname cannot contain numbers" << endl;
					return;
				}
			}
			else if(i==2)
				if(isalph(tok))
					strcpy(firstname,tok);
				else{
					cerr << "firstname cannot contain numbers" << endl;
					return;
				}
			else if(i==3){
				if(isfloat(tok))
					gpa = convert(tok);
				else{
					cerr << "gpa must be a float number" << endl;
					return;
				}
			}
			else if(i==4){
				bool falseinput = false;
				if(isnum(tok))
					noc = atoi(tok);
				else
					falseinput = true;
				if(noc<=0 || noc>=52)
					falseinput = true;
				if(falseinput==true){
					cerr << "number of courses must be an integer between 0 and 52" << endl;
					return;
				}
			}
			else if(i==5){
				if(isalph(tok))
					strcpy(deprt,tok);
				else{
					cerr << "department cannot contain numbers" << endl;
					return;
				}
			}
			else{
				if(isnum(tok) && strlen(tok)==5)
					strcpy(postcode,tok);
				else{
					cerr << "postcode must be a 5 digit number" << endl;
					return;
				}
			}
		}
		stuc->insert(studid,lastname,firstname,gpa,noc,deprt,postcode);
	}
	else if(!strcmp(tok,"q") || !strcmp(tok,"query")){
		int studid;
		if(file == true)
			cerr << line2 << endl;
		if((tok = strtok(NULL," \n"))==NULL){
			cerr << "wrong number of arguments" << endl;
				return;
		}
		studid = atoi(tok);
		stuc->query(studid);
	}
	else if(!strcmp(tok,"m") || !strcmp(tok,"modify")){
		int studid,noc;
		float gpa;
		if(file == true)
			cerr << line2 << endl;
		for(int i=0 ; i<3 ; i++){
			if((tok = strtok(NULL," \n")) == NULL){
				cerr << "wrong number of arguments" << endl;
				return;
			}
			if(i==0)
				studid = atoi(tok);
			else if(i==1){
				if(isfloat(tok))
					gpa = convert(tok);
				else{
					cerr << "gpa must be a float number" << endl;
					return;
				}
			}
			else if(i==2){
				if(isnum(tok))
					noc = atoi(tok);
				else{
					cerr << "number of courses must be an integer" << endl;
					return;
				}
			}
		}
		stuc->modify(studid,gpa,noc);
	}	
	else if(!strcmp(tok,"d") || !strcmp(tok,"delete")){
		int studid;
		if(file == true)
			cerr << line2 << endl;
		if((tok = strtok(NULL," \n"))==NULL){
			cerr << "wrong number of arguments" << endl;
				return;
		}
		studid = atoi(tok);
		stuc->deletestud(studid);
	}
	else if(!strcmp(tok,"ra") || !strcmp(tok,"raverage")){
		int studida,studidb;
		if(file == true)
			cerr << line2 << endl;
		if((tok = strtok(NULL," \n"))==NULL){
			cerr << "wrong number of arguments" << endl;
				return;
		}
		studida = atoi(tok);
		if((tok = strtok(NULL," \n"))==NULL){
			cerr << "wrong number of arguments" << endl;
				return;
		}
		studidb = atoi(tok);
		stuc->raverage(studida,studidb);
	}
	else if(!strcmp(tok,"a") || !strcmp(tok,"average")){
		char post[6];
		if(file == true)
			cerr << line2 << endl;
		if((tok = strtok(NULL," \n"))==NULL){
			cerr << "wrong number of arguments" << endl;
				return;
		}
		if(isnum(tok) && strlen(tok)==5)
			strcpy(post,tok);
		else{
			cerr << "postcode must be a 5-digit number" << endl;
			return;
		}
		stuc->average(post);
	}
	else if(!strcmp(tok,"ta") || !strcmp(tok,"taverage")){
		int k;
		char post[6];
		if(file == true)
			cerr << line2 << endl;
		if((tok = strtok(NULL," \n"))==NULL){
			cerr << "wrong number of arguments" << endl;
				return;
		}
		if(isnum(tok))
			k = atoi(tok);
		else{
			cerr << "k must be an integer" << endl;
			return;
		}
		if((tok = strtok(NULL," \n"))==NULL){
			cerr << "wrong number of arguments" << endl;
				return;
		}
		if(isnum(tok) && strlen(tok)==5)
			strcpy(post,tok);
		else{
			cerr << "postcode must be a 5-digit number" << endl;
			return;
		}
		stuc->taverage(k,post);
	}
	else if(!strcmp(tok,"b") || !strcmp(tok,"bottom")){
		int k;
		if(file == true)
			cerr << line2 << endl;
		if((tok = strtok(NULL," \n"))==NULL){
			cerr << "wrong number of arguments" << endl;
				return;
		}
		if(isnum(tok))
			k = atoi(tok);
		else{
			cerr << "k must be an integer" << endl;
			return;
		}
		stuc->bottom(k);
	}
	else if(!strcmp(tok,"ct") || !strcmp(tok,"courses-to-take")){
		char post[6],dprt[MAXSIZE];
		if(file == true)
			cerr << line2 << endl;
		if((tok = strtok(NULL," \n"))==NULL){
			cerr << "wrong number of arguments" << endl;
				return;
		}
		if(isnum(tok) && strlen(tok)==5)
			strcpy(post,tok);
		else{
			cerr << "postcode must be a 5-digit number" << endl;
			return;
		}
		if((tok = strtok(NULL," \n"))==NULL){
			cerr << "wrong number of arguments" << endl;
				return;
		}
		if(isalph(tok))
			strcpy(dprt,tok);
		else{
			cerr << "department cannot contain numbers" << endl;
			return;
		}
		stuc->courtotake(post,dprt);
	}
	else if(!strcmp(tok,"f") || !strcmp(tok,"find")){
		float gpa;
		if(file == true)
			cerr << line2 << endl;
		if((tok = strtok(NULL," \n"))==NULL){
			cerr << "wrong number of arguments" << endl;
				return;
		}
		if(isfloat(tok))
			gpa = convert(tok);
		else{
			cerr << "gpa must be a float number" << endl;
			return;
		}
		stuc->find(gpa);
	}
	else if(!strcmp(tok,"p") || !strcmp(tok,"percentile")){
		char post[6];
		if(file == true)
			cerr << line2 << endl;
		if((tok = strtok(NULL," \n"))==NULL){
			cerr << "wrong number of arguments" << endl;
				return;
		}
		if(isnum(tok) && strlen(tok)==5)
			strcpy(post,tok);
		else{
			cerr << "postcode must be a 5-digit number" << endl;
			return;
		}
		stuc->percentile(post);
	}
	else if(!strcmp(tok,"pe") || !strcmp(tok,"percentiles")){
		if(file == true)
			cerr << line2 << endl;
		stuc->percentiles();
	}
	else if(!strcmp(tok,"e") || !strcmp(tok,"exit")){
		if(file == true)
			cerr << line2 << endl;
		flag = true;
		delete stuc;
	}
	else
		cerr << "wrong command" << endl;
}


int main(int argc,char **argv){
	if(argc!=3 && argc!=5 && argc!=7){
		cerr << "Wrong number of arguments." << endl;
		return -1;
	}
	ifstream operations;
	int hashsize;
	char line[MAX];
	bool flag = false;
	if(argc==3){
		if(!strcmp(argv[1],"-b"))
			hashsize = atoi(argv[2]);
		else{
			cerr << "Wrong arguments." << endl;
			return -1;
		}
		Stucs *stuc = new Stucs(hashsize);
		while(cin.getline(line,MAX)){
			operate(line,stuc,flag,false);
		}
		if(flag==false) //if stuc wasn't destroyed
			delete stuc;
	}
	else if(argc==5){
		if(!strcmp(argv[1],"-l") && !strcmp(argv[3],"-b")){
			hashsize = atoi(argv[4]);
			operations.open(argv[2],ifstream::in | ios::binary);
		}
		else if(!strcmp(argv[1],"-b") && !strcmp(argv[3],"-l")){
			hashsize = atoi(argv[2]);
			operations.open(argv[4],ifstream::in);
		}
		else{
			cerr << "Wrong arguments." << endl;
			return -1;
		}
		Stucs *stuc = new Stucs(hashsize);
		while(operations.getline(line,MAX)){
			operate(line,stuc,flag,true);
		}
		if(flag==true){//if stuc was destroyed
			stuc = new Stucs(hashsize);
			flag = false;
		}
		while(cin.getline(line,MAX)){
			operate(line,stuc,flag,false);
		}
		if(flag==false)//if stuc wasn't destroyed
			delete stuc;
	}
	else if(argc==7){
		ifstream configfile;
		if(!strcmp(argv[1],"-l")){
			operations.open(argv[2],ifstream::in);
			if(!strcmp(argv[3],"-b") && !strcmp(argv[5],"-c")){
				hashsize = atoi(argv[4]);
				configfile.open(argv[6],ifstream::in);
			}
			else if(!strcmp(argv[3],"-c") && !strcmp(argv[5],"-b")){
				hashsize = atoi(argv[6]);
				configfile.open(argv[4],ifstream::in);
			}
			else{
				cerr << "Wrong arguments." << endl;
				return -1;
			}
		}
		else if(!strcmp(argv[1],"-b")){
			hashsize = atoi(argv[2]);
			if(!strcmp(argv[3],"-l") && !strcmp(argv[5],"-c")){
				operations.open(argv[4],ifstream::in);
				configfile.open(argv[6],ifstream::in);
			}
			else if(!strcmp(argv[3],"-c") && !strcmp(argv[5],"-l")){
				operations.open(argv[6],ifstream::in);
				configfile.open(argv[4],ifstream::in);
			}
			else{
				cerr << "Wrong arguments." << endl;
			}
		}
		else if(!strcmp(argv[1],"-c")){
			configfile.open(argv[2],ifstream::in | ios::binary);
			if(!strcmp(argv[3],"-l") && !strcmp(argv[5],"-b")){
				hashsize = atoi(argv[4]);
				operations.open(argv[6],ifstream::in);
			}
			else if(!strcmp(argv[3],"-b") && !strcmp(argv[5],"-l")){
				hashsize = atoi(argv[6]);
				operations.open(argv[4],ifstream::in);
			}
			else{
				cerr << "Wrong arguments." << endl;
				return -1;
			}
		}
		else{
			cerr << "Wrong arguments." << endl;
			return -1;
		}
		Stucs *stuc = new Stucs(hashsize);
		while(operations.getline(line,MAX)){
			operate(line,stuc,flag,true);
		}
		if(flag==true){//if stuc was destroyed
			stuc = new Stucs(hashsize);
			flag = false;
		}
		while(cin.getline(line,MAX)){
			operate(line,stuc,flag,false);
		}
		if(flag==false)//if stuc wasn't destroyed
			delete stuc;
	}
}