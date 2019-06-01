#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

#include "restaurant.h"

using namespace std;

int main(int argc, char* argv[]) {
	if(argc!=11){
		cerr << "Waiter: Wrong Number of arguments." << endl;
		return -1;
	}
	int i = 1, period, moneyamount, shmid1, shmid2, shmid3, per, money , *waitersCust, capacity, table;
	pid_t customer;
	int waiter_no, myanswer;
	CMessage *answer;
	BarSeat *seat;
	Table *available;
	char *tables, *sharedMem, *waiters_q, *waiters_sem, *cust_answer, *waitersStatus, *bar;
	sharedData* mem;
	sem_t *waiter, *queue, *barqueue;
	ofstream logging;
	while(i < argc){
		if(!strcmp(argv[i], "-d"))
			period = atoi(argv[i+1]);
		else if(!strcmp(argv[i], "-m"))
			moneyamount = atoi(argv[i+1]);
		else if(!strcmp(argv[i], "-s")){
			shmid1 = atoi(argv[i+1]);
			shmid2 = atoi(argv[i+2]);
			shmid3 = atoi(argv[i+3]);
			i+=2;
		}
		else if(!strcmp(argv[i], "-n"))
			waiter_no = atoi(argv[i+1]);
		else{
			cerr << "Waiter: Wrong argument." << endl;
			return -1;
		}
		i+=2;
	}
	logging.open("logging.txt", ofstream::out | ofstream::app);
	sharedMem = (char*) shmat(shmid1, (char*)0, 0);
	mem = (sharedData*) (sharedMem);
	waitersStatus = (char*) shmat(shmid2, (char*)0, 0);
	cust_answer = (char*) (waitersStatus + sizeof(int) * mem->waiters);
	waiters_sem = (char*) (cust_answer + sizeof(CMessage) * mem->waiters);
	waiters_q = (char*) (waiters_sem + sizeof(sem_t) * mem->waiters);
	tables = (char*) shmat(shmid3, (char*)0, 0);
	bar = (char*) (tables + sizeof(Table) * mem->tables);
	waitersCust = (int*) waitersStatus;
	P(&(mem->output));
	logging << "Waiter created with no " << waiter_no+1 << " created. Process id: " << getpid() << endl;
	V(&(mem->output));
	P(&(mem->output));
	logging << "Waiter with no " << waiter_no+1 << " attached to shared memory segments with ids " << shmid1 << "," << shmid2 << "," << shmid3 << endl << endl;
	V(&(mem->output));
	waiter = (sem_t*) (waiters_sem + waiter_no * sizeof(sem_t));
	queue = (sem_t*) (waiters_q + waiter_no * sizeof(sem_t));
	answer = (CMessage*) (cust_answer + waiter_no * sizeof(CMessage));
	srand(time(NULL));
	while(1){
		P(&(mem->output));
		logging << "Waiter with no " << waiter_no+1 << " waits. " << endl;
		V(&(mem->output));
		P(&(mem->waiters_mutex));
		if(waitersCust[waiter_no] > 1){
			waitersCust[waiter_no]--;
			V(queue);
		}
		V(&(mem->waiters_mutex));
		P(waiter);
		P(&(mem->stats));
		if(mem->cust_left == mem->customers){
			V(&(mem->stats));
			break;
		}
		V(&(mem->stats));
		P(&(mem->output));
		logging << "Waiter with no " << waiter_no+1 << " wakes up." << endl;
		V(&(mem->output));
		sleep(1);
		P(&(mem->an_mutex));
		myanswer = answer->answer;
		customer = answer->id;
		table = answer->table;
		capacity = answer->capacity;
		V(&(mem->an_mutex));
		sleep(1);
		if(myanswer == 0){
			P(&(mem->output));
			logging << "Waiter with no " << waiter_no+1 << " takes an order from the customer with process id: " << customer << endl;
			V(&(mem->output));
			per = rand() % (period+1);
			sleep(per);
			P(&(mem->output));
			logging << "Waiter with no " << waiter_no+1 << " served the customer with process id: " << customer << endl << endl;
			V(&(mem->output));
			P(&(mem->waiters_mutex));
			waitersCust[waiter_no]--;
			V(&(mem->waiters_mutex));
			V(&(answer->ans_sem));
		}
		else{
			P(&(mem->output));
			logging << "Waiter with no " << waiter_no+1 << " gives the bill to the customer with process id: " << customer << endl;
			V(&(mem->output));
			per = rand() % period;
			money = rand() % moneyamount + 1;
			sleep(per);
			P(&(mem->output));
			logging << "Waiter with no " << waiter_no+1 << " took " << money << " dollars from the customer with process id: " << customer << endl << endl;
			V(&(mem->output));
			P(&(mem->waiters_mutex));
			waitersCust[waiter_no]--;
			V(&(mem->waiters_mutex));
			P(&(mem->stats));
			mem->moneySum += money;
			mem->peoplePaid++;
			V(&(mem->stats));
			V(&(answer->ans_sem));
			P(&(mem->bar_mutex));
			int min = -1, seatNumber;
			for(int i = 0 ; i < mem->barLastSeat; i++){
				seat = (BarSeat*) (bar + sizeof(BarSeat) * i);
				if(seat->available == false && seat->people <= capacity){
					P(&(mem->stats));
					mem->moneySum += money;
					V(&(mem->stats));
					if(min < 0)
						min = seat->priority;
					else if(seat->priority < min)
						min = seat->priority;
					seatNumber = i;
				}
			}
			if(min >= 0){
				seat = (BarSeat*) (bar + sizeof(BarSeat) * seatNumber);
				seat->table_given = table;
				P(&(mem->output));
				logging << "Waiter with no " << waiter_no+1 << " informs group with process id " << seat->customer << " that table no " << table << "is available." << endl << endl;
				V(&(mem->output));
				V(&(seat->seatSem));
			}	
			else{
				P(&(mem->output));
				logging << "Waiter with no " << waiter_no+1 << " informs the doorman that table no " << table << " is available." << endl << endl;
				V(&(mem->output));
				P(&(mem->tables_mutex));
				for(int i = 0 ; i < capacity/2 ; i++)
					mem->tablesAvail[i]++;
				available = (Table*) (tables + (table-1) * sizeof(Table));
				available->available = true;
				available->customer = 0;
				V(&(mem->tables_mutex));
			}
			V(&(mem->bar_mutex));
		}
	}

	P(&(mem->output));
	logging << "Waiter " << waiter_no+1 << " leaves." << endl;
	V(&(mem->output));
	int err = shmdt((char *) mem);
	if(err < 0){
		cerr << "Doorman: cannot detach from memory." << endl;
		return -1;
	}
	err = shmdt((char *) waitersCust);
	if(err < 0){
		cerr << "Doorman: cannot detach from memory." << endl;
		return -1;
	}
	err = shmdt((char *) tables);
	if(err < 0){
		cerr << "Doorman: cannot detach from memory." << endl;
		return -1;
	}
	logging.close();
}
