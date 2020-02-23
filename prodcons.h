#include "stdio.h" 
#include "sys/types.h"
#include "stdlib.h"
#include "string.h"

extern int producerscount;
extern pthread_mutex_t mu;
extern pthread_cond_t notReady;
extern int rc, datap, datac, flag, jobDone, endLoop;
extern FILE *f2, *f1;

typedef struct circular_buffer
{
    void *buffer;     // data buffer
    void *buffer_end; // end of data buffer
    size_t capacity;  // maximum number of items in the buffer
    size_t count;     // number of items in the buffer
    size_t sz;        // size of each item in the buffer
    void *head;       // pointer to head
    void *tail;       // pointer to tail
} circular_buffer;

typedef struct producer_arguments {
	int prod_id;
	int total;
	int seed;
} PRODUCER_ARGUMENTS;

typedef struct consumer_arguments {
	int con_id;
} CONSUMER_ARGUMENTS;

void* producer(void *args);
void* consumer(void *args);
