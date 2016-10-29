#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}
void min_func_call(){}

unsigned long long process_switch_timer();
unsigned long long *result2;

void* thread0Call();
void* thread1Call();
pthread_mutex_t lock;
int state=0; // state = 0 or 1
pthread_key_t glob_var_key;
pthread_cond_t cond0;
pthread_cond_t cond1;
int num=0;
struct timespec startT0;
struct timespec stopT0;
struct timespec startT1;
struct timespec stopT1;
unsigned long long resultT0;
unsigned long long resultT1;


int main()
{

//--------------------------------Part 1--------------------------------
// timespec has 2 members tv_sec and tv_nsec that represent the number of seconds and nanoseconds elapsed, respectively
struct timespec start;
struct timespec stop;
unsigned long long result; //64 bit integer
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
// The first clock is CLOCK_REALTIME,
// which records the start and stop time of the system clock after sleeping for 1 second,
// This should only be used to disply a time on screen
// because it might causes errors when user change system clock in run-time
///////////////////////////////////////////////////////////////////////////////////////////
clock_gettime(CLOCK_REALTIME, &start);
sleep(1);
clock_gettime(CLOCK_REALTIME, &stop);

result=timespecDiff(&stop,&start);

printf("CLOCK_REALTIME Measured: %llu\n",result);

///////////////////////////////////////////////////////////////////////////////////////////
// The second clock is CLOCK_MONOTONIC,
// which records the absolute elapsed time of the system,
// this is useful when a time-dependent function needs to be carried out,
// as user cannot alter this clock
///////////////////////////////////////////////////////////////////////////////////////////
clock_gettime(CLOCK_MONOTONIC, &start);
sleep(1);
clock_gettime(CLOCK_MONOTONIC, &stop);

result=timespecDiff(&stop,&start);

printf("CLOCK_MONOTONIC Measured: %llu\n",result);

///////////////////////////////////////////////////////////////////////////////////////////
// The third clock is CLOCK_PROCESS_CPUTIME_ID,
// which records ONLY the elapsed time when the process is being executed in the CPU,
// this is useful when executing multiple process and when user want to mearsure the time elapsed
// for a particular process
///////////////////////////////////////////////////////////////////////////////////////////
clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
sleep(1);
clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

result=timespecDiff(&stop,&start);

printf("CLOCK_PROCESS_CPUTIME_ID Measured: %llu\n",result);

///////////////////////////////////////////////////////////////////////////////////////////
// The forth clock is CLOCK_THREAD_CPUTIME_ID,
// which is simlar to the last one but instead records the time a thread is being executed in CPU,
// this is useful when a program uses multiple threads
///////////////////////////////////////////////////////////////////////////////////////////
clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
sleep(1);
clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);

result=timespecDiff(&stop,&start);

printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result);


//--------------------------------Part 2--------------------------------
int n=0;
FILE* fminfnc = fopen("minFuncRecord.txt", "w");
for(n;n<500;n++){
  clock_gettime(CLOCK_MONOTONIC, &start);
  min_func_call();
  clock_gettime(CLOCK_MONOTONIC, &stop);

  result=timespecDiff(&stop,&start);
  fprintf(fminfnc,"%llu\n",result);
}


printf("Minmal Function Call Measured: %llu\n",result);

//--------------------------------Part 3--------------------------------
FILE* fsyscall = fopen("fsyscallRecord.txt", "w");
for(n=0;n<500;n++){
  clock_gettime(CLOCK_MONOTONIC, &start);
  getpid();
  clock_gettime(CLOCK_MONOTONIC, &stop);

  result=timespecDiff(&stop,&start);
  fprintf(fsyscall,"%llu\n",result);
}
printf("Minmal System Call Measured: %llu\n",result);

//--------------------------------Part 4--------------------------------
// int j=0;
// FILE* fp = fopen("process_switch_timerP.txt", "w");
// unsigned long long resultP;
// for(j;j<500;j++){
//   resultP = process_switch_timer();
//   fprintf(fp,"%llu\n",resultP);
// }


//--------------------------------Part 5--------------------------------
// cpu_set_t mask;
// CPU_ZERO(&mask);
// CPU_SET(0, &mask);
// sched_setaffinity(getpid(), sizeof(mask), &mask);
//
// pthread_t rt[1];
// int i =0;
//
//
// pthread_mutex_init(&lock, NULL);
//
//
// pthread_create(&rt[0],NULL,&thread0Call,&num);
// pthread_create(&rt[1],NULL,&thread1Call,&num);
//
//
// pthread_join(rt[0],NULL);
// pthread_join(rt[1],NULL);
//
//
// pthread_mutex_destroy(&lock);

return 0;
}


void* thread0Call(){
  FILE* thredRecord = fopen("thredRecord.txt", "w");
  int i=0;
  while(i<500){
    pthread_mutex_lock(&lock);
    while(state!=0){
      pthread_cond_wait(&cond0, &lock);
    }

    pthread_mutex_unlock(&lock);
    clock_gettime(CLOCK_MONOTONIC, &stopT1);
    resultT1=timespecDiff(&stopT1,&startT1);
    fprintf(thredRecord,"%llu\n",resultT1);

    num = 0;
    pthread_mutex_lock(&lock);
    state = 1;
    pthread_cond_signal(&cond1);
    pthread_mutex_unlock(&lock);
    i++;
  }

  return NULL;
}

void* thread1Call(){
  int j=0;
  while(j<500){
    pthread_mutex_lock(&lock);
    while(state!=1){
      pthread_cond_wait(&cond1, &lock);
    }
    pthread_mutex_unlock(&lock);
    num = 1;

    pthread_mutex_lock(&lock);
    state = 0;
    pthread_cond_signal(&cond0);
    pthread_mutex_unlock(&lock);
    clock_gettime(CLOCK_MONOTONIC, &startT1);
    j++;
  }
  return NULL;
}

unsigned long long process_switch_timer(){
  int i = 0;
  int j =0;
  int times = 5;
  unsigned long long difference;
  unsigned long long average;

  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(0, &mask);

  struct timespec start1;
  struct timespec stop1;
  struct timespec start2;
  struct timespec stop2;
  struct timespec start3;
  unsigned long long result1;
  result2=mmap(NULL, sizeof *result2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  *result2=0;

 int ChildtoParent[2]; // pipe for child contacts parent
 int ParenttoChild[2]; // pipe for parent contacts child
 int WriteStopTime[2];
 int byte;

 char readChar[] = "h";
 pid_t child_pid;
 sched_setaffinity(0, sizeof(mask), &mask);
 sched_setaffinity(getpid(), sizeof(mask), &mask);

 pipe(ChildtoParent);
 pipe(ParenttoChild);
 pipe(WriteStopTime);


 child_pid = fork();

 if (child_pid == -1){
   perror("fork error");
   exit(1);
 }
 if(child_pid==0){
   close(ParenttoChild[0]); // Close output of parent to child
   close(ChildtoParent[1]); // Close input side of parent
   for(j=0;j<500;j++){
     read(ChildtoParent[0],readChar,sizeof(readChar));
     write(ParenttoChild[1],readChar,(strlen(readChar)));// go to parent here
   }
   clock_gettime(CLOCK_MONOTONIC, &stop1);

   write(WriteStopTime[1],&stop1,sizeof(stop1));
   exit(0);
 }
 else{
   close(ParenttoChild[1]); // Close output of parent to child
   close(ChildtoParent[0]);
   int i=0;

   //timer start
   clock_gettime(CLOCK_MONOTONIC, &start1);
   for(i=0;i<500;i++){
     write(ChildtoParent[1],readChar,(strlen(readChar)));// go to child here
     read(ParenttoChild[0],readChar,sizeof(readChar));
   }

   read(WriteStopTime[0],&stop1,sizeof(stop1));

   wait(NULL);

   result1 = timespecDiff(&stop1,&start1);
   result1=result1/1000;
 }
 return result1;
}
