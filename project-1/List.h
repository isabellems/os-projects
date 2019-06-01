class Student;

struct LNode{
	float key;
	Student *ptr;
	LNode* next;
	LNode* previous;
	LNode();
};

class List{
private:
	LNode *front;
	LNode *rear;
	int count;
	int length;//max length, given by the user
public:
	List(int);
	~List();
	void add(Student*);
	void print();
};