#include <iostream>
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <iomanip>
#include "PList.h"
#include "students.h"
#include "hash.h"


using namespace std;
 
//Bucket node functions
Bnode::Bnode(){
	stud = NULL;
	next = NULL;
	previous = NULL;
}

Bnode::~Bnode(){
	delete stud;
}


//Bucket functions
Bucket::Bucket(){
	front = rear = NULL;
	count = 0;
}

Bucket::~Bucket(){
	Bnode *temp = NULL;
	while(front!=NULL){
		temp = front;
		front = front->next;
		delete temp;
	}
}

bool Bucket::add(Student &stud_){
	Bnode *add = new Bnode();
	add->stud = &stud_;
	if(front == NULL)
		front = add;
	else{
		rear->next = add;
		add->previous = rear;
	}
	rear = add;
	count ++;
	return true;
}

bool Bucket::remove(char *post,int studid){
	Bnode *temp = search(post,studid);
	if(temp == NULL) return false;
	if(temp->previous == NULL && temp->next ==NULL){
		front=rear=NULL;
	}
	else if(temp->previous != NULL && temp->next!=NULL){
		temp->previous->next = temp->next;
		temp->next->previous = temp->previous;
	}
	else if(temp->previous == NULL){
		front = front->next;
		front->previous = NULL;
	}
	else{
		rear = temp->previous;
		rear->next = NULL;
	}
	count--;
	delete temp;
	return true;
}

Bnode* Bucket::search(char *post,int studid){ //search for a student with a specific postcode and id
	char postc[6];
	if(this->isempty()) return NULL;
	Bnode* temp = front;
	for(int i=0 ; i < count ; i++){
		temp->stud->getpostcode(postc);
		if(!strcmp(post,postc) && temp->stud->getstudid() == studid){
			return temp;
		}
		temp = temp->next;
	}
	return NULL;
}

bool Bucket::isempty(){
	return count==0;
	if(this->isempty()){ return true;}
}

float Bucket::average(char* post){
	char postc[6];
	float totalgpa,gpa,av;
	totalgpa = gpa = av = 0;
	int numofstud = 0;
	if(this->isempty()) return -1;
	Bnode* temp = front;
	for(int i=0; i<count ; i++){
		temp->stud->getpostcode(postc);
		if(!strcmp(post,postc)){
			numofstud ++;
			gpa = temp->stud->getgpa();
			totalgpa += gpa;
		}
		temp = temp->next;
	}
	av = totalgpa/numofstud;
	float a = ceilf(av * 100) / 100; //round number to a float with two decimal places
	return a;
}

void Bucket::taverage(int k,char* post){
	char postc[6];
	float totalgpa,gpa;
	totalgpa = gpa = 0;
	int numofstud = 0;
	if(this->isempty()) return ;
	Bnode* temp = front;
	Student* starray[count];
	for(int i=0; i<count; i++){ //store all the records with the specific postcode in an array
		temp->stud->getpostcode(postc);
		if(!strcmp(post,postc)){
			starray[numofstud] = temp->stud;
			numofstud++;
		}
		temp = temp->next;
	}
	if(numofstud==0){
		 cerr << "not found" << endl;
		 return;
	}
	Student *tempstud = NULL;
	for(int i=0 ; i<numofstud ; i++){ //sort the array in a descending gpa order
		for(int j=0 ; j<numofstud-1 ; j++){
			if(starray[j]->getgpa()<starray[j+1]->getgpa()){
				tempstud = starray[j];
				starray[j] = starray[j+1];
				starray[j+1] = tempstud;
			}
		}
	}
	if(numofstud<k) //if records are less than k
		k=numofstud;
	for(int i=0 ; i<k ; i++)
		starray[i]->print();
}

void Bucket::courtotake(char* post,char* dprt){
	if(this->isempty()){
		cerr << "not found" << endl;
		return;
	}
	char postcode[6],depart[MAXSIZE];
	int totalnoc,noc;
	totalnoc = noc = 0;
	Bnode* temp = front;
	bool found = false;
	for(int i=0 ; i<count ; i++){
		temp->stud->getpostcode(postcode);
		if(!strcmp(postcode,post)){
			temp->stud->getdeprt(depart);
			if(!strcmp(depart,dprt)){
				found = true;
				noc = temp->stud->getnumofcourses();
				totalnoc += noc;
				temp->stud->print();
			}
		}
		temp = temp->next;
	}
	if(found==true)
		cout << totalnoc << endl;
	else
		cerr << "not found" << endl;
}

int Bucket::studsinpost(char* post){ // return the number of students with a specific postcode
	if(this->isempty()){
		cerr << "not found" << endl;
		return 0;
	}
	char postcode[6];
	int studs = 0;
	Bnode *temp = front;
	for(int i=0; i<count ; i++){
		temp->stud->getpostcode(postcode);
		if(!strcmp(postcode,post))
			studs++;
		temp = temp->next;
	}
	return studs;
}

void Bucket::print(){
	if(this->isempty()) return;
	Bnode* temp = front;
	for(int i=0 ; i<count ; i++){
		temp->stud->print();
		temp = temp->next;
	}
}

void Bucket::percentiles(int hashcount){
	char postcode[6];
	PList *list = new PList(); //List that keeps same postcodes together
	Bnode* temp = front;
	for(int i=0 ; i<count ; i++){ //insert bucket's postcodes to this list
		temp->stud->getpostcode(postcode);
		list->add(postcode);
		temp = temp->next;
	}
	list->percentiles(hashcount);
	delete list;
}

//Hashtable functions

Hashtable::Hashtable(int size_){
	size = size_;
	count = 0;
	array = new Bucket*[size];
	for(int i=0; i<size_ ; i++){
		array[i] = new Bucket();
	}
}

Hashtable::~Hashtable(){
	for(int i=0 ; i<size ; i++){
		delete array[i]; 
	}
	delete[] array;
}

bool Hashtable::insert(Student *stud){
	char post[6];
	stud->getpostcode(post); 
	long postcode = strtol(post,NULL,10);
	int bucket = postcode%size; //applies hash function
	if(array[bucket]->add(*stud)){ // and inserts the record to the proper bucket
		count++;
		return true;
	}
	else{
		return false;
	}
}

bool Hashtable::remove(char *post,int studid){
	long postcode = strtol(post,NULL,10);
	int bucket = postcode%size; // applies hash function 
	if(array[bucket]->remove(post,studid)){ //and removes the record from the proper bucket
		count--;
		return true;
	}
	else 
		return false;
}

Student* Hashtable::search(char *post,int studid){
	Bnode* node = NULL;
	long postc = strtol(post,NULL,10);
	int bucket = postc%size; //applies hash function
	node = array[bucket]->search(post,studid); //and searches for the record in the proper bucket
	if(node!=NULL)
		return node->stud;
	else
		return NULL;
}

bool Hashtable::isempty(){
	for(int i=0; i<size ; i++){
		if(array[i]->isempty()==false)
			return false;
	}
	return true;
}

int Hashtable::getsize(){
	return size;
}

float Hashtable::average(char *post){
	long postc = strtol(post,NULL,10);
	int bucket = postc%size;
	return array[bucket]->average(post);
}

void Hashtable::taverage(int k,char* post){
	long postc = strtol(post,NULL,10);
	int bucket = postc%size;
	array[bucket]->taverage(k,post);
}

void Hashtable::courtotake(char* post,char* dprt){
	long postc = strtol(post,NULL,10);
	int bucket = postc%size;
	array[bucket]->courtotake(post,dprt);
}

void Hashtable::percentile(char* post){
	long postc = strtol(post,NULL,10);
	int bucket = postc%size;
	int numofstuds = array[bucket]->studsinpost(post); //gets the number of students with the specific postcode
	float percentile = ceilf(numofstuds*100/count)/100; //calculates the percentile rounded to a float with 2 decimal places
	cout << fixed;
	cout << setprecision(2);
	cout << percentile << endl;
}

void Hashtable::percentiles(){
	for(int i=0 ; i<size ; i++)
		array[i]->percentiles(count); //call percentiles function for every bucket
}

void Hashtable::print(){
	for(int i=0 ; i<size ; i++){
		array[i]->print();
	}
}
