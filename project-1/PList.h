
struct PNode{
	int key; //postcode
	PNode* next;
	PNode();
};

class PList{
private:
	PNode *front;
	PNode *rear;
	int count;
public:
	PList();
	~PList();
	void add(char*);
	void percentiles(int);

	void print();
};