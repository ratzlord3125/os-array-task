// CHANGE THE WHOLE CODE!!

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <pthread.h>
#include <signal.h>

int processReturn = 0;
int rowSum = 0;

void* averageCalc(void* val) {
    int x = *((int*)val);
    printf("Processing element :: %d\n", x); 
    rowSum += x; 
    if(x < 0) exit(1); 
    pthread_exit(0); 
}
int processRow(int* arr, int n) {
    for(int i=0; i<n; i++) printf("%d ", arr[i]); putchar('\n'); 
    pthread_t tid; 
    pthread_attr_t attr; 
    pthread_t workers[n]; 
    for(int i=0; i<n; i++) {
        // printf("Creating thread (ptid :: %ld)\n", tid);
        pthread_create(&tid, NULL, averageCalc, &arr[i]); 
        workers[i] = tid; 
    }
    for(int i=0; i<n; i++) {
        pthread_join(workers[i], NULL); 
    }
    return rowSum; 
}

void sigchld() {
    if(processReturn > 0) {
        printf("There is some error\n. Exiting all processes\n");
        exit(1); 
    } else {
        printf("Child exited successfully\n");
    }
}
int main(int argc, char**argv)
{

    int n = atoi(argv[1]), a = atoi(argv[2]), b = atoi(argv[3]), p = atoi(argv[4]); 
    int arr[n][n]; 

    printf("n : %d | a : %d | b : %d | p : %d\n", n, a, b, p); 
    printf("The entered matrix : \n");
    for(int i=0; i<n*n; i++) {
        if((i)%n == 0) putchar('\n');
        printf("%s ", argv[i+5]);
        arr[i/n][i%n] = atoi(argv[i+5]); 
    }
    putchar('\n');
    int pi[n*2]; 
    //forking n processes
    int controller_pid = getpid(); 
    signal(SIGCHLD, sigchld);
    printf("Forking %d processes for the parent process (pid :: %d)\n", n, controller_pid); 
    for(int i=0; i<n; i++) {
        int pid = getpid(); 
        pipe(pi+i*2);
        if(pid == controller_pid) {
            int child_pid = fork(); 
            if(child_pid) { //you are in parent
                printf("Forked child (pid :: %d) for parent (pid :: %d)\n", child_pid, getppid()); 
                int x;
                wait(&processReturn);
                read(*(pi+i*2), &x, sizeof(int)); 
                printf("Read value %d from parent \n", x);
            } else { //you are in child
                printf("Processing row :: %d : ", i); 
                int x = processRow(arr[i], n); 
                write(*(pi+i*2+1), &x, sizeof(int)); 
                printf("Written value %d into pipe\n", x);
                exit(0); 
            }
        }
    }
    return 0;
}
//4 10 99 5 17 28 67 65 22 19 11 77 89 78 45 40 20 10 90 76
// gcc task.c -pthread -o task && ./task 10 99 5 17 28 67 65 22 19 11 77 89 78 45 40 20 10 90 76