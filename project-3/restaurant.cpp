#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctime>

#include "restaurant.h"

#define MAX 100

using namespace std;

int main(int argc, char* argv[]) {
	if(argc!=7){
		cerr << "Restaurant: Wrong Number of arguments." << endl;
		return -1;
	}
	int i = 1, customers, period, waiters, barCapacity, tables, maxwTime, maxdTime, maxcTime, wmoney;
	sharedData *mem;
	CMessage *cust;
	BarSeat *seat;
	int pos, *waitersCust;
	Table *table;
	char *tables_p, *sharedMem, *waiters_q, *waiters_sem, *cust_answer, *waitersStatus, *bar, *answer;
	int *tcap;
	sem_t *waiter1 , *waiter2, *barqueue;
	char flag[5];
	ifstream config;
	ofstream logging;
	while(i < argc){
		if(!strcmp(argv[i], "-n"))
			customers = atoi(argv[i+1]);
		else if(!strcmp(argv[i], "-I")){
			config.open(argv[i+1], ifstream::in);
			if(!config.is_open()){
				cerr << "Restaurant: cannot open file." << endl;
				return -1;
			}
		}
		else if(!strcmp(argv[i], "-d"))
			period = atoi(argv[i+1]);
		else{
			cerr << "Restaurant: Unknown argument " << argv[i+1] << endl;
			return -1;
		}
		i+=2;
	}  
	logging.open("logging.txt", ofstream::out);
	if(!logging.is_open()){
		cerr << "Restaurant: cannot open logging file." << endl;
		return -1;
	}
	while(config >> flag){
		if(!strcmp(flag, "-w"))
			config >> waiters;
		else if(!strcmp(flag, "-b"))
			config >> barCapacity;
		else if(!strcmp(flag, "-t")){
			config >> tables;
			tcap = new int[tables];
		}
		else if(!strcmp(flag, "-c")){
			for(int i=0 ; i < tables; i++)
				config >> tcap[i];
		}
		else if(!strcmp(flag, "-wt"))
			config >> maxwTime;
		else if(!strcmp(flag, "-dt"))
			config >> maxdTime;
		else if(!strcmp(flag, "-ct"))
			config >> maxcTime;
		else if(!strcmp(flag, "-wm"))
			config >> wmoney;
		else{
			cerr << "Restaurant: wrong configuration file argument." << endl;
			return -1;
		} 
	}
	logging << "Restaurant created. Process id: " << getpid() << endl;
	int shmsize = sizeof(sharedData);
	int id1 = shmget(IPC_PRIVATE, shmsize, 0666);
	logging << "Restaurant allocated shared memory segment with id " << id1 << endl;
	if(id1 < 0){
		cerr << "Restaurant: Cannot create shared memory segment." << endl;
		perror("Error: ");
		return -1;
	}
	sharedMem = (char*) shmat(id1, (char*)0, 0);
	shmsize = sizeof(int) * waiters + sizeof(CMessage) * waiters + sizeof(sem_t) * waiters + sizeof(sem_t) * waiters;
	logging << "Restaurant attached to shared memory segment with id " << id1 << endl;
	int id2 = shmget(IPC_PRIVATE, shmsize, 0666);
	logging << "Restaurant allocated shared memory segment with id " << id2 << endl;
	if(id2 < 0){
		cerr << "Restaurant: Cannot create shared memory segment." << endl;
		perror("Error: ");
		return -1;
	}
	waitersStatus = (char*) shmat(id2, (char*)0, 0);
	shmsize = sizeof(Table) * tables + sizeof(BarSeat) * barCapacity;
	int id3 = shmget(IPC_PRIVATE, shmsize, 0666);
	logging << "Restaurant allocated shared memory segment with id " << id3 << endl;
	if(id3 < 0){
		cerr << "Restaurant: Cannot create shared memory segment." << endl << endl;
		perror("Error: ");
		return -1;
	}
	tables_p = (char*) shmat(id3, (char*)0, 0);
	mem = (sharedData*) sharedMem;
	mem->waiters = waiters;
	mem->barCapacity = barCapacity;
	mem->tables = tables;
	mem->moneySum = 0;
	mem->firstGroupCap = 0;
	mem->customers = customers;
	mem->cust_left = 0;
	mem->barLastSeat = barCapacity;
	mem->peopleLeft = 0;
	mem->peopleTables = 0;
	mem->peopleBar = 0;
	mem->barLeft = 0;
	mem->peoplePaid = 0;
	for(int i = 0 ; i < 4 ; i++){
		mem->tablesAvail[i] = 0;
		mem->tablesCap[i] = 0;
	}
	mem->door_answer = 0;
	cout << "Waiters: " << mem->waiters << endl;
	cout << "Tables: " << mem->tables << endl;
	cout << "Bar: " << mem->barCapacity << endl;
	cout << "Waiter waiting period: " << maxwTime << endl;
	cout << "Doorman waiting period: " << maxdTime << endl;
	cout << "Customer waiting period: " << maxcTime << endl;
	cust_answer = (char*) (waitersStatus + sizeof(int) * mem->waiters);
	waiters_sem = (char*) (cust_answer + sizeof(CMessage) * mem->waiters);
	waiters_q = (char*) (waiters_sem + sizeof(sem_t) * mem->waiters);
	waitersCust = (int*) (waitersStatus);
	bar = (char*) (tables_p + sizeof(Table) * mem->tables);
	for(int i = 0 ; i < mem->barCapacity ; i++){
		seat = (BarSeat*) (bar + sizeof(BarSeat) * i);
		seat->customer = 0;
		seat->available = true;
		seat->people = 0;
		seat->table_given = -1;
		seat->priority = 0;
		sem_init(&(seat->seatSem), 1, 0);
	}
	for(int i = 0 ; i < tables; i++){
		table = (Table*) (tables_p + i * sizeof(Table));
		table->number = i+1;
		table->available = true;
		table->capacity = tcap[i];
		table->customer = 0;
		// table->waiter = 0;
		pos = tcap[i]/2 - 1;
		mem->tablesAvail[pos]++;
		mem->tablesCap[pos]++;
	}
	for(int i = 2 ; i >= 0 ; i--)
		mem->tablesAvail[i] = mem->tablesAvail[i] + mem->tablesAvail[i+1];
	for(int i = 0 ; i < mem->waiters ; i++){
		cust = (CMessage*) (cust_answer + sizeof(CMessage) * i);
		waitersCust[i] = 0;
		waiter1 = (sem_t*) (waiters_sem + i * sizeof(sem_t));
		waiter2 = (sem_t*) (waiters_q + + i * sizeof(sem_t));
		sem_init(waiter1 , 1, 0);
		sem_init(waiter2 , 1, 0);
		sem_init(&(cust->ans_sem), 1, 0);
	}
	sem_init(&(mem->queue), 1, 0);
	sem_init(&(mem->output), 1, 1);
	sem_init(&(mem->doorm), 1, 0);
	sem_init(&(mem->mutex), 1, 1);
	sem_init(&(mem->bar_mutex), 1, 1);
	sem_init(&(mem->tables_mutex), 1, 1);
	sem_init(&(mem->waiters_mutex), 1, 1);
	sem_init(&(mem->answer), 1, 0);
	sem_init(&(mem->stats) , 1, 1);
	sem_init(&(mem->an_mutex), 1, 1);
	sem_init(&(mem->waiter_answer), 1, 0);
	logging << "Restaurant initialised shared memory segment members." << endl;

	char timearg[MAX], shmidarg1[MAX], shmidarg2[MAX], shmidarg3[MAX];
	sprintf(timearg, "%d", maxdTime);
	sprintf(shmidarg1, "%d", id1);
	sprintf(shmidarg2, "%d", id2);
	sprintf(shmidarg3, "%d", id3);

	pid_t pid = fork();
	if(pid < 0){
		cerr << "Restaurant: fork error." << endl;
		return -1; 
	}
	else if (pid == 0){
		if(execl("doorman", "./doorman", "-d" , timearg, "-s", shmidarg1, shmidarg2, shmidarg3, NULL)){
			cerr << "Restaurant: execl error." << endl;
			return -1;
		}
	}


	char moneyamount[MAX], waiter_no[MAX];
	// int wtime, wmoney;
	sprintf(timearg, "%d", maxwTime);
	sprintf(moneyamount, "%d", wmoney);

	for(int i = 0 ; i < mem->waiters ; i++) {
		sprintf(waiter_no, "%d", i);
		pid = fork();
		if(pid < 0){
			cerr << "Restaurant: fork error." << endl;
			return -1;
		}
		else if(pid == 0){
			if(execl("waiter", "./waiter", "-d", timearg, "-m", moneyamount, "-s", shmidarg1, shmidarg2, shmidarg3, "-n", waiter_no, NULL)){
				cerr << "Restaurant: execl error." << endl;
				return -1;
			}
		}
	}

	usleep(100000);

	int people;
	char peoplearg[MAX];
	sprintf(timearg, "%d", maxcTime);
	srand(time(NULL));
	for(int i = 0 ; i < customers ; i++){
		people = rand() % 8 + 1;
		sprintf(peoplearg, "%d", people);
		pid = fork();
		if(pid < 0){
			cerr << "Restaurant fork error." << endl;
			return -1;
		}
		else if(pid == 0){
			if(execl("customer", "./customer", "-n", peoplearg, "-d", timearg, "-s", shmidarg1, shmidarg2, shmidarg3, NULL)){
				cerr << "Restaurant: execl error." << endl;
				return -1;
			}
		}
	}

	int child_proc = 1 + customers + waiters;
	while(1){
		sleep(period);
		P(&(mem->stats));
		cout << endl << "~Statistics~" << endl << endl;;
		cout << "Number of people in tables: " << mem->peopleTables << endl;
		cout << "Number of people in bar: " << mem->peopleBar << endl;
		cout << "Number of groups that left the entrance: " << mem->peopleLeft << endl;
		cout << "Number of groups that left the bar: " << mem->barLeft << endl;
		cout << "Number of groups that were served: " << mem->peoplePaid << endl;
		cout << "Total amount of money: " << mem->moneySum << endl;
		cout << "Total amount of groups who left: " << mem->cust_left << endl << endl;;
		if(mem->cust_left == mem->customers){
			V(&(mem->stats));
			break;
		}
		V(&(mem->stats));
	}

	for(int i = 0 ; i < child_proc ; i++){
		waitpid(-1, NULL, 0);
	}

	cout << "Restaurant closed." << endl;
	logging << "Restaurant closed." << endl;
	int err = shmctl(id1, IPC_RMID, 0); 
	if (err == -1) 
		cout << "Restaurant: cannot remove segment." << endl;
	err = shmctl(id2, IPC_RMID, 0); 
	if (err == -1) 
		cout << "Restaurant: cannot remove segment." << endl;
	err = shmctl(id3, IPC_RMID, 0); 
	if(err == -1) 
		cout << "Restaurant: cannot remove segment." << endl;

	delete[] tcap;
	config.close();
	logging.close();

}
