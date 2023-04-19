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

void* consumer(void* ptr) {

    CONSUMER* data = (CONSUMER *)ptr;
    ConsumerType type;
    //sem_wait(&data->broker->finished);
    while (true) {
        /*
         * Ok, so for consuming we want to be pulling a trade request from the end
         * of the queue, and giving it to either blockchain x or y, on a varying
         * basis. The order must be FIFO, which means we'll be pulling from the
         * back of the buffer queue since that is what was put in first. We will
         * do the same flow of pushing, where I place both if statements which
         * belong to the respective crypto/blockchain on top of each other. The
         * sleep cycle will determine the variance between the two.
         */
        sem_wait(&data->broker->full); // wait until there is an item in the buffer

        usleep(data->consumerTime*1000);
        pthread_mutex_lock(&data->broker->bufferMutex); // acquire buffer mutex before removing item from buffer

        /* Process into Blockchain X or Y */
        if(!data->broker->boundedBuffer.empty()){
            pthread_mutex_unlock(&data->broker->bufferMutex);
            /* Detect what type of crypto we are popping */
            pthread_mutex_lock(&data->broker->bufferMutex); // release buffer mutex after removing item from buffer
            if(data->broker->boundedBuffer.front() == Bitcoin){
                sem_post(&data->broker->btcEmpty);
                data->broker->consumed[data->consumerType][Bitcoin]++;
                data->broker->boundedBuffer.pop(); /* Pops bitcoin request from back of queue*/
                pthread_mutex_unlock(&data->broker->bufferMutex); // release buffer mutex after removing item from buffer
                data->broker->inRequestQueue[Bitcoin]--; /* Since we popped a bitcoin request we can decrement */
                if(data->broker->numRequests <= data->broker->maxRequests){
                    log_request_removed(data->consumerType, Bitcoin, data->broker->consumed[data->consumerType], data->broker->inRequestQueue);
                }
            }else{
                pthread_mutex_unlock(&data->broker->bufferMutex); // release buffer mutex after removing item from buffer
            }
            pthread_mutex_lock(&data->broker->bufferMutex); // release buffer mutex after removing item from buffer
            if(data->broker->boundedBuffer.front() == Ethereum){
                data->broker->consumed[data->consumerType][Ethereum]++;
                data->broker->boundedBuffer.pop(); /* Pops ethereum request from back of queue*/
                pthread_mutex_unlock(&data->broker->bufferMutex); // release buffer mutex after removing item from buffer
                data->broker->inRequestQueue[Ethereum]--; /* Since we popped a ethereum request we can decrement */
                if(data->broker->numRequests <= data->broker->maxRequests){
                    log_request_removed(data->consumerType, Ethereum, data->broker->consumed[data->consumerType], data->broker->inRequestQueue);
                }
            }else{
                pthread_mutex_unlock(&data->broker->bufferMutex); // release buffer mutex after removing item from buffer
            }
        }else{
            pthread_mutex_unlock(&data->broker->bufferMutex); // release buffer mutex after removing item from buffer

        }
        //pthread_mutex_unlock(&data->broker->bufferMutex); // release buffer mutex after removing item from buffer

        sem_post(&data->broker->empty); // signal that there is now space in the buffer
        pthread_mutex_lock(&data->broker->bufferMutex);
        if(data->broker->numRequests >= data->broker->maxRequests && data->broker->boundedBuffer.empty()){
            pthread_mutex_unlock(&data->broker->bufferMutex); // release buffer mutex after removing item from buffer
            //pthread_cond_signal(&data->broker->done);
            //std::cout<<"made it here"<<std::endl;
            break;
        }else{
            pthread_mutex_unlock(&data->broker->bufferMutex); // release buffer mutex after removing item from buffer

        }
    }
    sem_post(&data->broker->finished);
    return NULL;
}