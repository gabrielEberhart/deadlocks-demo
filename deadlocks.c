#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define RUNTIME 10 // seconds
#define TIMEOUT 1  // second
#define SETTIME()                                                              \
  struct timespec ts;                                                          \
  clock_gettime(CLOCK_REALTIME, &ts);                                          \
  ts.tv_sec += TIMEOUT;

pthread_mutex_t lockA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockB = PTHREAD_MUTEX_INITIALIZER;

// flag to indicate if program should continue running
volatile int running = 1;

void *thread_a(void *_) {
  while (running) {
    pthread_mutex_lock(&lockA);
    printf("[Thread A] Locked A\n");
    sleep(2);

    // try to lock B with timeout to avoid permanent deadlock
    SETTIME()

    int ret = pthread_mutex_timedlock(&lockB, &ts);
    if (ret == 0) {
      printf("[Thread A] Locked B\n");
      //! Critical section
      sleep(1);
      pthread_mutex_unlock(&lockB);
      printf("[Thread A] Unlocked B\n");
    } else if (ret == ETIMEDOUT)
      printf("[Thread A] Could not lock B (timeout), releasing A\n");

    // cleanup
    pthread_mutex_unlock(&lockA);
    printf("[Thread A] Unlocked A\n");
    sleep(1);
  }

  return NULL;
}

void *thread_b(void *_) {
  while (running) {
    pthread_mutex_lock(&lockB);
    printf("[Thread B] Locked B\n");
    sleep(2);

    // try to lock B with timeout to avoid permanent deadlock
    SETTIME()

    int ret = pthread_mutex_timedlock(&lockA, &ts);
    if (ret == 0) {
      printf("[Thread B] Locked A\n");
      //! Critical section
      sleep(1);
      pthread_mutex_unlock(&lockB);
      printf("[Thread B] Unlocked A\n");
    } else if (ret == ETIMEDOUT)
      printf("[Thread B] Could not lock A (timeout), releasing B\n");

    // cleanup
    pthread_mutex_unlock(&lockA);
    printf("[Thread B] Unlocked B\n");
    sleep(1);
  }

  return NULL;
}

int main() {
  pthread_t threadA, threadB;
  int result;

  // spawn threads
  result = pthread_create(&threadA, NULL, thread_a, NULL);
  if (result != 0)
    goto Error;
  result = pthread_create(&threadB, NULL, thread_b, NULL);
  if (result != 0)
    goto Error;

  // let run
  sleep(RUNTIME);
  // close threads
  printf("\nShutting down...\n");
  running = 0;

  // Cancel threads if stuck
  printf("Canceling threads if necessary\n");
  pthread_cancel(threadA);
  pthread_cancel(threadB);

  pthread_join(threadA, NULL);
  printf("Thread A joined\n");
  pthread_join(threadB, NULL);
  printf("Thread B joined\n");

  // cleanup mutexes
  pthread_mutex_destroy(&lockA);
  pthread_mutex_destroy(&lockB);

  // end program
  printf("Program finished successfully\n");
  return 0;

Error:
  perror("Failed to create thread\n");
  exit(1);
}
