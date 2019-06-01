#include <iostream>
#include <fstream>
// #include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <cstdlib>
#include <ctime>

#include "restaurant.h"

using namespace std;

int main(int argc, char* argv[]) {
	if(argc!=7){
		cerr << "Doorman: Wrong Number of arguments." << endl;
		return -1;
	}
	int i = 1, period, shmid1, shmid2, shmid3, div, mod, pos, per, seatNumber, priorityNum = 0, myanswer;
	char *tables, *sharedMem, *waiters_q, *waiters_sem, *cust_answer, *waitersStatus, *bar;
	BarSeat *seat;
	Table *currentTable;
	sharedData* mem;
	ofstream logging;
	while(i < argc){
		if(!strcmp(argv[i], "-d"))
			period = atoi(argv[i+1]);
		else if(!strcmp(argv[i], "-s")){
			shmid1 = atoi(argv[i+1]);
			shmid2 = atoi(argv[i+2]);
			shmid3 = atoi(argv[i+3]);
			i+=2;
		}
		else{
			cerr << "Doorman: Wrong argument." << endl;
			return -1;
		}
		i+=2;
	}
	logging.open("logging.txt", ofstream::out | ofstream::app);
	if(!logging.is_open()){
		cerr << "Doorman: cannot open logging file." << endl;
	}
	// cout << endl << "hello i am the doorman and i am quiting this job" << endl;
	sharedMem = (char*) shmat(shmid1, (char*)0, 0);
	mem = (sharedData*) (sharedMem);
	waitersStatus = (char*) shmat(shmid2, (char*)0, 0);
	cust_answer = (char*) (waitersStatus + sizeof(int) * mem->waiters);
	waiters_sem = (char*) (cust_answer + sizeof(CMessage) * mem->waiters);
	waiters_q = (char*) (waiters_sem + sizeof(sem_t) * mem->waiters);
	tables = (char*) shmat(shmid3, (char*)0, 0);
	bar = (char*) (tables + sizeof(Table) * mem->tables);
	P(&(mem->output));
	logging << "Doorman created. Process id: " << getpid() << endl;
	V(&(mem->output));
	P(&(mem->output));
	logging << "Doorman attached to shared memory segments with ids " << shmid1 << "," << shmid2 << "," << shmid3 << endl << endl;;
	V(&(mem->output));
	srand(time(NULL));
	while(1){
		sleep(1);
		V(&(mem->doorm));
		P(&(mem->output));
		logging << "Doorman waits. " << endl;
		V(&(mem->output));
		P(&(mem->queue));
		P(&(mem->stats));
		if(mem->cust_left == mem->customers){
			V(&(mem->stats));
			break;
		}
		V(&(mem->stats));
		P(&(mem->output));
		logging << "Doorman serves customer." << endl;
		V(&(mem->output));
		P(&(mem->mutex));
		P(&(mem->output));
		logging << "Current group's number of people: " << mem->firstGroupCap << endl;
		V(&(mem->output));
		div = mem->firstGroupCap / 2;
		mod = mem->firstGroupCap % 2;
		P(&(mem->tables_mutex));
		pos = div + mod - 1;
		P(&(mem->output));
		logging << "Number of available tables suitable for this group: " << mem->tablesAvail[pos] << endl;
		V(&(mem->output));
		per = rand() % (period+1);
		sleep(per);
		if(mem->tablesAvail[pos] > 0){
			for(int i = 0 ; i < mem->tables; i++){
				currentTable = (Table*) (tables + i * sizeof(Table));
				if(currentTable->available == true && currentTable->capacity / mem->firstGroupCap !=0){
					P(&(mem->output));
					logging << "Doorman gives customer the table no : " << currentTable->number << endl;
					V(&(mem->output));
					currentTable->available = false;
					P(&(mem->an_mutex));
					mem->door_answer = currentTable->number;
					V(&(mem->an_mutex));
					break;
				}
			}
			pos = currentTable->capacity / 2;
			for(int i=0 ; i < pos ; i++)
				mem->tablesAvail[i]--;
			V(&(mem->tables_mutex));
			V(&(mem->answer));
		}
		else{
			V(&(mem->tables_mutex));
			P(&(mem->bar_mutex));
			if(mem->barCapacity < mem->firstGroupCap || mem->tablesCap[pos] == 0){
				P(&(mem->an_mutex));
				mem->door_answer = 0;
				V(&(mem->an_mutex));
				V(&(mem->answer));
			}
			else{
				for(int i = 0 ; i < mem->barLastSeat ; i++){
					seat = (BarSeat*) (bar + sizeof(BarSeat) * i);
					if(seat->available){
						seatNumber = i;
						break;
					}
				}
				P(&(mem->an_mutex));
				mem->door_answer = 0 - 1 - seatNumber;
				V(&(mem->an_mutex));
				V(&(mem->answer));
				usleep(10000);
				P(&(mem->answer));
				P(&(mem->an_mutex));
				myanswer = mem->door_answer;
				V(&(mem->an_mutex));
				if(myanswer == 1){
					seat = (BarSeat*) (bar + sizeof(BarSeat) * seatNumber);
					seat->available = false;
					seat->people = mem->firstGroupCap;	
					seat->priority = priorityNum;
					priorityNum++;
					mem->barCapacity -= mem->firstGroupCap;
				}
			}
			V(&(mem->bar_mutex));
		}
		V(&(mem->mutex));
	}
	P(&(mem->output));
	logging << "Doorman leaves." << endl;
	V(&(mem->output));
	sleep(4);
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
	logging.close();
}
