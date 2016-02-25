#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>

#define N 5

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
	int lewy, prawy;
	lewy = k;
	prawy = (k + 1 ) % N;
	int i;
	if(fork()==0){
		while(1){
			//printf("%d: MYŚLI\n", k);
			wypisz_stan(k, '-');
	
			sleep(rand()%3);
			//printf("%d: ZGŁODNIAŁ\n", k);
			wypisz_stan(k, 'o');
			
			op_dost.sem_op = -1;
			semop(sid_dostep, &op_dost, 1);
			
			op_wid.sem_num = prawy;
			op_wid.sem_op = -1;
			semop(sid_widelce, &op_wid, 1);			

			op_wid.sem_num = lewy;
			op_wid.sem_op = -1; 
			semop(sid_widelce, &op_wid, 1);
			
			//printf("%d: JE\n", k);
			wypisz_stan(k, 'X');

			sleep(rand() %4);
			
			op_wid.sem_op = 1;
			semop(sid_widelce, &op_wid, 1);
			
			op_wid.sem_num = prawy;
			semop(sid_widelce, &op_wid, 1);

			op_dost.sem_op = 1;
			semop(sid_dostep, &op_dost, 1);
		
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


	
	

}

