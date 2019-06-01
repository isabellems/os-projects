#ifndef STUDENTS_H
#define STUDENTS_H

struct Bnode{
	Student* stud;
	Bnode* next;
	Bnode* previous;
	Bnode();
	~Bnode();
};

class Bucket{
protected:
	Bnode* front;
	Bnode* rear; 
	int count; //number of records in a bucket
public:
	Bucket();
	~Bucket();
	bool add(Student &);
	bool remove(char *,int);
	Bnode* search(char *,int);
	bool isempty();
	float average(char*);
	void taverage(int,char*);
	void courtotake(char*,char*);
	int studsinpost(char*);
	void percentiles(int count);
	void print();
};

class Hashtable{
protected:
	int size; //hashtable size given by the user
	int count; //number of records in the hashtable
	Bucket** array;
public:
	Hashtable(int);
	~Hashtable();
	bool insert(Student *);
	bool remove(char *,int);
	Student* search(char *,int);
	bool isempty();
	int getsize();
	float average(char *);
	void taverage(int,char*);
	void courtotake(char*,char*);
	void percentile(char*);
	void percentiles();
	void print();
};

#endif