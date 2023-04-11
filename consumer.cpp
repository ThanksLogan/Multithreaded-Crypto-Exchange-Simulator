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

    BROKER* broker = (BROKER *)ptr;
    ConsumerType type;

    while (true) {

        /*
         * Ok, so for consuming we want to be pulling a trade request from the end
         * of the queue, and giving it to either blockchain x or y, on a varying
         * basis. The order must be FIFO, which means we'll be pulling from the
         * back of the buffer queue since that is what was put in first. We will
         * do the same flow of pushing, where I place both if statements which
         * belong to the respective crypto/blockchain on top of each other. The
         * sleep cycle will determine the variance between the two. If there is
         * nothing in the queue, then our consumer must simply wait until there
         * is. One danger to this is that we don't want our consumer to hold onto
         * the semaphore forever (deadlock, I think) when there is nothing
         * in the queue.
         *
        *   HINT: One of the difficult problems for students is how to stop the program.
            Imagine that the Blockchain X consumer thread consumes the last request.
            The Blockchain Y consumer thread could be asleep then wait for a request
            becoming available in the queue to consume, and thus never able to exit.
            The trick here is to use a barrier (see precedence constraint in lecture
            slide) in the main thread that is signaled by the consumer that consumed
            the last request. The main thread should block until consumption is
            complete and kill the child threads (or simply exit and let the OS kill child
            threads). At the end of consumer thread logic, you would want to check if the broker
            queue is empty and if the production limit is reached; if so, signal the
            barrier. Notice there could be the case that one of the consumer threads is
            blocked, the other consumer thread consumes the last request in the queue,
            and it sees both the broker queue being empty and the production limit was
            reached, then it can signal the barrier (main thread is waiting on) to unlock
            the main thread. Then with main thread exiting, it would automatically force
            the blocked consumer thread to exit.
         */

        sem_wait(&broker->full); // wait until there is an item in the buffer

        pthread_mutex_lock(&broker->bufferMutex); // acquire buffer mutex before removing item from buffer

        /* Process into Blockchain X */
        if(!broker->boundedBuffer.empty()){

            broker->consumed[BlockchainX]++; /* Increase number of consumed for Blockchain X*/

            /* Detect what type of crypto we are popping */
            if(broker->boundedBuffer.back() == Bitcoin){
                broker->boundedBuffer.pop(); /* Pops bitcoin request from back of queue*/
                broker->numBitcoinRequestsInQueue--;
                broker->inRequestQueue[Bitcoin]--; /* Since we popped a bitcoin request we can decrement */
                log_request_removed(BlockchainX, Bitcoin, broker->consumed, broker->inRequestQueue);
            }
            else if(broker->boundedBuffer.back() == Ethereum){
                broker->boundedBuffer.pop(); /* Pops ethereum request from back of queue*/
                broker->inRequestQueue[Ethereum]--; /* Since we popped a ethereum request we can decrement */
                log_request_removed(BlockchainX, Ethereum, broker->consumed, broker->inRequestQueue);
            }

            /* Sleep for specified time */
            broker->X_ProcessingTime * 1000000;
            usleep(broker->X_ProcessingTime);
        }

        pthread_mutex_unlock(&broker->bufferMutex); // release buffer mutex after removing item from buffer

        sem_post(&broker->empty); // signal that there is now space in the buffer

        sem_wait(&broker->full); // wait until there is an item in the buffer

        pthread_mutex_lock(&broker->bufferMutex); // acquire buffer mutex before removing item from buffer

        /* Process into Blockchain Y */
        if(!broker->boundedBuffer.empty()){

            broker->consumed[BlockchainY]++; /* Increase number of consumed for Blockchain Y*/

            /* Detect what type of crypto we are popping */
            if(broker->boundedBuffer.back() == Bitcoin){
                broker->boundedBuffer.pop(); /* Pops bitcoin request from back of queue*/
                broker->numBitcoinRequestsInQueue--;
                broker->inRequestQueue[Bitcoin]--; /* Since we popped a bitcoin request we can decrement */
                log_request_removed(BlockchainY, Bitcoin, broker->consumed, broker->inRequestQueue);
            }
            else if(broker->boundedBuffer.back() == Ethereum){
                broker->boundedBuffer.pop(); /* Pops ethereum request from back of queue*/
                broker->inRequestQueue[Ethereum]--; /* Since we popped a ethereum request we can decrement */
                log_request_removed(BlockchainY, Ethereum, broker->consumed, broker->inRequestQueue);
            }

            /* Sleep for specified time */
            broker->Y_ProcessingTime * 1000000;
            usleep(broker->Y_ProcessingTime);
        }

        pthread_mutex_unlock(&broker->bufferMutex); // release buffer mutex after removing item from buffer


        sem_post(&broker->empty); // signal that there is now space in the buffer

    }
}