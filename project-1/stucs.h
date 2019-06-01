#include "skip.h"
#include "hash.h"

class Stucs{
private:
	Hashtable* hash;
	Skiplist* skip;
	int count;
public:
	Stucs(int);
	~Stucs();
	bool insert(int,char*,char*,float,int,char*,char*);
	void query(int);
	bool modify(int,float,int);
	void deletestud(int);
	void raverage(int,int);
	void average(char*);
	void taverage(int,char*);
	void bottom(int);
	void courtotake(char*,char*);
	void find(float);
	void percentile(char*);
	void percentiles();
	void exit();

	//testing purposes
	Student* skipsearch(int);
	Student* hashsearch(char*,int); 
};
