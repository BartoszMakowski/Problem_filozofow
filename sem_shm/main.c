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

int *stan;
int sid, sid2, mid;
struct sembuf op, op2;

void podnies_widelec(int k){
	stan[k] = 1;
	testuj(k);
	if(stan[k]!=2){	
            struct sembuf op2;
            op2.sem_num = k;
            op2.sem_flg = 0;
            op2.sem_op = -1;
            semop(sid, &op2, 1);
	}	
}

void odloz_widelec(int k){
	stan[k] = 0;
	testuj((k+4)%5);
	testuj((k+1)%5);	
}

void testuj(int k){

	if ((stan[(k+4)%5] != 2) && (stan[(k+1)%5] != 2) && (stan[k] == 1)){
		stan[k]=2;
		op.sem_num = k;
        op.sem_flg = 0;
        op.sem_op = 1;
		semop(sid, &op, 1);
	}
}

void filozof(int k){
    if(fork()==0){
        while(1){
            //printf("FILOZOF %d MYŚLI.\n", k);
            sleep(rand()%3);
            //printf("FILOZOF %d JEST GŁODNY.\n", k);

			op2.sem_num = 0;
            op2.sem_flg = 0;
            op2.sem_op = -1;
			semop(sid2, &op2, 1);

			stan[k]=1;
			testuj(k);

			op2.sem_num = 0;
            op2.sem_flg = 0;
            op2.sem_op = 1;
			semop(sid2, &op2, 1);

			op.sem_num = k;
        	op.sem_flg = 0;
        	op.sem_op = -1;
			semop(sid, &op, 1);

            //printf("FILOZOF %d JE.\n", k);
            sleep(rand()%3);

			op2.sem_op = -1;
			semop(sid2, &op2, 1);

            odloz_widelec(k);

			op2.sem_op = 1;
			semop(sid2, &op2, 1);
        }
    }
}


int main(int args, char** argv) {
    
    int i,k;    
    sid = semget(0x123, 5, 0600 | IPC_CREAT);
	sid2 = semget(0x124, 1, 0600 | IPC_CREAT);
	mid = shmget(0x125, 1024, 0600 | IPC_CREAT);
    stan = (int *)shmat(mid, 0,0);
	
    
    for (i=0; i < 5; i++){
		stan[i]=0;
        semctl(sid, i, SETVAL, 0);
    }

	semctl(sid2, 0, SETVAL, 1);    

    for (i=0; i < 5; i++){
        filozof(i);
    }

	while(1){
		
		//for (i=0; i < 5; i++){
       // 	k = semctl(sid, i, GETVAL, 0);
		//	printf("%d: %d\n", i, k, stan[i]); 				
		//}
		printf("\t\t\t%d: %d\n", 0, stan[0]);
		printf("\t\t%d: %d", 1, stan[1]);  
		printf("\t\t%d: %d\n", 4, stan[4]);
		printf("\t\t  %d: %d", 2, stan[2]);  
		printf("\t   %d: %d\n\n\n", 3, stan[3]);
		//printf("W:\t%d\n", semctl(sid2, 0, GETVAL, 0));  
		sleep(1);
	}
    
    wait(NULL);

	
    
    exit(0);
    

}


