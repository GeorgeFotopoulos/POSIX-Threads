#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "prodcons.h"

circular_buffer cb;

int main(int argc, char *argv[]) {
	if(argc == 6 && atoi(argv[1]) == 1 && atoi(argv[2]) == 1) {
		f2 = fopen("prod_in.txt", "w");
		f1 = fopen("cons_out.txt", "w");
		producerscount=atoi(argv[1]);
		endLoop = atoi(argv[1]) * atoi(argv[4]);
		
		PRODUCER_ARGUMENTS producerArgs;
		CONSUMER_ARGUMENTS consumerArgs;
		
		producerArgs.prod_id = 1;
		producerArgs.seed = atoi(argv[5]);
		producerArgs.total = atoi(argv[4]);
		consumerArgs.con_id = 1;
		
		cb_init(&cb, atoi(argv[3]), sizeof(int));
		
		pthread_t pro, con;
		pthread_mutex_init(&mu, NULL);
		pthread_cond_init(&notReady, NULL);
		
		rc = pthread_create(&con, NULL, consumer, &consumerArgs);
		rc = pthread_create(&pro, NULL, producer, &producerArgs);
		
		pthread_join(con, NULL);
		pthread_join(pro, NULL);

		pthread_mutex_destroy(&mu);
		pthread_cond_destroy(&notReady);
		cb_free(&cb);
		return 0;
	} else {
		printf("Wrong argument format!\n");
	}
}
