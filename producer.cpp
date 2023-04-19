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
    while (data->broker->numRequests < data->broker->maxRequests) {
        if(data->requestType==Bitcoin && data->broker->inRequestQueue[data->requestType] >= MAX_BTC_REQUESTS){
            sem_wait(&data->broker->btcEmpty);
            // Here, we want to start a wait for btc requests only
            sem_wait(&data->broker->empty); // wait until there is space in the buffer
            usleep(data->requestTime*1000);
            pthread_mutex_lock(&data->broker->bufferMutex); // acquire buffer mutex before adding item to buffer
            if (data->broker->boundedBuffer.size() <= BUFFER_SIZE &&
                data->broker->numRequests < data->broker->maxRequests &&
                data->broker->inRequestQueue[Bitcoin] < MAX_BTC_REQUESTS) {
                pthread_mutex_unlock(&data->broker->bufferMutex); // acquire buffer mutex before adding item to buffer

                pthread_mutex_lock(&data->broker->bufferMutex); // acquire buffer mutex before adding item to buffer
                data->broker->boundedBuffer.push(Bitcoin);
                pthread_mutex_unlock(&data->broker->bufferMutex);
                data->broker->numRequests++;
                data->broker->produced[Bitcoin]++;
                data->broker->inRequestQueue[Bitcoin]++;

                if(data->broker->numRequests <= data->broker->maxRequests){
                    log_request_added(Bitcoin, data->broker->produced, data->broker->inRequestQueue);
                }
            }else{
                pthread_mutex_unlock(&data->broker->bufferMutex); // acquire buffer mutex before adding item to buffer
            }
            //pthread_mutex_unlock(&data->broker->bufferMutex);
            //sem_post(&data->broker->maxReq);
            sem_post(&data->broker->full);
            sem_wait(&data->broker->btcFull);
        }

        sem_wait(&data->broker->empty); // wait until there is space in the buffer
        usleep(data->requestTime*1000);
        pthread_mutex_lock(&data->broker->bufferMutex); // acquire buffer mutex before adding item to buffer
        /* BTC or ETH Request Push */
        if (data->broker->boundedBuffer.size() <= BUFFER_SIZE &&
            data->broker->numRequests < data->broker->maxRequests &&
            data->broker->inRequestQueue[Bitcoin] < MAX_BTC_REQUESTS) {
            pthread_mutex_unlock(&data->broker->bufferMutex);
            //if(data->requestType == Bitcoin && data->broker->inRequestQueue[data->requestType] >= MAX_BTC_REQUESTS){
            //    while(data->broker->inRequestQueue[data->requestType] >= MAX_BTC_REQUESTS);
            //}
            pthread_mutex_lock(&data->broker->bufferMutex); // acquire buffer mutex before adding item to buffer
            data->broker->boundedBuffer.push(data->requestType);
            pthread_mutex_unlock(&data->broker->bufferMutex);
            /*Increase number of total BTC requests, and increase number of BTC requests in queue*/
            data->broker->numRequests++;
            //pthread_mutex_lock(&data->broker->producerMutex);
            data->broker->produced[data->requestType]++;
            //pthread_mutex_unlock(&data->broker->producerMutex);
            data->broker->inRequestQueue[data->requestType]++;
            //pthread_mutex_lock(&data->broker->producerMutex);
            if(data->broker->numRequests  <= data->broker->maxRequests){
                log_request_added(data->requestType, data->broker->produced, data->broker->inRequestQueue);
            }
            if(data->requestType == Bitcoin){
                sem_post(&data->broker->btcFull); // signal that btc limit is nearing capacity
            }
        }
        else{
            pthread_mutex_lock(&data->broker->bufferMutex);
        }
        //pthread_mutex_unlock(&data->broker->bufferMutex);
        //sem_post(&data->broker->maxReq);
        sem_post(&data->broker->full); // signal that a new item has been added to the buffer
        if(data->broker->numRequests >= data->broker->maxRequests){
            break;
        }
    }
    return NULL;
}