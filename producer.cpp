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

    BROKER* broker = (BROKER *)ptr;
    RequestType type;

    while (true){

        /*
         * I think we want to focus on using the usleep() function to loop between BTC
         * and ETH pushes to the buffer, whilst checking if there
         * is available space on the buffer (respectively). We will
         * do BTC pushes first and then and ETH push first. In the
         * case that there is no delay for either, we must ensure
         * that it does not get stuck only publishing BTC pushes.
         */

        sem_wait(&broker->empty); // wait until there is space in the buffer

        pthread_mutex_lock(&broker->bufferMutex); // acquire buffer mutex before adding item to buffer

        //std::cout << "Test - Current Number of BTC requests right now: " << broker->inRequestQueue[Bitcoin] << std::endl;
        //std::cout << "Test - Current Number of ETH requests right now: " << broker->inRequestQueue[Ethereum] << std::endl;

        /* BTC Request Push */
        if ( broker->boundedBuffer.size() <= BUFFER_SIZE && broker->numBitcoinRequestsInQueue < MAX_BTC_REQUESTS && broker->numRequests <= broker->maxRequests){
            broker->boundedBuffer.push(Bitcoin);
            /*Increase number of total BTC requests, and increase number of BTC requests in queue*/
            broker->numBitcoinRequestsInQueue++;
            broker->numRequests++;
            broker->produced[Bitcoin]++;
            broker->inRequestQueue[Bitcoin]++;
            log_request_added(Bitcoin,broker->produced, broker->inRequestQueue);
            /* TODO: Do we sleep() here, or before the above counter variable calls? Pretty sure down here is correct. */
            broker->BTC_reqTime * 1000000;
            usleep(broker->BTC_reqTime);
        }

        /*TODO: Figure out if we need mutex locking/unlocking BETWEEN BTC/ETH pushes to buffer.
         * Keeping 'em here for now.*/
        pthread_mutex_unlock(&broker->bufferMutex);

        sem_post(&broker->full); // signal that a new item has been added to the buffer

        sem_wait(&broker->empty); // wait until there is space in the buffer

        pthread_mutex_lock(&broker->bufferMutex);

        /* ETH Request Push */
        if ( broker->boundedBuffer.size() <= BUFFER_SIZE  && broker->numRequests <= broker->maxRequests){
            broker->boundedBuffer.push(Ethereum);
            /*Increase number of total ETH requests, and increase number of ETH requests in queue*/
            broker->numEthereumRequestsInQueue++;
            broker->numRequests++;
            broker->produced[Ethereum]++;
            broker->inRequestQueue[Ethereum]++;
            log_request_added(Ethereum,broker->produced, broker->inRequestQueue);

            broker->ETH_reqTime * 1000000;
            usleep(broker->ETH_reqTime);
        }

        pthread_mutex_unlock(&broker->bufferMutex); // release buffer mutex after adding item to buffer

        sem_post(&broker->full); // signal that a new item has been added to the buffer
    }
}