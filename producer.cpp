/*
 * created on 4/10/2023
 * AUTHORS:
 * Logan Foreman REDID: 825056655
 * Shane Wechsler REDID:
 */
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "log.h"
#include "producer.h"
#include "consumer.h"
#include "broker.h"


void* producer(void* ptr){

    PRODUCER* data = (PRODUCER *)ptr;

    while (true) {

        /*
         * I think we want to focus on using the usleep() function to loop between BTC
         * and ETH pushes to the buffer, whilst checking if there
         * is available space on the buffer (respectively). We will
         * do BTC pushes first and then and ETH push first. In the
         * case that there is no delay for either, we must ensure
         * that it does not get stuck only publishing BTC pushes.
         */
        if(data->requestType == Bitcoin){
            sem_wait(&data->broker->btc);
        }
        sem_wait(&data->broker->empty); // wait until there is space in the buffer
        usleep(data->requestTime*1000);
        pthread_mutex_lock(&data->broker->bufferMutex); // acquire buffer mutex before adding item to buffer

        //std::cout << "Test - Current Number of BTC requests right now: " << broker->inRequestQueue[Bitcoin] << std::endl;
        //std::cout << "Test - Current Number of ETH requests right now: " << broker->inRequestQueue[Ethereum] << std::endl;

        /* BTC Request Push */
        if (data->broker->boundedBuffer.size() <= BUFFER_SIZE &&
            data->broker->numRequests < data->broker->maxRequests) {
            data->broker->boundedBuffer.push(data->requestType);
            pthread_mutex_unlock(&data->broker->bufferMutex);
            /*Increase number of total BTC requests, and increase number of BTC requests in queue*/
            data->broker->numRequests++;
            data->broker->produced[data->requestType]++;
            data->broker->inRequestQueue[data->requestType]++;
            log_request_added(Bitcoin, data->broker->produced, data->broker->inRequestQueue);
                /* TODO: Do we sleep() here, or before the above counter variable calls? Pretty sure down here is correct. */
        }
        pthread_mutex_unlock(&data->broker->bufferMutex);

        /*TODO: Figure out if we need mutex locking/unlocking BETWEEN BTC/ETH pushes to buffer.
         * Keeping 'em here for now.*/

        sem_post(&data->broker->full); // signal that a new item has been added to the buffer
    }
}