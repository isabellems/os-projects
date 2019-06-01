#include <cstdlib>
#include <iostream>
#include <ctime>
#include <math.h>
#include "students.h"
#include "List.h"
#include "skip.h"

using namespace std;
int MaxLevel = 5; 
int MaxValue = 10000000;

//Skiplist node's functions
Node::Node(){
	ptr = NULL;
	forward = new Node*[MaxLevel];
	for(int i=0 ; i<MaxLevel ; i++)
		forward[i] = NULL;
}

Node::~Node(){
	delete[] forward;
}

//Skiplist's functions
Skiplist::Skiplist(){
	term = new Node(); 
	term->id = MaxValue;
	header = new Node();
	for(int i=0 ; i<MaxLevel ; i++){
		header->forward[i] = term;
	}
	count = 0;
}

Skiplist::~Skiplist(){
	Node *temp = header;
	while(header!=NULL){ 
			temp = header;
			header = header->forward[0];
			delete temp;
	}
}

Node *Skiplist::makeNode(int lvl,int searchKey,Student* value){
	Node* newnode = new Node(); //create new node
	newnode->lvl = lvl;
	newnode->id = searchKey;
	newnode->ptr = value;
	for(int i=0 ; i<lvl ; i++) //make lvl (out of MaxLevel) pointers point to the terminating node
		newnode->forward[i] = term;
	return newnode;
}

int Skiplist::randomLevel(){ 
	srand(time(NULL)); //calculate a random level
	return (rand() % MaxLevel) + 1; //from 1 to MaxLevel
}

bool Skiplist::insert(Student* newstud){
	Node* update[MaxLevel];
	Node* temp = header;
	for(int i=0 ; i<MaxLevel-1 ; i++)
		update[i] = NULL;
	int searchid = newstud->getstudid();
	for(int i=MaxLevel-1 ; i>=0 ; i--){
		while(temp->forward!=NULL && temp->forward[i]->id < searchid){
			temp = temp->forward[i];
		}
		update[i] = temp;
	}
	temp = temp->forward[0];
	if(temp->id == searchid){ //if student already exists keep the old one
		cerr << "student with such id already exists" << endl;
		return false;
	}
	else{
		int lvl = randomLevel();
		temp = makeNode(lvl,searchid,newstud);
		for(int i=0; i<lvl ; i++){
			temp->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = temp;
		}
	}
	count++;
	return true;
}

Student* Skiplist::search(int searchid){
	if(searchid>=MaxValue) return NULL;
	Node* temp = header;
	for(int i=MaxLevel-1; i>=0 ; i--){
		if(temp->forward[i]!=NULL){
			while(temp->forward[i]->id < searchid)
				temp = temp->forward[i];
		}
	}
	temp = temp->forward[0];
	if(temp->id == searchid)
		return temp->ptr;
	else
		return NULL;
}


bool Skiplist::deletest(int searchid,char* post){
	Node* update[MaxLevel];
	Node* temp = header;
	for(int i=MaxLevel-1; i>=0 ; i--){
		if(temp->forward[i]!=NULL){
			while(temp->forward[i]->id < searchid)
				temp = temp->forward[i];
		}
		update[i] = temp;
	}
	temp = temp->forward[0];
	if(temp->id == searchid){
		for(int i=0; i<MaxLevel ; i++){
			if(update[i]->forward[i]!=temp)
				break;
			update[i]->forward[i] = temp->forward[i];
		}
		temp->ptr->getpostcode(post); //return postcode as a function argument for removing record from the hashtable later
		temp->ptr->print();
		delete temp;
		count--;
		return true;
	}
	else
		return false;
}	

float Skiplist::raverage(int studida,int studidb){
	if(studida>=MaxValue || studidb>=MaxValue) 
		return -1;
	Node* temp = header;
	float totalgpa,gpa,ra;
	totalgpa = ra = 0;
	int studs = 0 ;
	for(int i=MaxLevel-1 ; i>=0 ; i--){
			while(temp->forward[i]!=NULL && temp->forward[i]->id <= studidb){
				temp = temp->forward[i];
				if(temp->id >= studida){
					studs++;
					gpa = temp->ptr->getgpa();
					totalgpa += gpa;
				}
			}
	}
	ra = totalgpa/studs;
	float r = ceilf(ra * 100) / 100;
	return r;
}

void Skiplist::bottom(int k){
	Node* temp = header;
	List *list = new List(k); //keep a sorted list in an ascending gpa order with max length k
	for(int i=0 ; i<count+1 ; i++){ //all nodes including header, except from the last (terminating node)
		if(temp->ptr!=NULL)
			list->add(temp->ptr); 
		temp = temp->forward[0];
	}
	list->print();
	delete list;
}

void Skiplist::find(float gpa){
	Node* temp = header;
	int highnoc,noc;
	highnoc = noc = 0;
	for(int i=0 ; i<count+1 ; i++){
		if(temp->ptr!=NULL){ //find the highest number of courses
			if((noc = temp->ptr->getnumofcourses()) > highnoc)
				highnoc = noc;
		}
		temp = temp->forward[0];
	}
	temp = header;
	for(int i=0 ; i<count+1 ; i++){
		if(temp->ptr!=NULL){
			if(temp->ptr->getnumofcourses() == highnoc && temp->ptr->getgpa() > gpa)
				temp->ptr->print();
		}
		temp = temp->forward[0];
	}
}




//testing purposes
void Skiplist::print(){
	Node* temp = header;
	while(temp!=NULL){
		cout << temp->id << " ";
		temp = temp->forward[0];
	}
	cout << endl;
}
