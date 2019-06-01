#include <cstring>
#include <ctype.h>
#include <fstream>
#include "functions.h"

#define MAX 200
#define MIN 100

using namespace std;

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

void whitespaceig(char* line){ //ignores whitespaces at the beggining of a string
	char temp[MAX];
	while(line[0] == ' '){
		strcpy(temp, line+1);
		strcpy(line, temp);
	}
}

void remlastword(char* line, char* substr){ //removes last word from a string and returns it to the calling function as an argument
	int len = strlen(line);
	int i = len;
	line[len-1] = 0; //ignore end of line which is in the string due to the use of fgets()
	while(line[i] != ' ' && i!=0)
		i--;
	if(i != 0){
		strcpy(substr,line+i+1); //(in this case substr should be the color)
		line[i] = 0;
	}
}

bool parseargs(char *line, char* util, char *arg1, char *arg2, char *arg3, char *arg4, int &numofargs){ //separate utility arguments, check if they are valid and return also the number of them
	int i = 0;
	char *tok;
	tok = strtok(line, " ");
	strcpy(util, tok);
	while(tok != NULL){
		if(i == 1)
			strcpy(arg1, tok);
		else if(i == 2)
			strcpy(arg2, tok);
		else if( i == 3)
			strcpy(arg3, tok);
		else
			strcpy(arg4, tok);
		i++;
		tok = strtok(NULL, " ");	
	}
	numofargs = i-1;
	if(((numofargs == 3) && (!strcmp(util, "circle") || !strcmp(util, "square"))) || ((numofargs == 4) && (!strcmp(util, "semicircle") || !strcmp(util, "ring") || !strcmp(util, "ellipse")))){
		if(numofargs == 3 && isfloat(arg1) && isfloat(arg2) && isfloat(arg3))
			return true;
		else if(!strcmp(util, "semicircle") && isfloat(arg1) && isfloat(arg2) && isfloat(arg3) && isalph(arg4))
			return true;
		else if(numofargs == 4 && isfloat(arg1) && isfloat(arg2) && isfloat(arg3) && isfloat(arg4))
			return true;
		else
			return false;
	}
	else
		return false;
}

bool isempty(char *fname){
	bool flag = false;
	int length = 0;
	fstream in;
	in.open(fname, ifstream::in);
	in.seekg(0, ios::end); //move cursor at the end of file
	length = in.tellg(); //get position of the cursor
	if(length == 0) 
		flag = true;
	in.close();
	return flag;
}

