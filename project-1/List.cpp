#include <iostream>
#include "students.h"
#include "List.h"

using namespace std;

//List node's functions
LNode::LNode(){
	key = 0;
	next = NULL;
	previous = NULL;
	ptr = NULL;
}

//List's functions
List::List(int k){
	length = k;
	front = rear = NULL;
	count = 0;
}


List::~List(){
	LNode* temp = NULL;
	while(front!=NULL){
		temp = front; 
		front = front->next;
		delete temp;
	}
}

void List::add(Student *newstud){
	float gpa = newstud->getgpa();
	LNode *newnode = new LNode();
	newnode->key = gpa;
	newnode->ptr = newstud;
	if(count == 0){
		front = rear = newnode;
	}
	else{
		LNode *temp = front;
		LNode *old = NULL;
		if(gpa <= front->key){ //insert records in a ascending gpa order
			old = front;
			front = newnode;
			front->next = old;
			old->previous = front;
		}
		else if(gpa >= rear->key){
			if(count < length){
				old = rear;
				rear = newnode;
				old->next = rear;
				rear->previous = old;
			}
			else{ // if count is equal to max length, do not insert
				delete newnode;
				return;
			}
		}
		else{
			while(temp!=NULL){
				if(gpa >= temp->key && gpa <= temp->next->key){
					old = temp->next;
					temp->next = newnode;
					newnode->previous = temp;
					newnode->next = old;
					old->previous = newnode;
					break;
				}
			temp = temp->next;
			}
		}
	}
	if(count+1>length){ //if count is larger than max length delete the last record
		LNode* old = rear;
		rear = old->previous;
		rear->next = NULL;
		count--;
		delete old;
	}
	count++;
}

void List::print(){
	LNode* temp = front;
	for(int i=0 ; i<count ; i++){
		temp->ptr->print();
		temp = temp->next;
	}
}
