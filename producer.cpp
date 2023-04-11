//
// Created by forem on 4/10/2023.
//
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>

#include "log.h"
#include "producer.h"
#include "consumer.h"
#include "broker.h"

//void* producer(void* ptr, void* reqType) {
void* producer(void* ptr){
    BROKER* broker = (BROKER *)ptr;
    //RequestType* type = (RequestType*)reqType;
    RequestType type = broker->requestType;
    std::cout << "Entering production..." << std::endl;
    while (true){
        std::cout << " I am Producing to type: " << type << std::endl;

        // generate new item
        //item = generate_new_item();

        // wait until there is space in the buffer
        sem_wait(&broker->empty);

        // acquire buffer mutex before adding item to buffer
        pthread_mutex_lock(&broker->bufferMutex);
        if(broker->boundedBuffer.size() <= BUFFER_SIZE){
            if(type == Bitcoin && broker->numBitcoinRequestsInQueue < MAX_BTC_REQUESTS){
                broker->boundedBuffer.push(Bitcoin);
            }
            else if (type == Ethereum){
                broker->boundedBuffer.push(Ethereum);
            }
        }

        // add BTC or ETH trade request to buffer
        //add_item_to_buffer(item);
        //broker->boundedBuffer.push(request);

        // release buffer mutex after adding item to buffer
        pthread_mutex_unlock(&broker->bufferMutex);

        // signal that a new item has been added to the buffer
        sem_post(&broker->full);
    }
}