#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "students.h"
#include "stucs.h"



using namespace std;

Stucs::Stucs(int hashsize){
	hash = new Hashtable(hashsize);
	skip = new Skiplist();
	count = 0;
}

Stucs::~Stucs(){
	delete hash;
	delete skip;
}

bool Stucs::insert(int studid,char* lastname,char* firstname,float gpa,int noc,char* dprt,char* postcode){
	Student* stud = new Student(studid,lastname,firstname,gpa,noc,dprt,postcode);
	if(stud == NULL){ 
		return false;
	}
	if(!skip->insert(stud)){
		delete stud;
		return false;
	} 
	if(!hash->insert(stud)){ //if it wasn't inserted in the hashtable , remove it from the skiplist
		skip->deletest(studid,NULL);
		delete stud;
	 	return false;
	}
	stud->print();
	return true;
}

void Stucs::query(int studid){
	Student *stud = skip->search(studid);
	if(stud==NULL)
		cerr << "not found" << endl;
	else{
		stud->print();
	}
}


bool Stucs::modify(int studid,float gpa,int noc){
	Student* stud = skip->search(studid);
	if(stud==NULL)
		return false;
	else{
		stud->newgpa(gpa); //modify gpa
		stud->newnumofc(noc); //modify number of courses
		stud->print(); //print the record
		return true;
	}
}

Student* Stucs::skipsearch(int id){ //search for a record in the skiplist
	return skip->search(id);
}

Student* Stucs::hashsearch(char* postcode,int studid){ //search for a record in the hashtable
	return hash->search(postcode,studid);
}
	
void Stucs::deletestud(int studid){
	char post[6]; 
	if(skip->deletest(studid,post))
		hash->remove(post,studid);
}

void Stucs::raverage(int studida,int studidb){
	float gpa = skip->raverage(studida,studidb);
	if(gpa < 0) 
		cerr << "not found" << endl;
	else{
		cout << fixed;
		cout << setprecision(2);
		cout << gpa << endl;
	}
}

void Stucs::average(char* post){
	float gpa = hash->average(post);
	if(gpa < 0) 
		cerr << "not found" << endl;
	else{
		cout << fixed;
		cout << setprecision(2);
		cout << gpa << endl;
	}
}

void Stucs::taverage(int k,char* postcode){
	hash->taverage(k,postcode);
}

void Stucs::bottom(int k){
	skip->bottom(k);
}

void Stucs::courtotake(char* postcode,char* deprt){
	hash->courtotake(postcode,deprt);
}

void Stucs::find(float gpa){
	skip->find(gpa);
}

void Stucs::percentile(char* postcode){
	hash->percentile(postcode);
}

void Stucs::percentiles(){
	hash->percentiles();
}
