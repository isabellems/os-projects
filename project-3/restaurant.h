#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/wait.h>

#define P(A) sem_wait(A)
#define V(A) sem_post(A)
#define Pt(A,B) sem_timedwait(A,B)

struct BarSeat{
	pid_t customer;
	bool available;
	int people;
	int table_given;
	int priority;
	sem_t seatSem;
};

struct CMessage{
	pid_t id;
	int answer;
	int table;
	int capacity;
	sem_t ans_sem;
};

struct Table{
	int number;
	bool available;
	int capacity;
	pid_t customer;
	int waiter;
};

struct sharedData {
	sem_t output;
	sem_t queue;
	sem_t mutex;
	sem_t doorm;
	sem_t stats;
	sem_t answer;//remove
	sem_t bar_mutex;
	sem_t tables_mutex;
	sem_t waiters_mutex;
	sem_t an_mutex;
	sem_t waiter_answer;

	int tables;
	int waiters;
	int barCapacity;
	int moneySum;
	int peopleLeft;
	int peopleTables;
	int peopleBar;
	int firstGroupCap;
	int cust_left;
	int customers;
	int barLeft;
	int peoplePaid;
	int tablesAvail[4];
	int tablesCap[4];
	int door_answer;
	int barLastSeat;
};