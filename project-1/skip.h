class Student;

extern int MaxLevel;
extern int MaxValue;

struct Node{
	int id;
	Student *ptr;
	struct Node **forward; //array of pointers pointing to the next node
	int lvl; //number of forward pointers actually pointing to the next node
	Node();
	~Node();
};

class Skiplist{
private:
	Node *header;
	Node *term;
	int count;
public:
	Skiplist();
	~Skiplist();
	Node *makeNode(int,int,Student*);
	int randomLevel();
	bool insert(Student*);
	Student* search(int);
	bool deletest(int,char*);
	float raverage(int,int);
	void bottom(int);
	void find(float);


	//testing purposes
	void print();
};


