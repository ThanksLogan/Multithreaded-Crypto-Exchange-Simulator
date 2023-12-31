 
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#include "log.h"
#include "producer.h"
#include "consumer.h"
#include "broker.h"

void* consumer(void* ptr) {

    CONSUMER* data = (CONSUMER *)ptr;
    //ConsumerType type;

    while (true) {

        /*
         * Ok, so for consuming we want to be pulling a trade request from the end
         * of the queue, and giving it to either blockchain x or y, on a varying
         * basis. The order must be FIFO, which means we'll be pulling from the
         * back of the buffer queue since that is what was put in first.
         */

        sem_wait(&data->broker->full); // wait until there is an item in the buffer
        usleep(data->consumerTime*1000);
        pthread_mutex_lock(&data->broker->bufferMutex); // acquire buffer mutex before removing item from buffer

        /* Process into Blockchain */
        if(!data->broker->boundedBuffer.empty()){
            /* request type(btc or eth) is whats ever at the front of the queue */
            RequestType reqType = data->broker->boundedBuffer.front();
            /* mark that we have consumed data by X/Y and BTC/ETH in the 2d array */
            data->broker->consumed[data->consumerType][reqType]++;
            data->broker->boundedBuffer.pop();
            data->broker->inRequestQueue[reqType]--;
            log_request_removed(data->consumerType, reqType, data->broker->consumed[data->consumerType], data->broker->inRequestQueue);
            pthread_mutex_unlock(&data->broker->bufferMutex);
            /* special case for bitcoin, we must signal the btc semaphore that there's room in the buffer for btc, max is 5 */
            if(reqType == Bitcoin){
                sem_post(&data->broker->btc);
                sem_post(&data->broker->empty);
            }else{
                sem_post(&data->broker->empty);
            }
        }
        else{
            pthread_mutex_unlock(&data->broker->bufferMutex);
        }
        //sem_post(&data->broker->empty); // signal that there is now space in the buffer

        /* lock to check boundedBuffer */
        pthread_mutex_lock(&data->broker->bufferMutex);
        if(data->broker->numRequests >= data->broker->maxRequests && data->broker->boundedBuffer.empty()){
            pthread_mutex_unlock(&data->broker->bufferMutex);
            /* break out of loop if we're at limit, and now more requests in the buffer */
            break;
        }else{
            pthread_mutex_unlock(&data->broker->bufferMutex);
        }
    }
    /* signal the "main" semaphore to begin to kill threads */
    sem_post(&data->broker->finished);
    return NULL;
}