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
	if(argc!=9){
		cerr << "Customer: Wrong Number of arguments." << endl;
		return -1;
	}
	int i = 1, people, period, shmid1, shmid2, shmid3, answer, per, decision = 0, *waitersCust;
	int mywaiter = -1, check = 0 , randomw, seatno;
	char *tables, *sharedMem, *waiters_q, *waiters_sem, *cust_answer, *waitersStatus, *bar;
	CMessage *myanswer;
	timespec ts;
	BarSeat *seat;
	bool q = false, *checked;
	sem_t *waiter , *queue;
	sharedData* mem;
	Table *mytable, *table;
	ofstream logging;
	while(i < argc){
		if(!strcmp(argv[i], "-n"))
			people = atoi(argv[i+1]);
		else if(!strcmp(argv[i], "-d"))
			period = atoi(argv[i+1]);
		else if(!strcmp(argv[i], "-s")){
			shmid1 = atoi(argv[i+1]);
			shmid2 = atoi(argv[i+2]);
			shmid3 = atoi(argv[i+3]);
			i+=2;
		}
		else{
			cerr << "Customer: Wrong argument." << endl;
			return -1;
		}
		i+=2;
	}
	srand(time(NULL));
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
	logging << "Customer group created. Process id: " << getpid() << ". Number of people: " << people << endl;
	logging << "Customer group with process id: " << getpid() << " was attached to shared memory segments with ids " << shmid1 << "," << shmid2 << "," << shmid3 << endl << endl;
	V(&(mem->output));
	logging << "Customer group with process id: " << getpid() << " waits for the doorman. " << endl;
	P(&(mem->doorm));
	P(&(mem->mutex));
	logging << "Customer group with process id: " << getpid() << " sets number of first group to : " << people << endl;
	mem->firstGroupCap = people;
	V(&(mem->mutex));
	P(&(mem->output));
	logging << "Customer group with process id: " << getpid() << " asks doorman for a table." << endl;
	V(&(mem->output));
	V(&(mem->queue));
	P(&(mem->output));
	logging << "Customer group with process id: " << getpid() << " waits for the doorman's answer." << endl;
	V(&(mem->output));
	P(&(mem->answer));
	P(&(mem->output));
	logging << "Customer group with process id: " << getpid() << " got the doorman's answer." << endl << endl;
	V(&(mem->output));
	// sleep(1000);
	P(&(mem->an_mutex));
	answer = mem->door_answer;
	V(&(mem->an_mutex));
	checked = new bool[mem->waiters];
	for(int i = 0 ; i < mem->waiters ; i++)
		checked[i] = false;
		//TODO : table
	if(answer < 0){
	//TODO : wait in bar
		decision = rand() % 30;
		if(decision != 29){
			P(&(mem->an_mutex));
			mem->door_answer = 1;
			V(&(mem->an_mutex));
			V(&(mem->answer));
			seatno = answer * (-1) - 1;
			seat = (BarSeat*) (bar + sizeof(BarSeat) * (seatno));
			P(&(mem->output));
			logging << "Customer group with process id: " << getpid() << " waits in the bar. BarSeat: " << seatno << endl;
			V(&(mem->output));
			P(&(mem->stats));
			mem->peopleBar += people;
			V(&(mem->stats));
			P(&(mem->bar_mutex));
			seat->customer = getpid();
			V(&(mem->bar_mutex));
			clock_gettime(CLOCK_REALTIME, &ts);	
			ts.tv_sec += 3;
			// P(&(seat->seatSem));
			// sleep(2);
			while(1){
				Pt(&(seat->seatSem), &ts);
				int val;
				sem_getvalue(&(seat->seatSem), &val);
				if(seat->table_given >= 0){
					V(&(mem->bar_mutex));
					break;
				}
				else{
					V(&(mem->bar_mutex));
					decision = rand() % 90;
					P(&(mem->output));
					logging << "Customer group with process id: " << getpid() << " decides if he wants to continue waiting in bar. " << endl;
					V(&(mem->output));
					if(decision == 89){
						P(&(mem->output));
						logging << "Customer group with process id: " << getpid() << " doesn't want to wait in the bar anymore. " << endl;
						V(&(mem->output));
						P(&(mem->bar_mutex));
						seat->customer = 0;
						seat->available = true;
						seat->people = 0;
						seat->priority = 0;
						seat->table_given = -1;
						V(&(mem->bar_mutex));
						P(&(mem->stats));
						mem->barLeft++;
						V(&(mem->stats));
						break;
					}
					else{
						clock_gettime(CLOCK_REALTIME, &ts);	
						ts.tv_sec += 3;	
					}
				}
			}
			P(&(mem->stats));
			mem->peopleBar -= people;
			V(&(mem->stats));
			P(&(mem->bar_mutex));
			if(seat->table_given > 0){
				answer = seat->table_given;
				seat->customer = 0;
				seat->available = true;
				seat->people = 0;
				seat->priority = 0;
				seat->table_given = -1;
			}
			mem->barCapacity += people;
			V(&(mem->bar_mutex));
		}
		else{
			P(&(mem->output));
			logging << "Customer group with process id: " << getpid() << " doesn't want to wait in the bar. " << endl;
			V(&(mem->output));
			P(&(mem->an_mutex));
			mem->door_answer = 0;
			V(&(mem->an_mutex));
			V(&(mem->answer));
			P(&(mem->stats));
			mem->peopleLeft++;
			V(&(mem->stats));
		}
	// P(&(mem->answer));
	}
	if(answer > 0){
		P(&(mem->tables_mutex));
		mytable = (Table*) (tables + (answer-1) * sizeof(Table));
		mytable->customer = getpid();
		V(&(mem->tables_mutex));	
		P(&(mem->output));
		logging << "Customer group with process id: " << getpid() << " sat in table no "  << mytable->number << endl;
		V(&(mem->output));
		P(&(mem->stats));
		mem->peopleTables += people;
		V(&(mem->stats));
		P(&(mem->waiters_mutex));
		do{
			randomw = rand() % (mem->waiters);
			if(waitersCust[randomw] == 0){
				mywaiter = randomw;
				P(&(mem->output));
				logging << "Customer group with process id: " << getpid() << " calls waiter with no "  << mywaiter+1 << endl;
				V(&(mem->output));
				break;
			}
			if(checked[randomw] == false){
				check++;
				checked[randomw] = true;
			}
		}
		while(check < mem->waiters);
		if(mywaiter < 0){
			mywaiter = rand() % (mem->waiters);
			waiter = (sem_t*) (waiters_sem + mywaiter * sizeof(sem_t));
			queue = (sem_t*) (waiters_q + mywaiter * sizeof(sem_t));
			waitersCust[mywaiter] += 2;
			P(&(mem->output));
			logging << "Customer group with process id: " << getpid() << " waits for waiter with no "  << mywaiter+1 << endl;
			V(&(mem->output));
			V(&(mem->waiters_mutex));
			P(queue);
			P(&(mem->output));
			logging << "Customer group with process id: " << getpid() << " woke up from waiter with no "  << mywaiter+1 << " " << waitersCust[mywaiter] << endl;
			V(&(mem->output));
		}
		else{
			waiter = (sem_t*) (waiters_sem + mywaiter * sizeof(sem_t));
			queue = (sem_t*) (waiters_q + mywaiter * sizeof(sem_t));
			waitersCust[mywaiter] = 1;
			V(&(mem->waiters_mutex));
		}
		P(&(mem->tables_mutex));
		mytable = (Table*) (tables + (answer-1) * sizeof(Table));
		mytable->waiter = mywaiter+1;
		V(&(mem->tables_mutex));	
		myanswer = (CMessage*) (cust_answer + mywaiter * sizeof(CMessage));
		P(&(mem->an_mutex));
		myanswer->answer = 0;
		myanswer->id = getpid();
		myanswer->table = mytable->number;
		myanswer->capacity = mytable->capacity;
		V(&(mem->an_mutex));
		V(waiter);
		P(&(mem->output));
		logging << "Customer group with process id: " << getpid() << " wakes waiter "  << mywaiter+1 << " and waits to be served." << endl;
		V(&(mem->output));
		usleep(10000);
		P(&(myanswer->ans_sem));
		P(&(mem->output));
		logging << "Customer group with process id: " << getpid() << " was served by "  << mywaiter+1 << endl;
		V(&(mem->output));
		per = rand() % (period+1); 
		sleep(per);
		P(&(mem->output));
		logging << "Customer group with process id: " << getpid() << " wants to pay." << endl;
		V(&(mem->output));
		P(&(mem->waiters_mutex));
		if(waitersCust[mywaiter] != 0)
			q = true;
		else
			q = false;
		V(&(mem->waiters_mutex));
		if(q == true){
			P(&(mem->waiters_mutex));
			waitersCust[mywaiter]+=2;
			int val;
			sem_getvalue(queue, &val);

			P(&(mem->output));
			logging << "Customer group with process id: " << getpid() << " waits for waiter with no "  << mywaiter+1 << endl;
			V(&(mem->output));
			V(&(mem->waiters_mutex));
			P(queue);
			// P(&(mem->output));
			// logging << "Customer group with process id: " << getpid() << " woke up from waiter with no "  << mywaiter+1  << endl;
			// V(&(mem->output));
		}
		else{
			P(&(mem->waiters_mutex));
			waitersCust[mywaiter] = 1;
			V(&(mem->waiters_mutex));
		}
		P(&(mem->an_mutex));
		myanswer->answer = 1;
		myanswer->id = getpid();
		myanswer->table = mytable->number;
		myanswer->capacity = mytable->capacity;
		V(&(mem->an_mutex));
		V(waiter);
		P(&(mem->output));
		logging << "Customer group with process id: " << getpid() << " calls waiter "  << mywaiter+1 << " to ask for the bill." << endl;
		V(&(mem->output));
		usleep(10000);
		P(&(myanswer->ans_sem));
		P(&(mem->output));
		logging << "Customer group with process id: " << getpid() << " paid waiter with no "  << mywaiter+1 << endl;
		V(&(mem->output));
		P(&(mem->stats));
		mem->peopleTables -= people;
		V(&(mem->stats));
	}
	else if(answer == 0){
		//TODO :leave
		P(&(mem->output));
		logging << "No table nor bar seat for customer group with process id: " << getpid() << "." << endl;
		V(&(mem->output));
		P(&(mem->stats));
		mem->peopleLeft++;
		V(&(mem->stats));
	}
	P(&(mem->stats));
	P(&(mem->output));
	logging << "Customer group with process id: " << getpid() << " leaves." << endl;
	V(&(mem->output));
	mem->cust_left++;
	if(mem->cust_left == mem->customers){
		V(&(mem->queue));
		for(int i = 0 ; i < mem->waiters ; i++){
			waiter = (sem_t*) (waiters_sem + i * sizeof(sem_t));
			V(waiter);
		}
	}
	V(&(mem->stats));
	int err = shmdt((char *) mem);
	if(err < 0){
		cerr << "Doorman: cannot detach from memory." << endl;
		return -1;
	}
	err = shmdt((char *) waitersStatus);
	if(err < 0){
		cerr << "Doorman: cannot detach from memory." << endl;
		return -1;
	}
	err = shmdt((char *) tables);
	if(err < 0){
		cerr << "Doorman: cannot detach from memory." << endl;
		return -1;
	}
	delete[] checked;
}
