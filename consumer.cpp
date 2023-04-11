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

#include "consumer.h"
void* consumer(void* ptr) {
    BROKER* broker = (BROKER *)ptr;
    //ConsumerType * type = (ConsumerType *)conType;
    ConsumerType type = broker->consumerType;
    while (true) {
        std::cout << " I am Consuming to type: " << type << std::endl;
        // wait until there is an item in the buffer
        sem_wait(&broker->full);

        // acquire buffer mutex before removing item from buffer
        pthread_mutex_lock(&broker->bufferMutex);

        // remove item from buffer
        //item = remove_item_from_buffer();
        if(broker)

        // release buffer mutex after removing item from buffer
        pthread_mutex_unlock(&broker->bufferMutex);

        // signal that there is now space in the buffer
        sem_post(&broker->empty);

        // process the BTC or ETH on blockchain X or blockchain Y
        //process_item(item);
    }
}