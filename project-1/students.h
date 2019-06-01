#define MAXSIZE 60

class Student{
protected:
	int studid;
	char lastname[MAXSIZE];
	char firstname[MAXSIZE];
	float gpa;
	int numofcourses;
	char deprt[MAXSIZE];
	char postcode[6];
public:
	Student(int,char *,char *,float,int,char *,char *);
	~Student();
	int getstudid();
	void getlastname(char *);
	void getfirstname(char *);
	float getgpa();
	int getnumofcourses();
	void getdeprt(char *);
	void getpostcode(char *);
	void newgpa(float);
	void newnumofc(int);
	void print();
};


