#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "prodcons.h"

#define MAXCHAR 1000
int counter=0;
int producerscount;
pthread_mutex_t mu;
pthread_cond_t notReady;
circular_buffer cb;
int rc, datap, datac, flag = 1, jobDone = 0, endLoop = 0;
FILE *f2, *f1;
char *token;
char charArr[MAXCHAR];
const char delim[] = ":";

//initialize circular buffer
//capacity: maximum number of elements in the buffer
//sz: size of each element 
void cb_init(circular_buffer *cb, size_t capacity, size_t sz)
{
    cb->buffer = malloc(capacity * sz);
    if(cb->buffer == NULL){
		printf("Could not allocate memory..Exiting! \n");
		exit(1);
		}
    cb->buffer_end = (char *)cb->buffer + capacity * sz;
    cb->capacity = capacity;
    cb->count = 0;
    cb->sz = sz;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
}

void cb_free(circular_buffer *cb)
{
    free(cb->buffer);
}

void cb_push_back(circular_buffer *cb, const void *item)
{
    if(cb->count == cb->capacity) {
		printf("Access violation. Buffer is full\n");
		exit(1);
	}
    memcpy(cb->head, item, cb->sz);
    cb->head = (char*)cb->head + cb->sz;
    if(cb->head == cb->buffer_end) cb->head = cb->buffer;
    cb->count++;
}

void cb_pop_front(circular_buffer *cb, void *item)
{
    if(cb->count == 0) {
		printf("Access violation. Buffer is empty\n");
		exit(1);
	}
    memcpy(item, cb->tail, cb->sz);
    cb->tail = (char*)cb->tail + cb->sz;
    if(cb->tail == cb->buffer_end) cb->tail = cb->buffer;
    cb->count--;
}

void* producer(void *args) {
	usleep(rand() % 100000);
	PRODUCER_ARGUMENTS *producerArgs;
	producerArgs = (PRODUCER_ARGUMENTS *) args;
	while (producerArgs->total > 0) {
		usleep((rand() % 10000));
		pthread_mutex_lock(&mu);
		if(cb.capacity == cb.count) {
			rc = pthread_cond_wait(&notReady, &mu);
		} else {
			rc = pthread_cond_broadcast(&notReady);
			datap = rand_r(&producerArgs->seed) % ((rand()%133) + 1);
			cb_push_back(&cb, &datap);
			fprintf(f2, "Producer %d: %d\n", producerArgs->prod_id, datap);
			producerArgs->total--;
			endLoop--;
		}
		if(endLoop == 0 && jobDone != 1) {
			jobDone = 1;
			fclose(f2);
			rc = pthread_cond_broadcast(&notReady);
		}
		while (producerArgs->total==0 && endLoop>0) pthread_cond_wait(&notReady, &mu);
		if (jobDone == 1) {
			f2 = fopen("prod_in.txt", "r");
			char temp[32] = "Producer ";			
			char prodNum[32];
			sprintf(prodNum, "%d", producerArgs->prod_id);
			strcat(temp, prodNum);
			printf("%s:", temp);
			while (fgets(charArr, MAXCHAR, f2) != NULL) {
				token = strtok(charArr, delim);
				if (strcmp(token, temp) == 0){
					token = strtok(NULL, "\n");
					printf(" %s", token);
					while (fgets(charArr, MAXCHAR, f2) != NULL) {
						token = strtok(charArr, delim);
						if (strcmp(token, temp) == 0){
							token = strtok(NULL, "\n");
							printf(", %s", token);
						}
					}
					printf("\n");
					break;
				}
			}
			fclose(f2);
			counter++;
		}
		if(counter==producerscount)rc = pthread_cond_broadcast(&notReady);
		pthread_mutex_unlock(&mu);
	}
	return 0;
}

void* consumer(void *args) {
	usleep(rand() % 10000);
	pthread_mutex_lock(&mu);
	if (cb.count == 0) {
		rc = pthread_cond_wait(&notReady, &mu);
	}
	pthread_mutex_unlock(&mu);
	CONSUMER_ARGUMENTS *consumerArgs;
	consumerArgs = (CONSUMER_ARGUMENTS *) args;
	while(flag == 1) {
		usleep((rand() % 10000));
		pthread_mutex_lock(&mu);
		if(cb.count == 0 && jobDone == 0) {
			rc = pthread_cond_wait(&notReady, &mu);
		} else {
			if(flag == 1) {
				cb_pop_front(&cb, &datac);
				fprintf(f1, "Consumer %d: %d\n", consumerArgs->con_id, datac);
				rc = pthread_cond_broadcast(&notReady);
			}
		}
		if(cb.count == 0 && jobDone == 1 && flag != 0) {
			flag = 0;
			fclose(f1);
		}
	//	while (flag == 0)
		if (flag == 0) {
			if(counter!=producerscount)	 pthread_cond_wait(&notReady, &mu);
		    f1=fopen("cons_out.txt", "r");
			char temp[32] = "Consumer ";			
			char conNum[32];
			sprintf(conNum, "%d", consumerArgs->con_id);
			strcat(temp, conNum);
			while (fgets(charArr, MAXCHAR, f1) != NULL) {
				token = strtok(charArr, delim);
				if (strcmp(token, temp) == 0) {
					printf("%s:", temp);
					token = strtok(NULL, "\n");
					printf(" %s", token);
					while (fgets(charArr, MAXCHAR, f1) != NULL) {
						token = strtok(charArr, delim);
						if (strcmp(token, temp) == 0){
							token = strtok(NULL, "\n");
							printf(", %s", token);
						}
					}
					printf("\n");
					break;
				}
			}
			fclose(f1);
	
		}
		pthread_mutex_unlock(&mu);
	}
	
	return 0;
}
