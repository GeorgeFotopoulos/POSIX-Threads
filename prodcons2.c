#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "prodcons.h"

circular_buffer cb;

int main(int argc, char *argv[]) {
	if(argc == 6) {
		f2 = fopen("prod_in.txt", "w");
		f1 = fopen("cons_out.txt", "w");
		producerscount=atoi(argv[1]);
		endLoop = atoi(argv[1]) * atoi(argv[4]);
		
		cb_init(&cb, atoi(argv[3]), sizeof(int));
		
		pthread_mutex_init(&mu, NULL);
		pthread_cond_init(&notReady, NULL);
		
		pthread_t pro[atoi(argv[1])];
		PRODUCER_ARGUMENTS producerArgs[atoi(argv[1])];
		for (int i=1; i<=atoi(argv[1]); i++){
			producerArgs[i-1].prod_id = i;
			producerArgs[i-1].seed = atoi(argv[5]);
			producerArgs[i-1].total = atoi(argv[4]);
			rc = pthread_create(&pro[i-1], NULL, producer, &producerArgs[i-1]);
		}
		
		CONSUMER_ARGUMENTS consumerArgs[atoi(argv[2])];
		pthread_t con[atoi(argv[2])];
		for (int i=1; i<=atoi(argv[2]); i++) {
			consumerArgs[i-1].con_id = i;
			rc = pthread_create(&con[i-1], NULL, consumer, &consumerArgs[i-1]);
		}
		for(int i=1; i<=atoi(argv[1]); i++) pthread_join(pro[i-1], NULL);
		for(int i=1; i<=atoi(argv[2]); i++) pthread_join(con[i-1], NULL);
		
		pthread_mutex_destroy(&mu);
		pthread_cond_destroy(&notReady);
		cb_free(&cb);
		return 0;
	} else {
		printf("Wrong argument format!\n");
	}
}
