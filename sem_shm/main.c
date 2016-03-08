//Bartosz Makowski
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <sys/ipc.h>

void odloz_widelec(int); // zwalnianie sztuccow
void testuj(int); // sprawdzanie dostepnosci sztuccow
void filozof(int); // proces filozofa


int *stan;
int sid_filozof, sid_dostep, mid;
struct sembuf op_fil, op_dost;


void odloz_widelec(int k){
// jesli filozow konczy jesc, sprawdza, czy zwolnione sztucce nie sa potrzebne sasiadom
	stan[k] = 0;
	testuj((k+4)%5);
	testuj((k+1)%5);	
}

void testuj(int k){
// jesli sasiedzi nie jedza i filozof chce jesc, to nie ma przeszkod, by to uczynil
	if ((stan[(k+4)%5] != 2) && (stan[(k+1)%5] != 2) && (stan[k] == 1)){ 
		stan[k]=2;
		op_fil.sem_num = k;
        	op_fil.sem_flg = 0;
        	op_fil.sem_op = 1;
		semop(sid_filozof, &op_fil, 1); // zgoda na jedzenie
	}
}

void filozof(int k){
    if(fork()==0){
        while(1){
            sleep(rand() %3); // filozof mysli
			
		op_dost.sem_num = 0; 
            	op_dost.sem_flg = 0;
            	op_dost.sem_op = -1;
		semop(sid_dostep, &op_dost, 1); // wejscie do sekcji krytycznej
		stan[k]=1;
		testuj(k);

		op_dost.sem_num = 0; 
            	op_dost.sem_flg = 0;
            	op_dost.sem_op = 1;
		semop(sid_dostep, &op_dost, 1); // wyjscie z sekcji krytycznej

		op_fil.sem_num = k; 
        	op_fil.sem_flg = 0;
        	op_fil.sem_op = -1;
		semop(sid_filozof, &op_fil, 1); // proba jedzenia
            
            	sleep(rand() %4); // filozof je

		op_dost.sem_op = -1; 
		semop(sid_dostep, &op_dost, 1); // wejscie do sekcji krytycznej

            	odloz_widelec(k);

		op_dost.sem_op = 1;
		semop(sid_dostep, &op_dost, 1); // wyjscie z sekcji krytycznej
        }
    }
}


int main(int args, char** argv) {
    
    	int i;    
    	sid_filozof = semget(0x123, 5, 0600 | IPC_CREAT);
	sid_dostep = semget(0x124, 1, 0600 | IPC_CREAT);
	mid = shmget(0x125, 1024, 0600 | IPC_CREAT);
    	stan = (int *)shmat(mid, 0,0); // tablica w pamieci wspoldzielonej, zawierajaca stan kazdego z filozofow
	srand(time(NULL));
    
	for (i=0; i < 5; i++){
		stan[i]=0;
		semctl(sid_filozof, i, SETVAL, 0);
    	}

	semctl(sid_dostep, 0, SETVAL, 1);    

	for (i=0; i < 5; i++){
		filozof(i);
	}

	while(1){		
// czytelne wypisywanie ID: STAN
		printf("\t\t\t%d: %d\n", 0, stan[0]);
		printf("\t\t%d: %d", 1, stan[1]);  
		printf("\t\t%d: %d\n", 4, stan[4]);
		printf("\t\t  %d: %d", 2, stan[2]);  
		printf("\t   %d: %d\n\n\n", 3, stan[3]); 
		sleep(1);
	}
    
    exit(0);
}


