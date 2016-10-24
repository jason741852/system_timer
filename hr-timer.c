#include <stdio.h>
#include <stdint.h>
#include <time.h>

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}
void min_func_call(){}


int main()
{
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


clock_gettime(CLOCK_MONOTONIC, &start);
min_func_call();
clock_gettime(CLOCK_MONOTONIC, &stop);

result=timespecDiff(&stop,&start);

printf("Minmal Function Call Measured: %llu\n",result);

clock_gettime(CLOCK_MONOTONIC, &start);
getpid();
clock_gettime(CLOCK_MONOTONIC, &stop);

result=timespecDiff(&stop,&start);

printf("Minmal System Call Measured: %llu\n",result);


}
