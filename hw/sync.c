#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

// gcc -g -O2 -pthread sync.c
// ./a.out 2

#define MAX 20
static int queue[MAX];
static int count = 0;
static int head = 0;
static int tail = 0;

static void
put(int v) {
  assert(count < MAX);
  queue[head] = v;
  printf("%d put %d\n", pthread_self(), queue[head]);
  head = (head + 1) % MAX;
  count++;
}

static int
get() {
  assert(count > 0);
  int v = queue[tail];
  tail = (tail + 1) % MAX;
  count--;

  printf("%d get %d\n", pthread_self(), v);
  return v;
}

static pthread_t *tha;
static int nthread = 2;
static int round = 0;

pthread_mutex_t mutex;
pthread_cond_t full_cond, empty_cond;

static void *
produce(void *xa)
{
  for (int i = 0; i < 20000; i++) {
    pthread_mutex_lock(&mutex);

    while (count == MAX)
    {
      printf("prodcue thread %u wait\n", pthread_self());
      pthread_cond_wait(&full_cond, &mutex);
    }
    int v = random();
    put(v);
    printf("produce thread %u put %d\n", pthread_self(), v);

    pthread_cond_signal(&empty_cond);
    pthread_mutex_unlock(&mutex);
  }
}

static void *
consume(void *xa)
{
  for (int i = 0; i < 10000; i++) {
    pthread_mutex_lock(&mutex);

    while (count == 0)
    {
      printf("consume thread %u wait\n", pthread_self());
      pthread_cond_wait(&empty_cond, &mutex);
    }
    int v = get();
    printf("consume thread %u get %d\n", pthread_self(), v);

    pthread_cond_signal(&full_cond);
    pthread_mutex_unlock(&mutex);
  }
}

int
main(int argc, char *argv[])
{
  // nthread = atoi(argv[1]);
  nthread = 3;
  tha = malloc(sizeof(pthread_t) * nthread);
  srandom(0);

  assert(pthread_mutex_init(&mutex, NULL) == 0);
  assert(pthread_cond_init(&full_cond, NULL) == 0);
  assert(pthread_cond_init(&empty_cond, NULL) == 0);

  int i;
  for(i = 0; i < nthread; i++) {
    if (i == 0) {
      assert(pthread_create(&tha[i], NULL, produce, &i) == 0);
    } else {
      assert(pthread_create(&tha[i], NULL, consume, &i) == 0);
    }
  }
  for(i = 0; i < nthread; i++) {
    assert(pthread_join(tha[i], NULL) == 0);
  }
  printf("OK; passed\n");
}