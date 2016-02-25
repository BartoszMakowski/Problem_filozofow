//Bartosz Makowski

/*
Rozwiazanie wykorzystuje sposrod mechanizmow IPC wylacznie semafory.
Kluczowe jest spostrzezenie, ze jesli kazdy z filozofow podnosi sztucce w takiej samej
kolejnosci (lewa-prawa/prawa-lewa), to do zakleszczenia moze dojsc wylacznie wtedy,
gdy wszyscy filozofowie podniosa widelec po tej samej stronie kazdego z nich.
Nalezy wiec zapewnic, by w dowolnej chwili liczba filozofow, ktorzy probuja podniesc sztucce,
nie byla wieksza od N-1, co gwarantuje odpowiedni semafor.
 
*/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define N 5 // liczba filozofow

void wypisz_stan(int, char); // wypisywanie aktualnego stanu jednego filozofa
void filozof(int); // proces filozofa

int sid_widelce, sid_dostep;
struct sembuf op_wid, op_dost;


void wypisz_stan(int k, char stan){
	int i;
	for(i=0; i<k; i++){
		printf("\t");
	}
	printf("%c\n", stan);
}

void filozof(int k){
	int lewy, prawy; // indeksy sztuccow po obu stronach filozofa
	lewy = k;
	prawy = (k + 1 ) % N;
	if(fork()==0){
		while(1){
			
			wypisz_stan(k, '-');	
			sleep(rand()%3); // filozof mysli
			
			wypisz_stan(k, 'o'); // filozof chce jesc
			
			op_dost.sem_op = -1; 
			semop(sid_dostep, &op_dost, 1); // sprawdzanie, czy mozna probowac podnosic sztucce - gwarancja braku zakleszczenia
			
			op_wid.sem_num = prawy;
			op_wid.sem_op = -1;
			semop(sid_widelce, &op_wid, 1); // podniesienie prawego stucca

			op_wid.sem_num = lewy;
			op_wid.sem_op = -1; 
			semop(sid_widelce, &op_wid, 1); // podniesienie lewego sztucca
			
			wypisz_stan(k, 'X');
			sleep(rand() %4); // filozof je
			
			op_wid.sem_op = 1;
			semop(sid_widelce, &op_wid, 1); // zwolnienie lewego sztucca
			
			op_wid.sem_num = prawy;
			semop(sid_widelce, &op_wid, 1); // zwolnienie prawego sztucca

			op_dost.sem_op = 1;
			semop(sid_dostep, &op_dost, 1); // zwolnienie miejsca filozofa
		
		}
	}
}


int main(int args, char *argv[]){

	int i;
	sid_widelce = semget(0x200, N, 0600 | IPC_CREAT);
	sid_dostep = semget(0x201, 1, 0600 | IPC_CREAT);
	
	op_dost.sem_num = 0;
	op_dost.sem_flg = 0;
	op_wid.sem_flg = 0;
	
	semctl(sid_dostep, 0, SETVAL, N-1);
	
	for(i=0; i<N; i++){
		semctl(sid_widelce, i, SETVAL, 1); 
		filozof(i);
	}
	return(EXIT_SUCCESS);
}

