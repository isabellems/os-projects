#include <iostream>
#include <cstdlib>
#include <math.h>
#include <iomanip>
#include "PList.h"


using namespace std;  

//PList node's functions
PNode::PNode(){
	key = 0;
	next = NULL;
}

PList::PList(){
	front = rear = NULL;
	count = 0;
}


PList::~PList(){
	PNode* temp = NULL;
	while(front!=NULL){
		temp = front; 
		front = front->next;
		delete temp;
	}
}

// Plist's functions

void PList::add(char* str){
	int post = atoi(str);
	PNode *newnode = new PNode();
	newnode->key = post;
	if(count == 0){
		front = rear = newnode;
	}
	else{
		PNode *temp = front;
		PNode *old = NULL;
		if(post == front->key){//insert all duplicates one next to the other
			old = front;
			front = newnode;
			front->next = old;
		}
		else if(post == rear->key){
			old = rear;
			rear = newnode;
			old->next = rear;
		}
		else{
			bool flag = false;
			while(temp!=NULL){
				if(post == temp->key){
					old = temp->next;
					temp->next = newnode;
					newnode->next = old;
					flag = true;
					break;
				}
			temp = temp->next;
			}
			if(flag==false){
				old = rear;
				rear = newnode;
				old->next = rear;
			}
		}
	}
	count++;
}

void PList::percentiles(int hashcount){
	PNode* temp = front;
	int studs = 0;
	float perc = 0;
	for(int i=0 ; i<count ; i++){
		studs++;
		if(temp->next==NULL || temp->key!=temp->next->key){
			perc = ceilf(studs*100/hashcount)/100; //calculate percentile and round it to a float number with two decimal places
			cout << fixed;
			cout << setprecision(2);
			cout << temp->key << " " << perc << endl;
			studs = 0;
		}
		temp = temp->next;
	}
}
