/*Write two C program named program1.c and program2.c and both will be responsible
for reading the contents of a single file. Now you put the concept of semaphore to provide
synchronization between the process program1 and program2, so that program2 can read the
contents of the specified file only after reading the contents of the file by program1. Again if
the program1 runs for n times, then program2 can read the contents of the file at most n times
only, not more than that. Moreover you have to remove the semaphore that is created for your
above mentioned operations properly by using proper system call.*/  

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<unistd.h>
#include<signal.h>

// union semun {
// 	int val;
// 	struct semid_ds *buf;
// 	unsigned short  *array;
// 	struct seminfo  *__buf;
// };

void wait_operation(int sem_set_identifier){
	struct sembuf sops[2];

	sops[0].sem_num = 0;
	sops[0].sem_op = 0;
	sops[0].sem_flg = 0;

	sops[1].sem_num = 0;
	sops[1].sem_op = 1;
	sops[1].sem_flg = 0;

	if(semop(sem_set_identifier, sops, 2) == -1){
		perror("semop() error");
		exit(1);
	}

	return;
}

void signal_operation(int sem_set_identifier){
	struct sembuf sops[1];

	sops[0].sem_num = 0;
	sops[0].sem_op = -1;
	sops[0].sem_flg = 0;

	if(semop(sem_set_identifier, sops, 1) == -1){
		perror("semop() error");
		exit(1);
	}

	return;
}

void handler_function(int signal){
	key_t key;
	int sem_set_identifier;
	int pid;

	key = ftok("file", 1);
	sem_set_identifier = semget(key, 1, 0666 | IPC_CREAT);

	signal_operation(sem_set_identifier);
	wait_operation(sem_set_identifier);

	printf("Enter Process ID\n");
	scanf("%d", &pid);	
	kill(pid, SIGINT);

	semctl(sem_set_identifier, 0, IPC_RMID);

	exit(0);
}
int main(int argc, char* argv[]){
	key_t key;
	int sem_set_identifier;
	union semun sem_ctl_4;

	signal(SIGINT, handler_function);

	key = ftok("file", 1);
	sem_set_identifier = semget(key, 1, 0666 | IPC_CREAT);

	sem_ctl_4.val = 0;
	semctl(sem_set_identifier, 0, SETVAL, sem_ctl_4);

	int count = 1;
	while(1){
		printf("Enter any character to continue\n");
		char ch = getchar();
		
		if(count > 1)
			signal_operation(sem_set_identifier);

		wait_operation(sem_set_identifier);

		if(count == 1)
			sleep(4);

		FILE *fp = fopen("read.txt","r");
		char buffer[1000];
		fgets(buffer, 1000, fp);
		printf("%d. %s\n", count, buffer);
		count++;

	}
	
}
