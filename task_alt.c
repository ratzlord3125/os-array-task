// for testing the alt logic
 
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <pthread.h>
#include <signal.h>
 
#include "./CSF372.h"
 
int processReturn = 0; // to collect error value
int rowthapx = 0; // for storing the sum of thapx of each cell in a row
 
int a = 0; // declaing a globally so that it's accesible to processRow function
int b = 0; // declaing b globally so that it's accesible to processRow function
int p = 0; // declaing p globally so that it's accesible to thapxCalc function
 
void sigchldErr() 
{
    if(processReturn > 0) 
    {
        PRINT_ERROR("There is some error...exiting all processes");
        exit(EXIT_FAILURE); 
    } 
    else 
    {
        PRINT_INFO("Child exited successfully");
    }
}
 
int isPrime(int a)
{
    int i;
    int prime = 1;
    if(a == 1)
        return 0;
    if(a == 2)
        return 1;
    for(i = 2; i <= a/2; i++)
    {
        if(a%i == 0)
        {
            prime = 0;
            break;
        }
    }
    return prime;
}
 
void* thapxCalc(void* val) 
{
    int x = *((int*)val);
    printf("Processing element : %d\n", x); 
 
    if(x < 0 || x < a || x > b) // checking range of the element
        exit(EXIT_FAILURE); 
 
    // lower half of primes
    int p1 = p, y = x-1, th = 0, c = 0;
    while(y > 0 && p1 > 0)
    {
        if(isPrime(y))
        {
            printf("Prime no. found = %d \n", y);
            th += y;
            p1--;
            c++;
        }
        y--;
    }
 
    // the element itself
    if(isPrime(x))
    {
        printf("Prime no. found = %d \n", x);
        th += x;
        c++;
    }
 
    // upper half of primes
    y = x+1;
    p1 = p;
    while(p1 > 0)
    {
        if(isPrime(y))
        {
            printf("Prime no. found = %d \n", y);
            th += y;
            p1--;
            c++;
        }
        y++;
    }
 
    int thapx = th/c;
 
    printf("thapx of element %d = %d\n", x, thapx);
 
    rowthapx += thapx; // summing thapx of each element to global variable so that wpapx can be calculated after thread ends
 
    PRINT_INFO("Terminating thread for element : %d", x);
    pthread_exit(EXIT_SUCCESS); 
}
 
int processRow(int* arr, int n) 
{
    int i;
    // printing the row to be processed
    for(i=0; i<n; i++) 
        printf("%d ", arr[i]); 
    putchar('\n'); 
 
    pthread_t tid; 
    pthread_attr_t attr; 
    pthread_t workers[10]; 
    for(i=0; i<n; i++) 
    {
        PRINT_INFO("Creating thread ptid = %ld", tid);
        pthread_create(&tid, NULL, thapxCalc, &arr[i]); 
        workers[i] = tid; 
    }
    
    PRINT_INFO("Joining all threads");
    for(i=0; i<n; i++) 
        pthread_join(workers[i], NULL); 
    
    int wpapx = rowthapx/n;
    printf("wpapx of current row = %d\n", wpapx);
    return wpapx; 
}
 
int main(int argc, char**argv)
{
    // loop variables
    int i,j;
 
    // taking values according to input format
    int n = atoi(argv[1]);
    int aa = atoi(argv[2]);
    int bb = atoi(argv[3]);
    int pp = atoi(argv[4]); 
    a = aa; // setting global value of a
    b = bb; // setting global value of b
    p = pp; // setting global value of p
 
    int arr[10][10]; // to store the input in matrix 
    for(i=0; i<n*n; i++) 
    {
        arr[i/n][i%n] = atoi(argv[i+5]);
    }
 
    printf("---INPUT---\n");
    printf("n : %d \na : %d \nb : %d \np : %d\n", n, a, b, p); 
    printf("Matrix : \n");
    for(i=0; i<n; i++) 
    {
        for(j=0; j<n; j++)
            printf("%d ",arr[i][j]);
        putchar('\n');
    }
 
    printf("---PROCESSING STARTS HERE---\n");
    
    signal(SIGCHLD, sigchldErr); // to handle errors
 
    //forking n processes
    int control_pid = getpid(); 
    int pi[20]; // n*2 size of pipe array, for read and write value of n rows
 
    int colwpapx = 0; // for storing the sum of wpapx of each row
    
    PRINT_INFO("Forking %d processes for the parent process pid = %d", n, control_pid); 
    
    int x; // for storing read and write value of pipes
 
    for(i=0;i<n;i++) 
    {
        pipe(pi+i*2);
        PRINT_INFO("Pipe between parent pid = %d and child pid = %d created", getppid(), getpid());
 
        pid_t child_pid = fork(); 
        
 
        if(child_pid == 0)
        {
           printf("Processing row no. %d : ", i); 
            x = processRow(arr[i], n); 
            write(*(pi+i*2+1), &x, sizeof(int)); 
            printf("Writing wpapx %d into pipe\n", x);
            exit(EXIT_SUCCESS); 
        }
 
    }
    for(i=0;i<n;i++) 
    {
        PRINT_INFO("Forked child pid = %d for parent pid = %d", getpid(), getppid());
 
        wait(&processReturn);
        read(*(pi+i*2), &x, sizeof(int)); 
        printf("Reading wpapx %d from parent \n", x);
        colwpapx += x;
    }
 
    int fapx = colwpapx/n;
    printf("The final calculated fapx = %d\n",fapx);
 
    return 0;
}
 
// gcc task_alt.c -pthread -o task_alt && ./task_alt 4 10 99 5 17 28 67 65 22 19 11 77 89 78 45 40 20 10 90 76