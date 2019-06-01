#include <iostream>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include "students.h"

using namespace std;

Student::Student(int studid_,char *lastname_,char *firstname_,float gpa_,int numofcourses_,char *deprt_,char *postcode_){
	studid = studid_;
	strcpy(lastname,lastname_);
	strcpy(firstname,firstname_);
	gpa = gpa_;
	numofcourses = numofcourses_;
	strcpy(deprt,deprt_);
	strcpy(postcode,postcode_);
}

Student::~Student(){}

int Student::getstudid(){
	return studid;
}

void Student::getlastname(char *last){
	strcpy(last,lastname);
}

void Student::getfirstname(char *first){
	strcpy(first,firstname);
}

float Student::getgpa(){
	return gpa;
}

int Student::getnumofcourses(){
	return numofcourses;
}

void Student::getdeprt(char *dep){
	strcpy(dep,deprt);
}

void Student::getpostcode(char *post){
	strcpy(post,postcode);
}

void Student::newgpa(float ngpa){
	gpa = ngpa;
}

void Student::newnumofc(int noc){
	numofcourses = noc;
}

void Student::print(){ //print the record
	cout << fixed;
	cout << setprecision(2);
	cout << studid << " " << lastname << " " << firstname << " " << gpa << " " << numofcourses << " " << deprt << " " << postcode << endl; 
}