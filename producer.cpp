 
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#include "log.h"
#include "producer.h"
#include "consumer.h"
#include "broker.h"


void* producer(void* ptr){

    PRODUCER* data = (PRODUCER *)ptr; // producer data / broker instance

    while (true) {

        /**
        * Here, we check if the request type is bitcoin so we can decide if we need to decrement
        * the number of available bitcoin slots, and/or the number of overall slots in the buffer.
        */
        usleep(data->requestTime * 1000);
        if(data->requestType == Bitcoin) {

            sem_wait(&data->broker->btc);
        }
        //usleep(data->requestTime * 1000);
        sem_wait(&data->broker->empty); // Amount of empty slots in the buffer gets decreased [empty++]

        //usleep(data->requestTime * 1000); // Simulate production time by sleeping for specified request-time.

        /**
        * Consecutively, we will push the request type to buffer, increment number of requests,
        *  then also add to produced[type], then log the request for printing. We do this all
        *  under control of the mutex above the if-statement.
        */
        pthread_mutex_lock(&data->broker->bufferMutex);
        if (data->broker->numRequests < data->broker->maxRequests)
        {
            data->broker->boundedBuffer.push(data->requestType);
            data->broker->numRequests++;
            data->broker->produced[data->requestType]++;
            data->broker->inRequestQueue[data->requestType]++;
            log_request_added(data->requestType, data->broker->produced,
                              data->broker->inRequestQueue);
        }
        pthread_mutex_unlock(&data->broker->bufferMutex);

        sem_post(&data->broker->full); // Amount of slots occupied in the buffer gets increased [full++]

        if(data->broker->numRequests >= data->broker->maxRequests){ // Check if we can break out of while loop.
            break;
        }
    }
    return NULL;
}